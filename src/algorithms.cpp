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
auto increment_in_domain(relation_entry& tuple, size_t domain_size) -> void {
	for (auto& val : tuple) {
		if (++val < domain_size) {
			break;
		}
		val = 0;
	}
}

auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<relation_entry> {
	const auto n_tuples = std::pow(domain_size, arity);
	auto result = std::vector<relation_entry>{};
	result.reserve(n_tuples);
	auto current = relation_entry(arity);
	gautil::repeat(n_tuples, [&]() {
		result.push_back(current);
		increment_in_domain(current, domain_size);
	});
	return result;
}

auto inverse(constraint const& _constraint, size_t domain_size) -> constraint {
	auto sorted_all_tuples = create_all_tuples(_constraint.get_relation().arity(), domain_size);
	std::ranges::sort(sorted_all_tuples);

	auto sorted_constraint_relation = _constraint.get_relation().data();
	std::ranges::sort(sorted_constraint_relation);

	auto inverse_relation = std::vector<relation_entry>{};
	inverse_relation.reserve(sorted_all_tuples.size() - sorted_constraint_relation.size());
	std::ranges::set_difference(
		sorted_all_tuples, sorted_constraint_relation, std::back_inserter(inverse_relation));
	return constraint(std::move(inverse_relation), _constraint.variables());
}

auto nogoods(std::vector<constraint> const& constraints, size_t domain_size)
	-> std::vector<constraint> {
	auto nogoods = std::vector<constraint>{};
	nogoods.reserve(constraints.size());
	std::ranges::transform(
		constraints, std::back_inserter(nogoods),
		[&](constraint const& constraint) { return __internal::inverse(constraint, domain_size); });
	return nogoods;
}

auto index_to_function_input(domain_value index, size_t arity, size_t domain_size)
	-> relation_entry {
	auto result = relation_entry(arity);
	for (auto i = 0u; i < arity; ++i) {
		const auto remainder = index % domain_size;
		result[arity - 1 - i] = remainder;
		index = (index - remainder) / domain_size;
	}
	return result;
}

auto function_input_to_index(relation_entry const& input, size_t domain_size) -> domain_value {
	auto result = size_t{0};
	auto multiplier = size_t{1};
	for (auto i = 0u; i < input.length(); ++i) {
		result += input[input.length() - 1 - i] * multiplier;
		multiplier *= domain_size;
	}
	return result;
}

auto satisfies_identity(relation_entry const& input, std::vector<domain_value> const& id) -> bool {
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
	relation_entry const& input,
	std::vector<domain_value> const& from,
	std::vector<domain_value> const& to) -> relation_entry {
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

auto all_nary_relations(size_t n, size_t domain_size) -> std::vector<relation> {
	const auto n_tuples = std::pow(domain_size, n);

	// all tetriary tuples of elements in the domain
	const auto all_tuples = __internal::create_all_tuples(n, domain_size);

	// tuples (a, b, c, ...) where a != b or a != c or ... or b != c or ...
	// <=> U  \ (a, b, c, ...) where (a == b == c == ...)
	auto tuples = std::vector<relation_entry>{};
	tuples.reserve(n_tuples - domain_size);
	std::ranges::copy_if(all_tuples, std::back_inserter(tuples), [](relation_entry const& tuple) {
		return std::ranges::adjacent_find(tuple, std::not_equal_to{}) != tuple.end();
	});

	//  all subsets of tuples, bar the empty set
	auto subsets = gautil::all_subsets(tuples);
	subsets.erase(subsets.begin());

	// to relations
	auto result = std::vector<relation>{};
	result.reserve(subsets.size());
	std::ranges::transform(
		subsets, std::back_inserter(result), [&](auto const& v) { return relation(v); });

	return result;
}

auto neq_relation(size_t arity, size_t domain_size) -> relation {
	return create_relation<relation_entry>(arity, domain_size, [](relation_entry const& range) {
		return std::ranges::adjacent_find(range, std::not_equal_to()) != range.end();
	});
}

auto eq_relation(size_t arity, size_t domain_size) -> relation {
	return create_relation<relation_entry>(arity, domain_size, [](relation_entry const& range) {
		return std::ranges::adjacent_find(range, std::equal_to()) != range.end();
	});
}

auto siggers_operation() -> operation { return operation(4, {{{0, 1, 0, 2}, {1, 0, 2, 1}}}); }

namespace __internal {
auto operation_identity_constraints(
	operation const& operation, size_t domain_size, std::vector<constraint>& result) -> void {
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
					const auto relation = eq_relation(2, domain_size);
					result.push_back(constraint{relation, {k, k_mirror}});
				}
			}
		}
	});
}

auto polymorphism_constraints(
	constraint const& _constraint,
	size_t domain_size,
	size_t operation_arity,
	std::vector<constraint>& result) -> void {
	const auto n_indices = _constraint.get_relation().size();
	const auto n_iterations = (size_t)std::pow(n_indices, operation_arity);
	// constraints from operation being polymorphism of all relations

	// for each choice of rows in relation
	auto current_relation_indices = relation_entry(operation_arity);
	gautil::repeat(n_iterations, [&]() {
		// extract each column by index in the set of rows and get its index in the function
		// table
		auto indices = std::vector<variable>(_constraint.get_relation().arity());
		for (auto j = 0u; j < _constraint.get_relation().arity(); ++j) {
			auto input = relation_entry(operation_arity);
			for (auto k = 0u; k < operation_arity; ++k) {
				input[k] = _constraint.get_relation()[current_relation_indices[k]][j];
			}
			indices[j] = __internal::function_input_to_index(input, domain_size);
		}
		// the ordered values at the set of indices must be in the relation
		result.push_back(constraint(_constraint.get_relation(), indices));

		__internal::increment_in_domain(current_relation_indices, n_indices);
	});
}
} // namespace __internal

auto has_polymorphism_csp(csp const& input_csp, operation const& operation) -> csp {
	auto constraints = std::vector<constraint>{};

	__internal::operation_identity_constraints(operation, input_csp.domain_size(), constraints);

	std::ranges::for_each(input_csp.constraints(), [&](constraint const& constraint) {
		__internal::polymorphism_constraints(
			constraint, input_csp.domain_size(), operation.arity, constraints);
	});

	return csp(std::move(constraints));
}

auto to_preserves_operation_csp(operation const& operation, relation const& relation) -> csp {
	auto variables = std::vector<variable>(relation.arity());
	std::iota(variables.begin(), variables.end(), 0);
	const auto input = csp({constraint(relation, variables)});
	return has_polymorphism_csp(input, operation);
}

} // namespace cspc
