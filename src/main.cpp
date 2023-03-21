#include "kissat.hpp"
#include "minizinc.hpp"
#include "polymorphisms.hpp"
#include "src/data_structures.hpp"
#include "test.hpp"
#include "util.hpp"
#include <execution>
#include <ranges>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <utility>

constexpr auto EXECUTION_POLICY = std::execution::par;

// constexpr auto PROGRESS_BAR_LENGTH = u32{20};
// template <
// 	std::ranges::input_range Range,
// 	std::weakly_incrementable OutputIterator,
// 	std::copy_constructible Function,
// 	class Proj = std::identity>
// 	requires std::indirectly_writable<
// 				 OutputIterator,
// 				 std::indirect_result_t<
// 					 Function&,
// 					 std::projected<std::ranges::iterator_t<Range>, Proj>>>
// auto transform_and_print_progress(Range&& range, OutputIterator result, Function op, Proj proj =
// {})
// 	-> std::ranges::
// 		unary_transform_result<std::ranges::borrowed_iterator_t<Range>, OutputIterator> {
// 	// setup logger
// 	const auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
// 	auto carriage_return_logger = spdlog::logger("logger", {console_sink});
// 	carriage_return_logger.set_formatter(std::make_unique<spdlog::pattern_formatter>(
// 		"[%Y-%m-%d %H:%M:%S.%e] [%l] %v", spdlog::pattern_time_type::local, std::string("\r")));

// 	const auto distance = std::ranges::distance(range);
// 	auto input = std::ranges::begin(range);
// 	carriage_return_logger.info("[{}]", std::string(PROGRESS_BAR_LENGTH, ' '));
// 	for (auto i = 0u; i < distance; ++i) {
// 		*result++ = std::invoke(op, std::invoke(proj, *input++));
// 		const auto filled = f32(i) / distance;
// 		const auto unfilled = f32(1.0) - filled;
// 		carriage_return_logger.info(
// 			"[{}{}] {:.2f}%", std::string(filled * PROGRESS_BAR_LENGTH, '='),
// 			std::string(unfilled * PROGRESS_BAR_LENGTH, ' '), filled * 100.0);
// 	}
// 	carriage_return_logger.info("[{}]", std::string(PROGRESS_BAR_LENGTH, '='));

// 	return {input, result};
// }

struct Config {
	enum Solver {
		KISSAT = 0,
		MINIZINK,
	};
	Solver solver;
	bool run_tests;
};

struct Input {
	std::vector<CSP> csps;
	Operation operation;
};

constexpr auto DEFAULT_CONFIG = Config{
	.solver = Config::KISSAT,
	.run_tests = false,
};

auto parse_config() -> std::optional<Config> {
	// TODO parse config file
	return std::nullopt;
}

auto parse_problem() -> std::optional<Input> {
	// TODO parse problem
	return std::nullopt;
}

auto main() -> int {
	const auto config = parse_config().value_or(DEFAULT_CONFIG);

	if (config.run_tests) {
		spdlog::info("Running all tests...");
		if (!run_tests()) {
			spdlog::error("Some tests failed");
			return EXIT_FAILURE;
		}
		spdlog::info("All tests passed!");
	}

	// spdlog::info("Setting up relations and operation...");
	// const auto maybe_input = parse_problem();
	// if (maybe_input.has_value()) {
	// 	spdlog::error("Failed to parse input.");
	// 	return EXIT_FAILURE;
	// }
	// const auto input = std::move(maybe_input.value());

	// spdlog::info("Creating CSP instances...");
	// auto csps = std::vector<std::optional<CSP>>(input.csps.size());
	// std::transform(
	// 	EXECUTION_POLICY, input.csps.begin(), input.csps.end(), csps.begin(),
	// 	[&](CSP const& csp) { return poly::factory::has_polymorphism_csp(csp, input.operation); });

	constexpr auto DOMAIN_SIZE = 3;
	const auto relations = poly::factory::all_binary_relations(DOMAIN_SIZE).value();
	const auto siggers = poly::factory::siggers_operation(DOMAIN_SIZE);
	auto csps = std::vector<std::optional<CSP>>(relations.size());
	std::transform(
		EXECUTION_POLICY, relations.begin(), relations.end(), csps.begin(),
		[&](Relation const& relation) {
			return poly::to_preserves_operation_csp(DOMAIN_SIZE, siggers, relation);
		});

	// spdlog::info("Solving CSP instances...");
	// auto satisfiability = std::vector<std::optional<Satisfiability>>();
	// csps.reserve(relations.size());
	// util::transform_maybes<CSP, Satisfiability>(
	// 	csps, std::back_inserter(satisfiability), solvers::solve_minizinc);

	spdlog::info("Converting CSP instances to SAT instances...");
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	std::transform(
		EXECUTION_POLICY, csps.begin(), csps.end(), sats.begin(),
		util::lift_optional<CSP, SAT>(poly::multivalued_direct_encoding));

	spdlog::info("Solving SAT instances...");
	auto satisfiability = std::vector<std::optional<Satisfiability>>(sats.size());
	std::transform(
		EXECUTION_POLICY, sats.begin(), sats.end(), satisfiability.begin(),
		util::lift_optional<SAT, Satisfiability>(solvers::solve_kissat));

	spdlog::info("Printing results...");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i].value() == SATISFIABLE ? "P\t" : "NP-hard";
		spdlog::info("{}\t| {}", label, relations[i]);
	});

	return EXIT_SUCCESS;
}

/*
  To decide if a there exists a Siggers operation function table for a
  relation R, do the following:

  Let {f_0000, f_0001, ..., f_dddd} be a table of d*d*d*d values

  Group all table entries together that must have the same value for f to be a
  Siggers operation.

  Create an empty SAT formulation S

  for every group of functions G
	create a new clause of terms c
	for every function value f(i j k l) in G
	  for every four rows where the lhs of the relations together are (i j k
  l) and the rhs are some other values (a b c d) for every other row (x, y)
		  for every function g in G
			add the term (g(i j k l) == x and g(a b c d) == y) to c
	add the clause to S

  If S has a solution, then R has a Siggers operation as a polymorphism.

  d * d * d * d function values
  d * d * d * d selections of four rows
  d * d other rows
  * 100 lol



  are more occurances of the value on the lhs of the table than in f, add the
  constraint that f(all rows in bucket) == this value AND f(or the next etc.


*/
