#pragma once

#include "../algorithms.hpp"
#include "../data_structures.hpp"

namespace cspc {
namespace __internal {
extern auto nogoods(std::vector<cspc::constraint> const& constraints, size_t domain_size)
	-> std::vector<cspc::constraint>;
template <std::output_iterator<clause> OutputIterator>
auto at_least_one_clauses(csp const& csp, OutputIterator result) -> OutputIterator {
	const auto domain_size = csp.domain_size();
	for (auto var = variable(0); var < csp.n_variables(); ++var) {
		auto _clause = clause{};
		_clause.reserve(domain_size);
		for (auto k = 0u; k < domain_size; ++k) {
			const auto xj_eq_relkj = var * domain_size + k;
			_clause.push_back(literal(xj_eq_relkj, REGULAR));
		}
		*result++ = _clause;
	}
	return result;
}

template <std::output_iterator<clause> OutputIterator>
auto at_most_one_clauses(csp const& csp, OutputIterator result) -> OutputIterator {
	const auto domain_size = csp.domain_size();
	for (auto var = variable(0); var < csp.n_variables(); ++var) {
		for (auto k = 0u; k < domain_size; ++k) {
			for (auto l = k + 1; l < domain_size; ++l) {
				const auto xj_eq_k = var * domain_size + k;
				const auto xj_eq_l = var * domain_size + l;
				*result++ = {literal(xj_eq_k, NEGATED), literal(xj_eq_l, NEGATED)};
			}
		}
	};
	return result;
}
} // namespace __internal

using encoding = std::function<sat(csp const&)>;
using polymorphism_checker = std::function<satisfiability(relation)>;

extern auto create_encoding_solver(operation const& op, encoding _encoding, solver _solver)
	-> polymorphism_checker;

} // namespace cspc