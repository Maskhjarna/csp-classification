#include "common.hpp"
#include "cspc/encodings/direct.hpp"
#include "gautil/functional.hpp"
#include <chrono>
#include <cspc/algorithms.hpp>
#include <cspc/kissat.hpp>
#include <ranges>
#include <spdlog/spdlog.h>

auto duration_to_precise_ms(auto duration) -> f64 {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / std::pow(10, 6);
}

auto encoding_siggers_checker() -> cspc::polymorphism_checker {
	return cspc::create_encoding_solver(
		cspc::siggers_operation(), cspc::multivalued_direct_encoding, cspc::kissat_is_satisfiable);
}

auto check_single(cspc::relation const& relation, cspc::polymorphism_checker checker) -> void {
	const auto time_before = std::chrono::system_clock::now();
	const auto satisfiable = checker(relation);
	const auto time_after = std::chrono::system_clock::now();

	spdlog::info("Class: {}", satisfiable ? "P" : "NP-comp");
	spdlog::info("Job took {} ms", duration_to_precise_ms(time_after - time_before));
}

auto check_all_nary_on_domain(size_t n, size_t domain_size, cspc::polymorphism_checker checker)
	-> void {
	const auto time_before = std::chrono::system_clock::now();

	// set up relations
	const auto relations = cspc::all_nary_relations(n, domain_size);
	const auto time_relations_done = std::chrono::system_clock::now();

	// create Meta-CSPs, convert to SAT instances, and solve
	auto satisfiability = std::vector<cspc::satisfiability>{};
	satisfiability.reserve(relations.size());

	// std::ranges::transform but with a progress bar
	gautil::transform_and_print_progress(relations, std::back_inserter(satisfiability), checker);
	const auto time_solved_done = std::chrono::system_clock::now();

	// calculate durations
	const auto time_elapsed_relations_ms =
		duration_to_precise_ms(time_relations_done - time_before);
	const auto time_elapsed_solve_ms =
		duration_to_precise_ms(time_solved_done - time_relations_done);
	const auto time_elapsed_total_ms = duration_to_precise_ms(time_solved_done - time_before);

	// print results
	spdlog::info("Results:");
	spdlog::info("{:<10} │ {}", "Class", "cspc::relation");
	spdlog::info("───────────┼─────────");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i] == cspc::SATISFIABLE ? "P" : "NP-hard";
		spdlog::info("{:<10} │ {}", label, relations[i]);
	});

	// print time profile
	spdlog::info("");
	spdlog::info("Time profile:");
	spdlog::info("{:<30} │ {}", "Task", "Time");
	spdlog::info("───────────────────────────────┼─────────────");
	spdlog::info("{:<30} │ {:>10.2f}ms", "Constructing relations", time_elapsed_relations_ms);
	spdlog::info("{:<30} │ {:>10.2f}ms", "Constructing and solving SATs", time_elapsed_solve_ms);
	spdlog::info("┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┼┄┄┄┄┄┄┄┄┄┄┄┄┄");
	spdlog::info("{:<30} │ {:>10.2f}ms", "Total", time_elapsed_total_ms);
}