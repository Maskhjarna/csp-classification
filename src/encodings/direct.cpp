#include "cspc/encodings/direct.hpp"

#include "cspc/algorithms.hpp"
#include "cspc/encodings/common.hpp"
#include <gautil/functional.hpp>
#include <numeric>

namespace cspc {

namespace __internal {
auto conflict_clauses(Constraint const& nogood, size_t domain_size, std::vector<Clause>& result)
	-> void {
	const auto n_nogoods = nogood.relation.size();
	const auto arity = nogood.relation.arity();
	for (auto j = 0u; j < n_nogoods; ++j) {
		auto clause = Clause{};
		clause.reserve(arity);
		for (auto k = 0u; k < arity; ++k) {
			const auto xk_eq_nogoodjk = nogood.variables[k] * domain_size + nogood.relation[j][k];
			clause.push_back(Literal(xk_eq_nogoodjk, NEGATED));
		}
		result.push_back(std::move(clause));
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
		__internal::at_least_one_clauses(constraint, domain_size, clauses);
		__internal::at_most_one_clauses(constraint, domain_size, clauses);
	});
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		__internal::conflict_clauses(nogood, domain_size, clauses);
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
		__internal::at_least_one_clauses(constraint, domain_size, clauses);
	});
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		__internal::conflict_clauses(nogood, domain_size, clauses);
	});

	return SAT(std::move(clauses));
}
} // namespace cspc