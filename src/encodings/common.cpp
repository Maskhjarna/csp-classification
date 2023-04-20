#include "cspc/encodings/common.hpp"

namespace cspc {
namespace __internal {
auto at_least_one_clauses(
	Constraint const& constraint, size_t domain_size, std::vector<Clause>& result) -> void {
	const auto arity = constraint.relation.arity();
	for (auto j = 0u; j < arity; ++j) {
		auto clause = Clause{};
		clause.reserve(domain_size);
		for (auto k = 0u; k < domain_size; ++k) {
			const auto xj_eq_relkj = constraint.variables[j] * domain_size + k;
			clause.push_back(Literal(xj_eq_relkj, REGULAR));
		}
		result.push_back(std::move(clause));
	}
}

auto at_most_one_clauses(
	Constraint const& constraint, size_t domain_size, std::vector<Clause>& result) -> void {
	const auto arity = constraint.relation.arity();
	for (auto j = 0u; j < arity; ++j) {
		for (auto k = 0u; k < domain_size; ++k) {
			for (auto l = 0u; l < domain_size; ++l) {
				const auto xj_eq_k = constraint.variables[j] * domain_size + k;
				const auto xj_eq_l = constraint.variables[j] * domain_size + l;
				result.push_back({Literal(xj_eq_k, NEGATED), Literal(xj_eq_l, NEGATED)});
			}
		}
	}
}
} // namespace __internal
} // namespace cspc