#include "cspc/encodings/label_cover.hpp"

#include "cspc/encodings/common.hpp"
#include <gautil/functional.hpp>
#include <gautil/math.hpp>

namespace cspc {
namespace __internal {
auto at_most_one_entry_clauses(
	Constraint const& constraint, size_t variable_offset, std::vector<Clause>& result) -> void {
	const auto n_entries = constraint.relation.size();
	for (auto j = 0u; j < n_entries; ++j) {
		const auto entry_variable = variable_offset + j;
		for (auto k = j + 1; k < n_entries; ++k) {
			const auto other_entry_variable = variable_offset + k;
			result.push_back({
				Literal(entry_variable, NEGATED),
				Literal(other_entry_variable, NEGATED),
			});
		}
	}
};

auto at_least_one_entry_clauses(
	Constraint const& constraint, size_t variable_offset, std::vector<Clause>& result) -> void {
	const auto n_entries = constraint.relation.size();
	auto clause = Clause{};
	clause.reserve(n_entries);
	for (auto j = 0u; j < n_entries; ++j) {
		const auto relation_entry_true = variable_offset + j;
		clause.push_back(Literal(relation_entry_true, REGULAR));
	}
	result.push_back(clause);
}

auto implication_clauses(
	Constraint const& constraint,
	size_t domain_size,
	size_t variable_offset,
	std::vector<Clause>& result) {
	const auto n_entries = constraint.relation.size();
	const auto arity = constraint.relation.arity();
	// A variable representing an entry in a constraint relation implies an assignment to the
	// variables which the relation concerns.
	for (auto j = 0u; j < n_entries; ++j) {
		const auto relation_entry_true = variable_offset + j;
		for (auto k = 0u; k < arity; ++k) {
			const auto xk_eq_reljk =
				constraint.variables[k] * domain_size + constraint.relation[j][k];
			result.push_back({
				Literal(relation_entry_true, NEGATED),
				Literal(xk_eq_reljk, REGULAR),
			});
		}
	}
	variable_offset += n_entries;
	return variable_offset;
}
} // namespace __internal

auto label_cover_encoding(CSP const& csp) -> SAT {
	const auto n_clauses =
		csp.n_variables() * gautil::n_choose_k(csp.domain_size(), 2) + csp.n_variables() +
		csp.constraints().size() +
		gautil::fold(csp.constraints(), 0ul, std::plus{}, [&](auto const& constraint) {
			return gautil::n_choose_k(constraint.relation_size(), 2) +
				   constraint.relation_size() * constraint.arity();
		});

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	auto variable_offset = csp.n_variables() * csp.domain_size();
	std::ranges::for_each(csp.constraints(), [&](auto const& constraint) {
		__internal::at_most_one_clauses(constraint, csp.domain_size(), clauses);
		__internal::at_least_one_clauses(constraint, csp.domain_size(), clauses);
		__internal::at_most_one_entry_clauses(constraint, variable_offset, clauses);
		__internal::at_least_one_entry_clauses(constraint, variable_offset, clauses);
		__internal::implication_clauses(constraint, csp.domain_size(), variable_offset, clauses);
		variable_offset += constraint.relation_size();
	});

	return SAT(std::move(clauses));
}

auto multivalued_label_cover_encoding(CSP const& csp) -> SAT {
	const auto n_clauses =
		csp.n_variables() * gautil::n_choose_k(csp.domain_size(), 2) + csp.n_variables() +
		csp.constraints().size() +
		gautil::fold(csp.constraints(), 0ul, std::plus{}, [&](auto const& constraint) {
			return gautil::n_choose_k(constraint.relation_size(), 2) +
				   constraint.relation_size() * constraint.arity();
		});

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	auto variable_offset = csp.n_variables() * csp.domain_size();
	std::ranges::for_each(csp.constraints(), [&](auto const& constraint) {
		__internal::at_least_one_clauses(constraint, csp.domain_size(), clauses);
		__internal::at_most_one_entry_clauses(constraint, variable_offset, clauses);
		__internal::at_least_one_entry_clauses(constraint, variable_offset, clauses);
		__internal::implication_clauses(constraint, csp.domain_size(), variable_offset, clauses);
		variable_offset += constraint.relation_size();
	});

	return SAT(std::move(clauses));
}
} // namespace cspc