#include "cspc/encodings/direct.hpp"

#include "cspc/algorithms.hpp"
#include <gautil/functional.hpp>
#include <numeric>

namespace cspc {
namespace __internal {
auto at_least_one_clauses(
	Constraint const& constraint,
	size_t domain_size,
	std::back_insert_iterator<std::vector<Clause>> result) -> void {
	const auto arity = constraint.relation.arity();
	for (auto j = 0u; j < arity; ++j) {
		auto clause = Clause{};
		clause.reserve(domain_size);
		for (auto k = 0u; k < domain_size; ++k) {
			const auto xj_eq_relkj = constraint.variables[j] * domain_size + k;
			clause.push_back(Literal(xj_eq_relkj, REGULAR));
		}
		result++ = std::move(clause);
	}
}
auto at_most_one_clauses(
	Constraint const& constraint,
	size_t domain_size,
	std::back_insert_iterator<std::vector<Clause>> result) -> void {
	const auto arity = constraint.relation.arity();
	for (auto j = 0u; j < arity; ++j) {
		for (auto k = 0u; k < domain_size; ++k) {
			for (auto l = 0u; l < domain_size; ++l) {
				const auto xj_eq_k = constraint.variables[j] * domain_size + k;
				const auto xj_eq_l = constraint.variables[j] * domain_size + l;
				result++ = {Literal(xj_eq_k, NEGATED), Literal(xj_eq_l, NEGATED)};
			}
		}
	}
}
auto conflict_clauses(
	Constraint const& nogood,
	size_t domain_size,
	std::back_insert_iterator<std::vector<Clause>> result) -> void {
	const auto n_nogoods = nogood.relation.size();
	const auto arity = nogood.relation.arity();
	for (auto j = 0u; j < n_nogoods; ++j) {
		auto clause = Clause{};
		clause.reserve(arity);
		for (auto k = 0u; k < arity; ++k) {
			const auto xk_eq_nogoodjk = nogood.variables[k] * domain_size + nogood.relation[j][k];
			clause.push_back(Literal(xk_eq_nogoodjk, NEGATED));
		}
		result++ = std::move(clause);
	}
}
} // namespace __internal
auto direct_encoding(CSP const& csp) -> SAT {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses = gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) +
					 gautil::fold(nogoods, 0u, std::plus{}, &Constraint::relation_size);

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	std::ranges::for_each(csp.constraints(), [&](Constraint const& constraint) {
		__internal::at_least_one_clauses(constraint, domain_size, std::back_inserter(clauses));
	});
	std::ranges::for_each(csp.constraints(), [&](Constraint const& constraint) {
		__internal::at_most_one_clauses(constraint, domain_size, std::back_inserter(clauses));
	});
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		__internal::conflict_clauses(nogood, domain_size, std::back_inserter(clauses));
	});

	return SAT(std::move(clauses));
}

auto multivalued_direct_encoding(CSP const& csp) -> SAT {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses = gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) +
					 gautil::fold(nogoods, 0u, std::plus{}, &Constraint::relation_size);

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	std::ranges::for_each(csp.constraints(), [&](Constraint const& constraint) {
		__internal::at_least_one_clauses(constraint, domain_size, std::back_inserter(clauses));
	});
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		__internal::conflict_clauses(nogood, domain_size, std::back_inserter(clauses));
	});

	return SAT(std::move(clauses));
}
} // namespace cspc