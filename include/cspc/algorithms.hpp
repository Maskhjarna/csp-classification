#pragma once

#include "data_structures.hpp"
#include <cmath>
#include <functional>

#include <spdlog/spdlog.h>

namespace cspc {

// these are exposed for the sake of unit tests
namespace __internal {
extern auto index_to_function_input(domain_value index, size_t arity, size_t domain_size)
	-> relation_entry;
extern auto function_input_to_index(relation_entry const& input, size_t domain_size)
	-> domain_value;
extern auto satisfies_identity(relation_entry const& input, std::vector<domain_value> const& id)
	-> bool;
extern auto satisfies_identity(relation_entry const& input, std::vector<domain_value> const& id)
	-> bool;
extern auto apply_identity(
	relation_entry const& input,
	std::vector<domain_value> const& from,
	std::vector<domain_value> const& to) -> relation_entry;
} // namespace __internal

extern auto neq_relation(size_t arity, size_t domain_size) -> relation;
extern auto eq_relation(size_t arity, size_t domain_size) -> relation;
extern auto all_nary_relations(size_t n, size_t domain_size) -> std::vector<relation>;
extern auto inverse(constraint const& _constraint, size_t domain_size) -> constraint;
extern auto siggers_operation() -> operation;
extern auto
construct_preserves_operation_csp(operation const& _operation, relation const& _relation) -> csp;
extern auto inverse(constraint const& _constraint, size_t domain_size) -> constraint;
extern auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<relation_entry>;

template <std::ranges::input_range Range>
auto create_relation(size_t arity, size_t domain_size, std::function<bool(Range)> fn) -> relation {
	auto entries = std::vector<relation_entry>{};
	entries.reserve(std::pow(domain_size, arity));
	std::ranges::copy_if(create_all_tuples(arity, domain_size), std::back_inserter(entries), fn);
	return relation(entries);
}
} // namespace cspc