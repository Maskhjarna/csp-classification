#include <chrono>
#include <cspc/algorithms.hpp>
#include <cspc/kissat.hpp>
#include <ranges>
#include <spdlog/spdlog.h>

auto duration_to_precise_ms(auto duration) -> double {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / std::pow(10, 6);
}

auto siggers_all_nary_on_domain(size_t n, size_t domain_size) -> void {
	const auto siggers = cspc::siggers_operation();

	// start timer
	const auto time_before = std::chrono::system_clock::now();

	// set up relations
	const auto relations = cspc::all_nary_relations(n, domain_size).value();
	const auto time_relations_done = std::chrono::system_clock::now();

	// create meta-CSP
	auto csps = std::vector<CSP>{};
	csps.reserve(relations.size());
	std::ranges::transform(relations, std::back_inserter(csps), [&](Relation const& relation) {
		return cspc::to_preserves_operation_csp(siggers, relation);
	});
	const auto time_csps_done = std::chrono::system_clock::now();

	// convert to SAT instances
	auto sats = std::vector<SAT>{};
	sats.reserve(csps.size());
	std::ranges::transform(csps, std::back_inserter(sats), cspc::multivalued_direct_encoding);
	const auto time_sats_done = std::chrono::system_clock::now();

	// solve SAT instances
	auto satisfiability = std::vector<Satisfiability>{};
	satisfiability.reserve(sats.size());
	std::ranges::transform(sats, std::back_inserter(satisfiability), solvers::solve_kissat);
	const auto time_solved_done = std::chrono::system_clock::now();

	// calculate durations
	const auto time_elapsed_relations_ms =
		duration_to_precise_ms(time_relations_done - time_before);
	const auto time_elapsed_csps_ms = duration_to_precise_ms(time_csps_done - time_relations_done);
	const auto time_elapsed_sats_ms = duration_to_precise_ms(time_sats_done - time_csps_done);
	const auto time_elapsed_solve_ms = duration_to_precise_ms(time_solved_done - time_sats_done);
	const auto time_elapsed_total_ms = duration_to_precise_ms(time_solved_done - time_before);

	// print results
	spdlog::info("Results:");
	spdlog::info("{:<10} │ {}", "Class", "Relation");
	spdlog::info("───────────┼─────────");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i] == SATISFIABLE ? "P" : "NP-hard";
		spdlog::info("{:<10} │ {}", label, relations[i]);
	});

	// print time profile
	spdlog::info("");
	spdlog::info("Time profile:");
	spdlog::info("{:<30} │ {}", "Task", "Time");
	spdlog::info("───────────────────────────────┼─────────────");
	spdlog::info("{:<30} │ {:>10.2f}ms", "Constructing relations", time_elapsed_relations_ms);
	spdlog::info("{:<30} │ {:>10.2f}ms", "Constructing Meta-CSPs", time_elapsed_csps_ms);
	spdlog::info("{:<30} │ {:>10.2f}ms", "Constructing SATs", time_elapsed_sats_ms);
	spdlog::info("{:<30} │ {:>10.2f}ms", "Solving SATs", time_elapsed_solve_ms);
	spdlog::info("┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┼┄┄┄┄┄┄┄┄┄┄┄┄┄");
	spdlog::info("{:<30} │ {:>10.2f}ms", "Total", time_elapsed_total_ms);
}
