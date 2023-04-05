#include "common.hpp"

#include "kissat.hpp"
#include "polymorphisms.hpp"
#include "util.hpp"

auto siggers_all_nary_on_domain(size_t n, size_t domain_size) -> void {
	const auto siggers = poly::factory::siggers_operation();

	spdlog::info("Setting up relations...");
	const auto relations = poly::factory::all_nary_relations(n, domain_size).value();
	auto csps = std::vector<std::optional<CSP>>(relations.size());
	transform_and_print_progress(relations, csps.begin(), [&](Relation const& relation) {
		return poly::to_preserves_operation_csp(siggers, relation);
	});

	spdlog::info("Converting CSP instances to SAT instances...");
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	transform_and_print_progress(
		csps, sats.begin(), util::lift_optional<CSP, SAT>(poly::multivalued_direct_encoding));

	spdlog::info("Solving SAT instances...");
	auto satisfiability = std::vector<std::optional<Satisfiability>>(sats.size());
	transform_and_print_progress(
		sats, satisfiability.begin(),
		util::lift_optional<SAT, Satisfiability>(solvers::solve_kissat));

	spdlog::info("Printing results...");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i].value() == SATISFIABLE ? "P\t" : "NP-hard";
		spdlog::info("{}\t| {}", label, relations[i]);
	});
}
