#include "cspc/encodings/label_cover.hpp"

#include "cspc/encodings/common.hpp"
#include <gautil/functional.hpp>
#include <gautil/math.hpp>

namespace cspc {
namespace __internal {
template <std::output_iterator<Clause> OutputIterator>
auto at_most_one_entry_clauses(CSP const& csp, OutputIterator result) -> OutputIterator {
	auto variable_offset = csp.n_variables() * csp.domain_size();
	std::ranges::for_each(csp.constraints(), [&](auto const& constraint) {
		const auto n_entries = constraint.relation.size();
		for (auto j = 0u; j < n_entries; ++j) {
			const auto entry_variable = variable_offset + j;
			for (auto k = j + 1; k < n_entries; ++k) {
				const auto other_entry_variable = variable_offset + k;
				*result++ = {
					Literal(entry_variable, NEGATED),
					Literal(other_entry_variable, NEGATED),
				};
			}
		}
		variable_offset += n_entries;
	});
	return result;
};

template <std::output_iterator<Clause> OutputIterator>
auto at_least_one_entry_clauses(CSP const& csp, OutputIterator result) -> OutputIterator {
	auto variable_offset = csp.n_variables() * csp.domain_size();
	return std::ranges::transform(
			   csp.constraints(), result,
			   [&](auto const& constraint) {
				   const auto n_entries = constraint.relation.size();
				   auto clause = Clause{};
				   clause.reserve(n_entries);
				   gautil::repeat(
					   n_entries, [&]() { clause.push_back(Literal(variable_offset++, REGULAR)); });
				   return clause;
			   })
		.out;
}

template <std::output_iterator<Clause> OutputIterator>
auto implication_clauses(CSP const& csp, OutputIterator result) -> OutputIterator {
	const auto domain_size = csp.domain_size();
	auto variable_offset = csp.n_variables() * csp.domain_size();
	std::ranges::for_each(csp.constraints(), [&](auto const& constraint) {
		// A variable representing an entry in a constraint relation implies an assignment to the
		// variables which the relation concerns.
		const auto arity = constraint.relation.arity();
		std::ranges::for_each(constraint.relation, [&](auto const& entry) {
			const auto relation_entry_true = variable_offset++;
			for (auto k = 0u; k < arity; ++k) {
				const auto xk_eq_reljk = constraint.variables[k] * domain_size + entry[k];
				*result++ = Clause{
					Literal(relation_entry_true, NEGATED),
					Literal(xk_eq_reljk, REGULAR),
				};
			}
		});
	});
	return result;
}
} // namespace __internal

auto label_cover_encoding(CSP const& csp) -> SAT {
	// clang-format off
	auto n_clauses =
		// at most one var
		csp.n_variables() * gautil::n_choose_k(csp.domain_size(), 2) +
		// at least one var
		gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) +
		// at most one entry in relation
		gautil::fold(csp.constraints(), 0ul, std::plus{}, [&](auto const& constraint) {
			return gautil::n_choose_k(constraint.relation_size(), 2);
		}) +
		// at least one entry in relation
		csp.constraints().size() +
		// implication
		gautil::fold(csp.constraints(), 0ul, std::plus{}, [&](auto const& constraint) {
			return constraint.relation_size() * constraint.arity();
		});
	// clang-format on

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::at_most_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_most_one_entry_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_entry_clauses(csp, std::back_inserter(clauses));
	__internal::implication_clauses(csp, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}

auto multivalued_label_cover_encoding(CSP const& csp) -> SAT {
	// clang-format off
	auto n_clauses =
		// at most one var
		csp.n_variables() * gautil::n_choose_k(csp.domain_size(), 2) +
		// at least one var
		gautil::fold(csp.constraints(), 0u, std::plus{}, &Constraint::arity) +
		// at least one entry in relation
		csp.constraints().size() +
		// implication
		gautil::fold(csp.constraints(), 0ul, std::plus{}, [&](auto const& constraint) {
			return constraint.relation_size() * constraint.arity();
		});
	// clang-format on

	auto clauses = std::vector<Clause>{};
	clauses.reserve(n_clauses);

	__internal::at_most_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_clauses(csp, std::back_inserter(clauses));
	__internal::at_least_one_entry_clauses(csp, std::back_inserter(clauses));
	__internal::implication_clauses(csp, std::back_inserter(clauses));

	assert(n_clauses == clauses.size());

	return SAT(std::move(clauses));
}
} // namespace cspc