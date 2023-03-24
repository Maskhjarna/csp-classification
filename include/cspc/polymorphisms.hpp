#pragma once

#include "data_structures.hpp"
#include "types.hpp"
#include <numeric>
#include <optional>

namespace poly {

namespace factory {
extern auto relation(
	size_t arity,
	size_t domain_size,
	std::function<bool(std::vector<size_t>::const_iterator, std::vector<size_t>::const_iterator)>
		fn) -> Relation;
extern auto neq_constraint(std::vector<size_t> const& variables, size_t domain_size) -> Constraint;
extern auto eq_constraint(std::vector<size_t> const& variables, size_t domain_size) -> Constraint;
extern auto unary_relation(RuntimeArray<size_t> const& row) -> Relation;
extern auto all_binary_relations(size_t domain_size) -> std::optional<std::vector<Relation>>;
extern auto all_tetriary_relations(size_t domain_size) -> std::optional<std::vector<Relation>>;
extern auto majority_operation(size_t arity, size_t domain_size) -> Operation;
extern auto siggers_operation(size_t domain_size) -> Operation;
extern auto has_polymorphism_csp(CSP const& input_csp, Operation const& operation) -> CSP;
} // namespace factory
extern auto satisfies_identity(RuntimeArray<size_t> const& input, std::vector<size_t> id) -> bool;
auto apply_identity(
	RuntimeArray<size_t> const& input,
	std::vector<size_t> const& lhs,
	std::vector<size_t> const& rhs) -> RuntimeArray<size_t>;
extern auto index_to_function_input(size_t index, size_t arity, size_t domain_size)
	-> RuntimeArray<size_t>;
extern auto function_input_to_index(RuntimeArray<size_t> const& input, size_t domain_size)
	-> size_t;
extern auto support_encoding(CSP const& csp) -> SAT;
extern auto multivalued_direct_encoding(CSP const& csp) -> SAT;
extern auto
to_preserves_operation_csp(size_t domain_size, Operation const& operation, Relation const& relation)
	-> CSP;
} // namespace poly