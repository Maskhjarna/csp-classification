#include "minizinc.hpp"
#include "util.hpp"
#include <filesystem>
#include <fstream>
#include <numeric>

namespace solvers {
auto csp_to_minizinc(CSP const& csp) -> std::optional<std::string> {
	const auto header = fmt::format(
		"set of int: DOM = 0..{domain_size};\n"
		"int: n = {n_variables};\n"
		"\n"
		"array [0..n] of var DOM: data;\n"
		"\n",
		fmt::arg("n_variables", csp.n_variables() - 1),
		fmt::arg("domain_size", csp.domain_size() - 1));
	auto constraint_strings = std::vector<std::string>(csp.constraints().size());
	std::ranges::transform(
		csp.constraints(), constraint_strings.begin(),
		[](Constraint const& constraint) -> std::string {
			switch (constraint.tag) {
			case EQ:
				return fmt::format(
					"constraint data[{}] == data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case NE:
				return fmt::format(
					"constraint data[{}] != data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case GT:
				return fmt::format(
					"constraint data[{}] > data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case GE:
				return fmt::format(
					"constraint data[{}] >= data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case LT:
				return fmt::format(
					"constraint data[{}] < data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case LE:
				return fmt::format(
					"constraint data[{}] <= data[{}];\n", constraint.variables[0],
					constraint.variables[1]);
			case IS:
				return fmt::format(
					"constraint data[{}] = {};\n", constraint.variables[0], constraint.relation[0]);
			case OTHER: {
				auto result = std::vector<std::string>{};
				for (auto row : constraint.relation) {
					auto literals = std::vector<std::string>(constraint.variables.size());
					for (auto i = 0u; i < constraint.variables.size(); ++i) {
						literals[i] =
							fmt::format("data[{}] == {}", constraint.variables[i], row[i]);
					}
					result.push_back(fmt::format("{}", fmt::join(literals, " /\\ ")));
				}
				return fmt::format("constraint ({});\n", fmt::join(result, " \\/ "));
			}
			}
			assert(false); // malformed input
		});
	const auto result =
		header +
		std::accumulate(constraint_strings.begin(), constraint_strings.end(), std::string{}) +
		"\nsolve satisfy;\noutput [];";
	return result;
}

auto write_csp_as_minizinc(CSP const& csp) -> std::optional<std::string> {
	const auto path = std::filesystem::path{EXPORT_DIR + "csp.mzn"};
	if (!std::filesystem::is_directory(path.parent_path()) &&
		!std::filesystem::create_directories(path.parent_path())) {
		spdlog::error("Failed to create output directory");
		return std::nullopt;
	}
	const auto maybe_output = csp_to_minizinc(csp);
	if (!maybe_output.has_value()) {
		spdlog::error("Failed to create MiniZinc input from CSP");
		return std::nullopt;
	}
	auto const& output = maybe_output.value();
	auto ofs = std::ofstream(path);
	if (!ofs.write(output.data(), output.size())) {
		spdlog::error("Failed to write file");
		return std::nullopt;
	}
	return path;
}

auto solve_minizinc(CSP const& csp) -> std::optional<Satisfiability> {
	static constexpr auto MINIZINC_OUTPUT_SATISFIABLE = "----------\n";
	static constexpr auto MINIZINC_OUTPUT_UNSATISFIABLE = "=====UNSATISFIABLE=====\n";

	const auto maybe_path = write_csp_as_minizinc(csp);
	if (!maybe_path.has_value()) {
		spdlog::error("Failed to write MiniZinc input");
		return std::nullopt;
	}

	const auto command =
		fmt::format("cat {} | minizinc --solver gecode --input-from-stdin", maybe_path.value());

	const auto maybe_minizinc_output = util::call(command);
	if (!maybe_minizinc_output.has_value()) {
		spdlog::error("Failed executing MiniZinc");
		return std::nullopt;
	}
	auto const& minizinc_output = maybe_minizinc_output.value();

	if (minizinc_output == MINIZINC_OUTPUT_SATISFIABLE) {
		return SATISFIABLE;
	} else if (minizinc_output == MINIZINC_OUTPUT_UNSATISFIABLE) {
		return UNSATISFIABLE;
	}
	spdlog::error("Unexpected output from MiniZinc: {}", minizinc_output);
	return std::nullopt;
}

} // namespace solvers
