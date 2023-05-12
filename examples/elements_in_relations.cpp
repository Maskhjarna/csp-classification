#include "common.hpp"
#include "cspc/algorithms.hpp"
#include "gautil/functional.hpp"
#include <spdlog/spdlog.h>

auto count_csp_constraint_elements(std::vector<cspc::relation> const& relations) -> size_t {
	return gautil::fold(relations, 0ul, std::plus{}, [&](auto const& relation) {
		const auto csp = cspc::to_preserves_operation_csp(cspc::siggers_operation(), relation);
		gautil::fold(csp.constraints(), 0ul, std::plus{}, &cspc::constraint::relation_size);
		return gautil::fold(csp.constraints(), 0ul, std::plus{}, &cspc::constraint::relation_size);
	});
}

auto main() -> int {
	spdlog::info(
		"All binary [0, 1]: {}", count_csp_constraint_elements(cspc::all_nary_relations(2, 2)));
	spdlog::info(
		"All binary [0, 2]: {}", count_csp_constraint_elements(cspc::all_nary_relations(2, 3)));
	spdlog::info(
		"All ternary[0, 1]: {}", count_csp_constraint_elements(cspc::all_nary_relations(3, 2)));
	spdlog::info("{{!=}} [0, 3]}}: {}", count_csp_constraint_elements({cspc::neq_relation(2, 4)}));
	spdlog::info("{{!=}} [0, 4]: {}", count_csp_constraint_elements({cspc::neq_relation(2, 5)}));
	spdlog::info(
		"{{!= ==}} [0, 3]: {}",
		count_csp_constraint_elements({cspc::neq_relation(2, 4), cspc::eq_relation(2, 4)}));

	return EXIT_SUCCESS;
}
