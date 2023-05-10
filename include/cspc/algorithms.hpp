#pragma once

#include "data_structures.hpp"
#include <cmath>
#include <functional>

namespace cspc {

namespace __internal {
extern auto increment_in_domain(relation_entry& tuple, size_t domain_size) -> void;
extern auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<relation_entry>;
extern auto inverse(constraint const& _constraint, size_t domain_size) -> constraint;
auto nogoods(std::vector<constraint> const& constraints, size_t domain_size)
	-> std::vector<constraint>;
extern auto index_to_function_input(domain_value index, size_t arity, size_t domain_size)
	-> relation_entry;
extern auto function_input_to_index(relation_entry const& input, size_t domain_size)
	-> domain_value;
extern auto satisfies_identity(relation_entry const& input, std::vector<domain_value> const& id)
	-> bool;
extern auto apply_identity(
	relation_entry const& input,
	std::vector<domain_value> const& lhs,
	std::vector<domain_value> const& rhs) -> relation_entry;
} // namespace __internal

template <std::ranges::input_range Range>
auto create_relation(size_t arity, size_t domain_size, std::function<bool(Range)> fn) -> relation {
	auto entries = std::vector<relation_entry>{};
	entries.reserve(std::pow(domain_size, arity));
	std::ranges::copy_if(
		__internal::create_all_tuples(arity, domain_size), std::back_inserter(entries), fn);
	return relation(entries);
}
extern auto neq_relation(size_t arity, size_t domain_size) -> relation;
extern auto eq_relation(size_t arity, size_t domain_size) -> relation;
extern auto all_nary_relations(size_t n, size_t domain_size) -> std::vector<relation>;
extern auto siggers_operation() -> operation;
extern auto has_polymorphism_csp(csp const& input_csp, operation const& operation) -> csp;
extern auto to_preserves_operation_csp(operation const& operation, relation const& relation) -> csp;
} // namespace cspc