#include "cspc/algorithms.hpp"

#include "cspc/data_structures.hpp"
#include "cspc/formatters.hpp"
#include <fmt/core.h>
#include <gautil/formatters.hpp>
#include <gautil/functional.hpp>
#include <gautil/math.hpp>
#include <numeric>
#include <ranges>
#include <spdlog/spdlog.h>

namespace cspc {
namespace __internal {
auto increment_in_domain(Relation::Entry& tuple, size_t domain_size) -> void {
	for (auto& val : tuple) {
		if (++val < domain_size) {
			break;
		}
		val = 0;
	}
}

auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<Relation::Entry> {
	const auto n_tuples = std::pow(domain_size, arity);
	auto result = std::vector<Relation::Entry>{};
	result.reserve(n_tuples);
	auto current = Relation::Entry(arity);
	gautil::repeat(n_tuples, [&]() {
		result.push_back(current);
		increment_in_domain(current, domain_size);
	});
	return result;
}

auto inverse(Constraint const& constraint, size_t domain_size) -> Constraint {
	auto sorted_all_tuples = create_all_tuples(constraint.relation.arity(), domain_size);
	std::ranges::sort(sorted_all_tuples);

	auto sorted_constraint_relation = constraint.relation.data();
	std::ranges::sort(sorted_constraint_relation);

	auto inverse_relation = std::vector<Relation::Entry>{};
	inverse_relation.reserve(sorted_all_tuples.size() - sorted_constraint_relation.size());
	std::ranges::set_difference(
		sorted_all_tuples, sorted_constraint_relation, std::back_inserter(inverse_relation));
	return Constraint(std::move(inverse_relation), constraint.variables);
}

auto nogoods(std::vector<Constraint> const& constraints, size_t domain_size)
	-> std::vector<Constraint> {
	auto nogoods = std::vector<Constraint>{};
	nogoods.reserve(constraints.size());
	std::ranges::transform(
		constraints, std::back_inserter(nogoods),
		[&](Constraint const& constraint) { return __internal::inverse(constraint, domain_size); });
	return nogoods;
}

auto index_to_function_input(DomainValue index, size_t arity, size_t domain_size)
	-> Relation::Entry {
	auto result = Relation::Entry(arity);
	for (auto i = 0u; i < arity; ++i) {
		const auto remainder = index % domain_size;
		result[arity - 1 - i] = remainder;
		index = (index - remainder) / domain_size;
	}
	return result;
}

auto function_input_to_index(Relation::Entry const& input, size_t domain_size) -> DomainValue {
	auto result = size_t{0};
	auto multiplier = size_t{1};
	for (auto i = 0u; i < input.length(); ++i) {
		result += input[input.length() - 1 - i] * multiplier;
		multiplier *= domain_size;
	}
	return result;
}

auto satisfies_identity(Relation::Entry const& input, std::vector<DomainValue> const& id) -> bool {
	for (auto i = 0u; i < input.length(); ++i) {
		for (auto j = i + 1; j < input.length(); ++j) {
			if ((id[i] == id[j]) && (input[i] != input[j])) {
				return false;
			}
		}
	}
	return true;
}

auto apply_identity(
	Relation::Entry const& input,
	std::vector<DomainValue> const& from,
	std::vector<DomainValue> const& to) -> Relation::Entry {
	auto result = input;
	for (auto i = 0u; i < from.size(); ++i) {
		for (auto j = 0u; j < to.size(); ++j) {
			if (from[i] == to[j]) {
				result[j] = input[i];
			}
		}
	}
	return result;
}
} // namespace __internal

auto unary_relation(std::vector<Variable> const& variables, Relation::Entry const& row)
	-> Constraint {
	return Constraint(Relation({row}), variables, IS);
}

auto all_nary_relations(size_t n, size_t domain_size) -> std::vector<Relation> {
	const auto n_tuples = std::pow(domain_size, n);

	// all tetriary tuples of elements in the domain
	const auto all_tuples = __internal::create_all_tuples(n, domain_size);

	// tuples (a, b, c, ...) where a != b or a != c or ... or b != c or ...
	auto tuples = std::vector<Relation::Entry>{};
	tuples.reserve(n_tuples - domain_size);
	std::ranges::copy_if(all_tuples, std::back_inserter(tuples), [](Relation::Entry const& tuple) {
		return std::ranges::adjacent_find(tuple, std::not_equal_to{}) != tuple.end();
	});

	//  all subsets of tuples, bar the empty set
	auto subsets = gautil::all_subsets(tuples);
	subsets.erase(subsets.begin());

	// to relations
	auto result = std::vector<Relation>{};
	result.reserve(subsets.size());
	std::ranges::transform(
		subsets, std::back_inserter(result),
		[&](std::vector<Relation::Entry> const& v) { return Relation(v); });

	return result;
}

auto neq_constraint(std::vector<Variable> const& variables, size_t domain_size) -> Constraint {
	return Constraint(
		relation<Relation::Entry>(
			variables.size(), domain_size,
			[](Relation::Entry const& range) {
				return std::ranges::adjacent_find(range, std::not_equal_to()) != range.end();
			}),
		variables, NE);
}

auto eq_constraint(std::vector<Variable> const& variables, size_t domain_size) -> Constraint {
	return Constraint(
		relation<Relation::Entry>(
			variables.size(), domain_size,
			[](Relation::Entry const& range) {
				return std::ranges::adjacent_find(range, std::equal_to()) != range.end();
			}),
		variables, EQ);
}

auto siggers_operation() -> Operation { return Operation(4, {{{0, 1, 0, 2}, {1, 0, 2, 1}}}); }

namespace __internal {
auto operation_identity_constraints(
	Operation const& operation, size_t domain_size, std::vector<Constraint>& result) -> void {
	const auto function_table_entries = std::pow(domain_size, operation.arity);
	std::ranges::for_each(operation.identities, [&](auto const& identity) {
		for (auto i = 0u; i < identity.elements.size(); ++i) {
			for (auto j = i + 1; j < identity.elements.size(); ++j) {
				for (auto k = 0u; k < function_table_entries; ++k) {
					const auto input =
						__internal::index_to_function_input(k, operation.arity, domain_size);
					if (!__internal::satisfies_identity(input, identity.elements[i])) {
						continue;
					}
					const auto mirror = __internal::apply_identity(
						input, identity.elements[i], identity.elements[j]);
					const auto k_mirror = __internal::function_input_to_index(mirror, domain_size);
					result.push_back(eq_constraint({k, k_mirror}, domain_size));
				}
			}
		}
	});
}
auto polymorphism_constraints(
	Constraint const& constraint,
	size_t domain_size,
	size_t operation_arity,
	std::vector<Constraint>& result) -> void {
	const auto n_indices = constraint.relation.size();
	const auto n_iterations = (size_t)std::pow(n_indices, operation_arity);
	// constraints from operation being polymorphism of all relations

	// for each choice of rows in relation
	auto current_relation_indices = Relation::Entry(operation_arity);
	gautil::repeat(n_iterations, [&]() {
		// extract each column by index in the set of rows and get its index in the function
		// table
		auto indices = std::vector<Variable>(constraint.relation.arity());
		for (auto j = 0u; j < constraint.relation.arity(); ++j) {
			auto input = Relation::Entry(operation_arity);
			for (auto k = 0u; k < operation_arity; ++k) {
				input[k] = constraint.relation[current_relation_indices[k]][j];
			}
			indices[j] = __internal::function_input_to_index(input, domain_size);
		}
		// the ordered values at the set of indices must be in the relation
		result.push_back(Constraint(constraint.relation, indices));

		__internal::increment_in_domain(current_relation_indices, n_indices);
	});
}
} // namespace __internal

auto has_polymorphism_csp(CSP const& input_csp, Operation const& operation) -> CSP {
	auto constraints = std::vector<Constraint>{};

	__internal::operation_identity_constraints(operation, input_csp.domain_size(), constraints);

	std::ranges::for_each(input_csp.constraints(), [&](Constraint const& constraint) {
		__internal::polymorphism_constraints(
			constraint, input_csp.domain_size(), operation.arity, constraints);
	});

	return CSP(std::move(constraints));
}

auto to_preserves_operation_csp(Operation const& operation, Relation const& relation) -> CSP {
	auto variables = std::vector<Variable>(relation.arity());
	std::iota(variables.begin(), variables.end(), 0);
	const auto input = CSP({Constraint(relation, variables)});
	return has_polymorphism_csp(input, operation);
}
} // namespace cspc
