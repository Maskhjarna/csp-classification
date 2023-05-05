#pragma once

#include "../data_structures.hpp"

namespace cspc {
namespace __internal {
template <std::output_iterator<Clause> OutputIterator>
auto at_least_one_clauses(CSP const& csp, OutputIterator result) -> OutputIterator {
	const auto domain_size = csp.domain_size();
	for (auto variable = Variable(0); variable < csp.n_variables(); ++variable) {
		auto clause = Clause{};
		clause.reserve(domain_size);
		for (auto k = 0u; k < domain_size; ++k) {
			const auto xj_eq_relkj = variable * domain_size + k;
			clause.push_back(Literal(xj_eq_relkj, REGULAR));
		}
		*result++ = clause;
	}
	return result;
}

template <std::output_iterator<Clause> OutputIterator>
auto at_most_one_clauses(CSP const& csp, OutputIterator result) -> OutputIterator {
	const auto domain_size = csp.domain_size();
	for (auto variable = Variable(0); variable < csp.n_variables(); ++variable) {
		for (auto k = 0u; k < domain_size; ++k) {
			for (auto l = k + 1; l < domain_size; ++l) {
				const auto xj_eq_k = variable * domain_size + k;
				const auto xj_eq_l = variable * domain_size + l;
				*result++ = {Literal(xj_eq_k, NEGATED), Literal(xj_eq_l, NEGATED)};
			}
		}
	};
	return result;
}
} // namespace __internal
} // namespace cspc