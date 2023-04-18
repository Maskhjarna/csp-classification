#include "cspc/encodings/binary.hpp"

#include "cspc/algorithms.hpp"
#include <cmath>
#include <gautil/functional.hpp>
#include <numeric>
#include <ranges>

namespace cspc {
namespace __internal {

auto create_nogood_clauses(
	std::vector<Constraint> const& nogoods,
	size_t n_bits,
	std::back_insert_iterator<std::vector<Clause>> result) -> void {
	std::ranges::for_each(nogoods, [&](Constraint const& nogood) {
		const auto arity = nogood.relation.arity();
		std::ranges::transform(nogood.relation, result, [&](Relation::Entry const& entry) {
			auto clause = Clause{};
			clause.reserve(n_bits * arity);
			for (auto j = 0u; j < entry.size(); ++j) {
				auto v = entry[j];
				for (auto k = 0u; k < n_bits; ++k) {
					const auto bit = v & 1;
					clause.push_back(
						Literal(nogood.variables[j] * n_bits + k, bit ? NEGATED : REGULAR));
					v >>= 1;
				}
			}
			return clause;
		});
	});
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

auto create_prohibited_value_clauses(
	size_t n_variables,
	size_t domain_size,
	size_t inclusive_domain_size,
	size_t n_bits,
	auto result) -> void {
	for (auto v = 0u; v < n_variables; ++v) {
		for (auto d = domain_size; d < inclusive_domain_size; ++d) {
			*result++ = __internal::create_prohibited_value_clause(v, d, n_bits);
		}
	}
}
} // namespace __internal

auto log_encoding(CSP const& csp) -> SAT {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto nogoods = __internal::nogoods(csp.constraints(), csp.domain_size());
	const auto inclusive_domain_size = std::pow(2, n_bits);
	const auto n_clauses = gautil::fold(nogoods, 0ul, std::plus{}, &Constraint::relation_size) +
						   n_bits * csp.n_variables() * (inclusive_domain_size - csp.domain_size());

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));
	__internal::create_prohibited_value_clauses(
		csp.n_variables(), csp.domain_size(), inclusive_domain_size, n_bits,
		std::back_inserter(clauses));

	return SAT(std::move(clauses));
}

auto binary_encoding(CSP const& csp) -> SAT {
	const auto n_bits =
		u64(std::numeric_limits<size_t>::digits - std::countl_zero(csp.domain_size()));
	const auto inclusive_domain_size = std::pow(2, n_bits);
	const auto nogoods = __internal::nogoods(csp.constraints(), inclusive_domain_size);

	const auto n_clauses = std::transform_reduce(
		nogoods.begin(), nogoods.end(), 0, std::plus{},
		[](Constraint const& constraint) { return constraint.relation.size(); });
	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::create_nogood_clauses(nogoods, n_bits, std::back_inserter(clauses));

	return SAT(std::move(clauses));
}
} // namespace cspc