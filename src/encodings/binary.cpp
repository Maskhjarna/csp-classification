#include "cspc/encodings/binary.hpp"

#include "cspc/algorithms.hpp"
#include "cspc/encodings/common.hpp"
#include <cmath>
#include <gautil/functional.hpp>
#include <numeric>
#include <ranges>

namespace cspc {

namespace __internal {
template <std::output_iterator<clause> OutputIterator>
auto create_nogood_clauses(
	std::vector<constraint> const nogoods, size_t n_bits, OutputIterator result) -> OutputIterator {
	std::ranges::for_each(nogoods, [&](constraint const& nogood) {
		const auto arity = nogood.get_relation().arity();
		result = std::ranges::transform(nogood.get_relation(), result, [&](auto const& entry) {
					 auto _clause = clause{};
					 _clause.reserve(n_bits * arity);
					 for (auto j = 0u; j < arity; ++j) {
						 auto v = entry[j];
						 for (auto k = 0u; k < n_bits; ++k) {
							 const auto bit = v & 1;
							 _clause.push_back(literal(
								 nogood.variables()[j] * n_bits + k, bit ? NEGATED : REGULAR));
							 v >>= 1;
						 }
					 }
					 return _clause;
				 }).out;
	});
	return result;
}

auto create_prohibited_value_clause(variable v, domain_value d, size_t n_bits) -> clause {
	auto _clause = clause{};
	_clause.reserve(n_bits);
	for (auto k = 0u; k < n_bits; ++k) {
		auto bit = d & 1;
		_clause.push_back(literal(v * n_bits + k, bit ? NEGATED : REGULAR));
		d >>= 1;
	}
	return _clause;
}

template <std::output_iterator<clause> OutputIterator>
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

auto binary_encoding(csp const& csp) -> sat {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto inclusive_domain_size = std::pow(2, n_bits);
	const auto nogoods = __internal::nogoods(csp.constraints(), inclusive_domain_size);

	const auto n_clauses = gautil::fold(nogoods, 0ul, std::plus{}, &constraint::relation_size);
	auto clauses = std::vector<clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return sat(std::move(clauses));
}

auto log_encoding(csp const& csp) -> sat {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());
	const auto inclusive_domain_size = std::pow(2, n_bits);

	const auto n_clauses = gautil::fold(nogoods, 0ul, std::plus{}, &constraint::relation_size) +
						   n_bits * csp.n_variables() * (inclusive_domain_size - csp.domain_size());
	auto clauses = std::vector<clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));
	__internal::create_prohibited_value_clauses(
		csp.n_variables(), csp.domain_size(), inclusive_domain_size, n_bits,
		std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return sat(std::move(clauses));
}

} // namespace cspc