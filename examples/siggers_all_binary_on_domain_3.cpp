#include "kissat.hpp"
#include "polymorphisms.hpp"
#include "util.hpp"
#include <algorithm>
#include <ranges>
#include <utility>

constexpr auto DOMAIN_SIZE = 3;

auto main() -> int {
	const auto relations = poly::factory::all_binary_relations(DOMAIN_SIZE).value();
	const auto siggers = poly::factory::siggers_operation(DOMAIN_SIZE);
	auto csps = std::vector<std::optional<CSP>>(relations.size());
	std::ranges::transform(relations, csps.begin(), [&](Relation const& relation) {
		return poly::to_preserves_operation_csp(DOMAIN_SIZE, siggers, relation);
	});

	spdlog::info("Converting CSP instances to SAT instances...");
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	std::ranges::transform(
		csps, sats.begin(), util::lift_optional<CSP, SAT>(poly::multivalued_direct_encoding));

	spdlog::info("Solving SAT instances...");
	auto satisfiability = std::vector<std::optional<Satisfiability>>(sats.size());
	std::ranges::transform(
		sats, satisfiability.begin(),
		util::lift_optional<SAT, Satisfiability>(solvers::solve_kissat));

	spdlog::info("Printing results...");
	std::ranges::for_each(std::views::iota(0u, satisfiability.size()), [&](size_t i) {
		const auto label = satisfiability[i].value() == SATISFIABLE ? "P\t" : "NP-hard";
		spdlog::info("{}\t| {}", label, relations[i]);
	});

	return EXIT_SUCCESS;
}