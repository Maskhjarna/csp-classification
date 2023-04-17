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
#include <unordered_map>

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

auto inverse(Relation const& relation, size_t domain_size) -> Relation {
	auto result = Relation(create_all_tuples(relation.arity(), domain_size));
	std::ranges::for_each(relation, [&](Relation::Entry const& entry) { result.erase(entry); });
	return result;
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

auto all_nary_relations(size_t n, size_t domain_size) -> std::optional<std::vector<Relation>> {
	if (domain_size > 64) {
		spdlog::error("Infeasible domain size (> 64)");
		return std::nullopt;
	}
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

	std::ranges::sort(result, {}, &Relation::size);
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

auto has_polymorphism_csp(CSP const& input_csp, Operation const& operation) -> CSP {
	auto constraints = std::vector<Constraint>{};
	const auto domain_size = input_csp.domain_size();
	const auto function_table_entries = std::pow(domain_size, operation.arity);

	// constraints derived from operation identites
	for (auto const& identity : operation.identities) {
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
					constraints.push_back(eq_constraint({k, k_mirror}, domain_size));
				}
			}
		}
	}

	// constraints from operation being polymorphism of all relations
	for (auto constraint : input_csp.constraints()) {
		const auto n_indices = constraint.relation.size();
		const auto n_iterations = (size_t)std::pow(n_indices, operation.arity);

		// for each choice of rows in relation
		auto current_relation_indices = Relation::Entry(operation.arity);
		gautil::repeat(n_iterations, [&]() {
			// extract each column by index in the set of rows and get its index in the function
			// table
			auto indices = std::vector<Variable>(constraint.relation.arity());
			for (auto j = 0u; j < constraint.relation.arity(); ++j) {
				auto input = Relation::Entry(operation.arity);
				for (auto k = 0u; k < operation.arity; ++k) {
					input[k] = constraint.relation[current_relation_indices[k]][j];
				}
				indices[j] = __internal::function_input_to_index(input, domain_size);
			}
			// the ordered values at the set of indices must be in the relation
			constraints.push_back(Constraint(constraint.relation, indices));

			__internal::increment_in_domain(current_relation_indices, n_indices);
		});
	}

	return CSP(std::move(constraints));
}

