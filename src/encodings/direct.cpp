#include "cspc/encodings/direct.hpp"

#include "cspc/algorithms.hpp"
#include "cspc/encodings/common.hpp"
#include <gautil/functional.hpp>
#include <gautil/math.hpp>
#include <numeric>

namespace cspc {

namespace __internal {
template <std::output_iterator<clause> outputiterator>
auto conflict_clauses(
	std::vector<constraint> const& nogoods, size_t domain_size, outputiterator result)
	-> outputiterator {
	std::ranges::for_each(nogoods, [&](constraint const& nogood) {
		const auto arity = nogood.get_relation().arity();
		result = std::ranges::transform(nogood.get_relation(), result, [&](auto const& entry) {
					 auto _clause = clause{};
					 _clause.reserve(arity);
					 for (auto k = 0u; k < arity; ++k) {
						 const auto xk_eq_nogoodjk = nogood.variables()[k] * domain_size + entry[k];
						 _clause.push_back(literal(xk_eq_nogoodjk, NEGATED));
					 }
					 return _clause;
				 }).out;
	});
	return result;
}
} // namespace __internal

auto direct_encoding(csp const& csp) -> sat {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses =
		csp.n_variables() * gautil::n_choose_k(domain_size, 2) +			   // at most one
		gautil::fold(csp.constraints(), 0u, std::plus{}, &constraint::arity) + // at least one
		gautil::fold(nogoods, 0u, std::plus{}, &constraint::relation_size);	   // conflict

	auto clauses = std::vector<clause>{};
	clauses.reserve(n_clauses);

	__internal::at_most_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::conflict_clauses(nogoods, domain_size, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return sat(std::move(clauses));
}

auto multivalued_direct_encoding(csp const& csp) -> sat {
	const auto domain_size = csp.domain_size();
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());

	auto n_clauses =
		gautil::fold(csp.constraints(), 0u, std::plus{}, &constraint::arity) + // at least one
		gautil::fold(nogoods, 0u, std::plus{}, &constraint::relation_size);	   // conflict

	auto clauses = std::vector<clause>{};
	clauses.reserve(n_clauses);

	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::conflict_clauses(nogoods, domain_size, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return sat(std::move(clauses));
}
} // namespace cspc