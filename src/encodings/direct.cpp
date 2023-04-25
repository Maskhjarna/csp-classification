#include "cspc/encodings/direct.hpp"

#include "cspc/algorithms.hpp"
#include "cspc/encodings/common.hpp"
#include <gautil/functional.hpp>
#include <gautil/math.hpp>
#include <numeric>

namespace cspc {

namespace __internal {
template <std::output_iterator<Clause> OutputIterator>
auto conflict_clauses(
	std::vector<Constraint> const& nogoods, size_t domain_size, OutputIterator result)
	-> OutputIterator {
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		const auto arity = nogood.relation.arity();
		result = std::ranges::transform(nogood.relation, result, [&](auto const& entry) {
					 auto clause = Clause{};
					 clause.reserve(arity);
					 for (auto k = 0u; k < arity; ++k) {
						 const auto xk_eq_nogoodjk = nogood.variables[k] * domain_size + entry[k];
						 clause.push_back(Literal(xk_eq_nogoodjk, NEGATED));
					 }
					 return clause;
				 }).out;
	});
	return result;
}
} // namespace __internal

auto direct_encoding(CSP const& csp) -> SAT {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses =
		csp.n_variables() * gautil::n_choose_k(domain_size, 2) +			   // at most one
		gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) + // at least one
		gautil::fold(nogoods, 0u, std::plus{}, &Constraint::relation_size);	   // conflict

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::at_most_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::conflict_clauses(nogoods, domain_size, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}

auto multivalued_direct_encoding(CSP const& csp) -> SAT {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses =
		gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) + // at least one
		gautil::fold(nogoods, 0u, std::plus{}, &Constraint::relation_size);	   // conflict

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::conflict_clauses(nogoods, domain_size, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}
} // namespace cspc