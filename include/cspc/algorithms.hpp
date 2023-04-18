#pragma once

#include "data_structures.hpp"
#include <cmath>
#include <functional>

namespace cspc {

namespace __internal {
extern auto increment_in_domain(Relation::Entry& tuple, size_t domain_size) -> void;
extern auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<Relation::Entry>;
extern auto inverse(Constraint const& constraint, size_t domain_size) -> Constraint;
auto nogoods(std::vector<Constraint> const& constraints, size_t domain_size)
	-> std::vector<Constraint>;
extern auto index_to_function_input(DomainValue index, size_t arity, size_t domain_size)
	-> Relation::Entry;
extern auto function_input_to_index(Relation::Entry const& input, size_t domain_size)
	-> DomainValue;
extern auto satisfies_identity(Relation::Entry const& input, std::vector<DomainValue> const& id)
	-> bool;
extern auto apply_identity(
	Relation::Entry const& input,
	std::vector<DomainValue> const& lhs,
	std::vector<DomainValue> const& rhs) -> Relation::Entry;
} // namespace __internal

template <std::ranges::input_range Range>
auto relation(size_t arity, size_t domain_size, std::function<bool(Range)> fn) -> Relation {
	auto entries = std::vector<Relation::Entry>{};
	entries.reserve(std::pow(domain_size, arity));
	std::ranges::copy_if(
		__internal::create_all_tuples(arity, domain_size), std::back_inserter(entries), fn);
	return Relation(entries);
}
extern auto neq_constraint(std::vector<Variable> const& variables, size_t domain_size)
	-> Constraint;
extern auto eq_constraint(std::vector<Variable> const& variables, size_t domain_size) -> Constraint;
extern auto unary_relation(std::vector<Variable> const& variables, Relation::Entry const& row)
	-> Constraint;
extern auto all_nary_relations(size_t n, size_t domain_size) -> std::vector<Relation>;
extern auto siggers_operation() -> Operation;
extern auto has_polymorphism_csp(CSP const& input_csp, Operation const& operation) -> CSP;
extern auto label_cover_encoding(CSP const& csp) -> SAT;
extern auto to_preserves_operation_csp(Operation const& operation, Relation const& relation) -> CSP;
} // namespace cspc