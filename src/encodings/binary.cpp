#include "cspc/encodings/binary.hpp"

#include "cspc/algorithms.hpp"
#include <bits/iterator_concepts.h>
#include <cmath>
#include <gautil/functional.hpp>
#include <numeric>
#include <ranges>

namespace cspc {

namespace __internal {
template <std::output_iterator<Clause> OutputIterator>
auto create_nogood_clauses(
	std::vector<Constraint> const nogoods, size_t n_bits, OutputIterator result) -> OutputIterator {
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		const auto arity = nogood.relation.arity();
		result = std::ranges::transform(nogood.relation, result, [&](auto const& entry) {
					 auto clause = Clause{};
					 clause.reserve(n_bits * arity);
					 for (auto j = 0u; j < arity; ++j) {
						 auto v = entry[j];
						 for (auto k = 0u; k < n_bits; ++k) {
							 const auto bit = v & 1;
							 clause.push_back(Literal(
								 nogood.variables[j] * n_bits + k, bit ? NEGATED : REGULAR));
							 v >>= 1;
						 }
					 }
					 return clause;
				 }).out;
	});
	return result;
}

auto create_prohibited_value_clause(Variable v, DomainValue d, size_t n_bits) -> Clause {
	auto clause = Clause{};
	clause.reserve(n_bits);
	for (auto k = 0u; k < n_bits; ++k) {
		auto bit = d & 1;
		clause.push_back(Literal(v * n_bits + k, bit ? NEGATED : REGULAR));
		d >>= 1;
	}
	return clause;
}

template <std::output_iterator<Clause> OutputIterator>
auto create_prohibited_value_clauses(
	size_t n_variables,
	size_t domain_size,
	size_t inclusive_domain_size,
	size_t n_bits,
	OutputIterator result) -> OutputIterator {
	for (auto v = 0u; v < n_variables; ++v) {
		for (auto d = domain_size; d < inclusive_domain_size; ++d) {
			*result++ = __internal::create_prohibited_value_clause(v, d, n_bits);
		}
	}
	return result;
}
} // namespace __internal

auto log_encoding(CSP const& csp) -> SAT {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());
	const auto inclusive_domain_size = std::pow(2, n_bits);

	// NOTE: without reservation push_back is not thread safe in the comming calls
	const auto n_clauses = gautil::fold(nogoods, 0ul, std::plus{}, &Constraint::relation_size) +
						   n_bits * csp.n_variables() * (inclusive_domain_size - csp.domain_size());
	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));
	__internal::create_prohibited_value_clauses(
		csp.n_variables(), csp.domain_size(), inclusive_domain_size, n_bits,
		std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}

auto binary_encoding(CSP const& csp) -> SAT {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto inclusive_domain_size = std::pow(2, n_bits);
	const auto nogoods = __internal::nogoods(csp.constraints(), inclusive_domain_size);

	const auto n_clauses = gautil::fold(nogoods, 0ul, std::plus{}, &Constraint::relation_size);
	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}
} // namespace cspc