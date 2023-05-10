#include "cspc/minizinc.hpp"

#include "cspc/formatters.hpp"
#include <filesystem>
#include <fstream>
#include <gautil/system.hpp>
#include <numeric>
#include <spdlog/spdlog.h>

namespace cspc {
auto csp_to_minizinc(cspc::csp const& csp) -> std::string {

	const auto header = fmt::format(
		"set of int: DOM = 0..{domain_size};\n"
		"int: n = {n_variables};\n"
		"\n"
		"array [0..n] of var DOM: data;\n",
		fmt::arg("n_variables", csp.n_variables() - 1),
		fmt::arg("domain_size", csp.domain_size() - 1));

	const auto footer = "\nsolve satisfy;\noutput [];";

	auto body = std::vector<std::string>{};
	body.resize(csp.constraints().size());
	std::ranges::transform(
		csp.constraints(), std::back_inserter(body), [&](auto const& constraint) {
			switch (constraint.tag()) {
			case cspc::constraint_tag::EQ:
				return fmt::format(
					"constraint data[{}] == data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::NE:
				return fmt::format(
					"constraint data[{}] != data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::GT:
				return fmt::format(
					"constraint data[{}] > data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::GE:
				return fmt::format(
					"constraint data[{}] >= data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::LT:
				return fmt::format(
					"constraint data[{}] < data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::LE:
				return fmt::format(
					"constraint data[{}] <= data[{}];", constraint.variables()[0],
					constraint.variables()[1]);
			case cspc::constraint_tag::IS:
				return fmt::format(
					"constraint data[{}] = {};", constraint.variables()[0],
					constraint.get_relation()[0]);
			case cspc::constraint_tag::OTHER: {
				auto clauses = std::vector<std::string>{};
				clauses.reserve(constraint.get_relation().size());
				std::ranges::transform(
					constraint.get_relation(), std::back_inserter(clauses), [&](auto const& row) {
						auto literals = std::vector<std::string>(constraint.variables().size());
						for (auto i = 0u; i < constraint.variables().size(); ++i) {
							literals[i] =
								fmt::format("data[{}] == {}", constraint.variables()[i], row[i]);
						}
						return fmt::format("{}\n", fmt::join(literals, " /\\ "));
					});
				return fmt::format("\nconstraint ({});", fmt::join(clauses, " \\/ "));
			}
			}
			spdlog::critical("Unexhaustive switch");
			exit(EXIT_FAILURE);
			return std::string{};
		});

	return fmt::format("{}\n{}\n{}", header, fmt::join(body, "\n"), footer);
}

auto write_csp_as_minizinc(cspc::csp const& csp) -> std::optional<std::string> {
	const auto path = std::filesystem::path{EXPORT_FILE_PATH};
	if (!std::filesystem::is_directory(path.parent_path()) &&
		!std::filesystem::create_directories(path.parent_path())) {
		spdlog::error("Failed to create output directory");
		return std::nullopt;
	}
	const auto output = csp_to_minizinc(csp);
	auto ofs = std::ofstream(path);
	if (!ofs.write(output.data(), output.size())) {
		spdlog::error("Failed to write file");
		return std::nullopt;
	}
	return path;
}

auto minizinc_is_satisfiable(cspc::csp const& csp) -> std::optional<cspc::satisfiability> {
	static constexpr auto MINIZINC_OUTPUT_SATISFIABLE = "----------\n";
	static constexpr auto MINIZINC_OUTPUT_UNSATISFIABLE = "=====UNSATISFIABLE=====\n";

	const auto maybe_path = write_csp_as_minizinc(csp);
	if (!maybe_path.has_value()) {
		spdlog::error("Failed to write MiniZinc input");
		return std::nullopt;
	}

	const auto command = fmt::format("minizinc --solver Gecode {}", maybe_path.value());

	const auto maybe_minizinc_output = gautil::call(command);
	if (!maybe_minizinc_output.has_value()) {
		spdlog::error("Failed executing MiniZinc");
		return std::nullopt;
	}
	const auto minizinc_output = maybe_minizinc_output.value();

	if (minizinc_output == MINIZINC_OUTPUT_SATISFIABLE) {
		return cspc::SATISFIABLE;
	} else if (minizinc_output == MINIZINC_OUTPUT_UNSATISFIABLE) {
		return cspc::UNSATISFIABLE;
	}
	spdlog::error("Unexpected output from MiniZinc: {}", minizinc_output);
	return std::nullopt;
}

} // namespace cspc
