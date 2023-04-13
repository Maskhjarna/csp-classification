#include "common.hpp"

#include <cspc/algorithms.hpp>
#include <cspc/kissat.hpp>
#include <gautil/functional.hpp>

auto siggers_all_nary_on_domain(size_t n, size_t domain_size) -> void {
	const auto siggers = cspc::siggers_operation();

	spdlog::info("Setting up relations...");
	const auto relations = cspc::all_nary_relations(n, domain_size).value();
	auto csps = std::vector<std::optional<CSP>>(relations.size());
	transform_and_print_progress(relations, csps.begin(), [&](Relation const& relation) {
		return cspc::to_preserves_operation_csp(siggers, relation);
	});

	spdlog::info("Converting CSP instances to SAT instances...");
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	transform_and_print_progress(
		csps, sats.begin(), gautil::lift_optional<CSP, SAT>(cspc::multivalued_direct_encoding));

	spdlog::info("Solving SAT instances...");
	auto satisfiability = std::vector<std::optional<Satisfiability>>(sats.size());
	transform_and_print_progress(
		sats, satisfiability.begin(),
		gautil::lift_optional<SAT, Satisfiability>(solvers::solve_kissat));

	spdlog::info("Printing results...");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i].value() == SATISFIABLE ? "P\t" : "NP-hard";
		spdlog::info("{}\t| {}", label, relations[i]);
	});
}