auto support_encoding(CSP const& csp) -> SAT {
	const auto n_constraints = csp.constraints().size();
	const auto n_csp_vars = csp.n_variables() * csp.domain_size();

	auto n_clauses = csp.n_variables() * gautil::n_choose_k(csp.domain_size(), 2) +
					 csp.n_variables() + n_constraints;
	for (auto i = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		const auto arity = csp.constraints()[i].relation.arity();
		n_clauses += gautil::n_choose_k(n_entries, 2) + n_entries * arity;
	}

	auto clauses = std::vector<Clause>(n_clauses);
	auto clause_offset = size_t{0};

	// At most one assignment to each CSP variable
	for (auto i = 0u, variable_offset = 0u; i < csp.n_variables(); ++i) {
		for (auto j = 0u; j < csp.domain_size(); ++j) {
			for (auto k = j + 1; k < csp.domain_size(); ++k) {
				clauses[clause_offset] = {
					Literal(variable_offset + j, NEGATED),
					Literal(variable_offset + k, NEGATED),
				};
				clause_offset += 1;
			}
		}
		variable_offset += csp.domain_size();
	}

	// At least one assignment to each CSP variable
	for (auto i = 0u, variable_offset = 0u; i < csp.n_variables(); ++i) {
		clauses[clause_offset].reserve(csp.domain_size());
		for (auto j = 0u; j < csp.domain_size(); ++j) {
			clauses[clause_offset].push_back(Literal(variable_offset, REGULAR));
		}
		variable_offset += csp.domain_size();
		clause_offset += 1;
	}

	// At most one row in constraint relation true
	for (auto i = 0u, variable_offset = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		for (auto j = 0u; j < n_entries; ++j) {
			const auto entry_variable = n_csp_vars + variable_offset + j;
			for (auto k = j + 1; k < n_entries; ++k) {
				const auto other_entry_variable = n_csp_vars + variable_offset + k;
				clauses[clause_offset] = {
					Literal(entry_variable, NEGATED),
					Literal(other_entry_variable, NEGATED),
				};
				clause_offset += 1;
			}
		}
		variable_offset += n_entries;
	}
	// At least one row in constraint relation true
	for (auto i = 0u, variable_offset = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		clauses[clause_offset].reserve(n_entries);
		for (auto j = 0u; j < n_entries; ++j) {
			const auto relation_entry_true = n_csp_vars + variable_offset + j;
			clauses[clause_offset].push_back(Literal(relation_entry_true, REGULAR));
		}
		variable_offset += n_entries;
		clause_offset += 1;
	}

	// A variable representing an entry in a constraint relation implies an assignment to the
	// variables which the relation concerns.
	for (auto i = 0u, variable_offset = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		const auto arity = csp.constraints()[i].relation.arity();
		for (auto j = 0u; j < n_entries; ++j) {
			const auto relation_entry_true = n_csp_vars + variable_offset + j;
			for (auto k = 0u; k < arity; ++k) {
				const auto xk_eq_reljk = csp.constraints()[i].variables[k] * csp.domain_size() +
										 csp.constraints()[i].relation[j][k];
				clauses[clause_offset] = {
					Literal(relation_entry_true, NEGATED),
					Literal(xk_eq_reljk, REGULAR),
				};
				clause_offset += 1;
			}
		}
		variable_offset += n_entries;
	}
	return SAT(std::move(clauses));
}

auto multivalued_direct_encoding(CSP const& csp) -> SAT {
	const auto n_constraints = csp.constraints().size();

	auto nogoods = std::vector<Relation>();
	nogoods.reserve(n_constraints);
	std::ranges::transform(
		csp.constraints(), std::back_inserter(nogoods), [&](Constraint const& constraint) {
			return __internal::inverse(constraint.relation, csp.domain_size());
		});

	auto n_clauses = 0u;
	for (auto i = 0u; i < n_constraints; ++i) {
		n_clauses += csp.constraints()[i].relation.arity() + nogoods[i].size();
	}

	auto clauses = std::vector<Clause>(n_clauses);
	for (auto i = 0u, offset = 0u; i < n_constraints; ++i) {
		const auto n_nogoods = nogoods[i].size();
		const auto n_goods = csp.constraints()[i].relation.size();
		const auto arity = csp.constraints()[i].relation.arity();
		for (auto j = 0u; j < n_nogoods; ++j) {
			clauses[offset].reserve(arity);
			for (auto k = 0u; k < arity; ++k) {
				const auto xk_eq_nogoodjk =
					csp.constraints()[i].variables[k] * csp.domain_size() + nogoods[i][j][k];
				clauses[offset].push_back(Literal(xk_eq_nogoodjk, NEGATED));
			}
			offset += 1;
		}
		for (auto j = 0u; j < arity; ++j) {
			clauses[offset].reserve(n_goods);
			for (auto k = 0u; k < n_goods; ++k) {
				const auto xj_eq_relkj = csp.constraints()[i].variables[j] * csp.domain_size() +
										 csp.constraints()[i].relation[k][j];
				clauses[offset].push_back(Literal(xj_eq_relkj, REGULAR));
			}
			offset += 1;
		}
	}
	return SAT(std::move(clauses));
}

auto to_preserves_operation_csp(Operation const& operation, Relation const& relation) -> CSP {
	auto variables = std::vector<Variable>(relation.arity());
	std::iota(variables.begin(), variables.end(), 0);
	const auto input = CSP({Constraint(relation, variables)});
	return has_polymorphism_csp(input, operation);
}
} // namespace cspc
