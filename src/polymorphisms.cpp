#include "polymorphisms.hpp"

#include "data_structures.hpp"
#include "util.hpp"
#include <fmt/core.h>
#include <numeric>
#include <ranges>
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace poly {

auto increment_in_domain(RuntimeArray<size_t>& tuple, size_t domain_size) -> void {
	for (auto& val : tuple) {
		if (++val < domain_size) {
			break;
		}
		val = 0;
	}
}

auto create_all_tuples(size_t arity, size_t domain_size) -> std::vector<RuntimeArray<size_t>> {
	const auto n_tuples = std::pow(domain_size, arity);
	auto result = std::vector<RuntimeArray<size_t>>{};
	result.reserve(n_tuples);
	auto current = RuntimeArray<size_t>(arity);
	util::repeat(n_tuples, [&]() {
		result.push_back(current);
		increment_in_domain(current, domain_size);
	});
	return result;
}

auto inverse(Relation const& relation, size_t domain_size) -> Relation {
	auto result = Relation(create_all_tuples(relation.arity(), domain_size));
	std::ranges::for_each(
		relation, [&](RuntimeArray<size_t> const& entry) { result.erase(entry); });
	return result;
}

auto index_to_function_input(size_t index, size_t arity, size_t domain_size)
	-> RuntimeArray<size_t> {
	auto result = RuntimeArray<size_t>(arity);
	for (auto i = 0u; i < arity; ++i) {
		const auto remainder = index % domain_size;
		result[arity - 1 - i] = remainder;
		index = (index - remainder) / domain_size;
	}
	return result;
}

auto function_input_to_index(RuntimeArray<size_t> const& input, size_t domain_size) -> size_t {
	auto result = size_t{0};
	auto multiplier = size_t{1};
	for (auto i = 0u; i < input.length(); ++i) {
		result += input[input.length() - 1 - i] * multiplier;
		multiplier *= domain_size;
	}
	return result;
}

auto apply_identity(
	RuntimeArray<size_t> const& input,
	std::vector<size_t> const& from,
	std::vector<size_t> const& to) -> RuntimeArray<size_t> {
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

auto satisfies_identity(RuntimeArray<size_t> const& input, std::vector<size_t> id) -> bool {
	for (auto i = 0u; i < input.length(); ++i) {
		for (auto j = i + 1; j < input.length(); ++j) {
			if ((id[i] == id[j]) && (input[i] != input[j])) {
				return false;
			}
		}
	}
	return true;
}

// } // namespace

namespace factory {

template <std::ranges::input_range Range>
auto relation(size_t arity, size_t domain_size, std::function<bool(Range)> fn) -> Relation {
	auto entries = std::vector<RuntimeArray<size_t>>{};
	auto entry = RuntimeArray<size_t>(arity);
	util::repeat(std::pow(domain_size, arity), [&]() {
		if (fn(entry)) {
			entries.push_back(entry); // TODO create domain iterator and use here
		}
		increment_in_domain(entry, domain_size);
	});
	return Relation(entries);
}

auto assignment_constraint(
	std::vector<variable> const& variables, RuntimeArray<size_t> const& constants) -> Constraint {
	return Constraint(Relation({constants}), variables, IS);
}

auto all_nary_relations(size_t n, size_t domain_size) -> std::optional<std::vector<Relation>> {
	if (domain_size > 64) {
		spdlog::error("Infeasible domain size (> 64)");
		return std::nullopt;
	}
	const auto n_tuples = std::pow(domain_size, n);

	// all tetriary tuples of elements in the domain
	const auto all_tuples = create_all_tuples(n, domain_size);

	// tuples (a, b, c, ...) where a != b or a != c or ... or b != c or ...
	auto tuples = std::vector<RuntimeArray<size_t>>{};
	tuples.reserve(n_tuples - domain_size);
	std::ranges::copy_if(
		all_tuples, std::back_inserter(tuples), [](RuntimeArray<size_t> const& tuple) {
			return std::ranges::adjacent_find(tuple, std::not_equal_to{}) != tuple.end();
		});

	auto result = std::vector<Relation>{};
	result.reserve(std::pow(2, n_tuples));
	util::all_subsets(tuples, result.begin());

	std::ranges::sort(result, {}, &Relation::size);
	return result;
}

auto neq_constraint(std::vector<size_t> const& variables, size_t domain_size) -> Constraint {
	return Constraint(
		relation(
			variables.size(), domain_size,
			[](auto begin, auto end) {
				return std::adjacent_find(begin, end, std::not_equal_to()) != end;
			}),
		variables, NE);
}

auto eq_constraint(std::vector<size_t> const& variables, size_t domain_size) -> Constraint {
	return Constraint(
		relation(
			variables.size(), domain_size,
			[](auto begin, auto end) {
				return std::adjacent_find(begin, end, std::equal_to()) != end;
			}),
		variables, EQ);
}

auto siggers_operation() -> Operation { return Operation(4, {{{0, 1, 0, 2}, {1, 0, 2, 1}}}); }

auto has_polymorphism_csp(CSP const& input_csp, Operation const& operation) -> CSP {
	auto constraints = std::vector<Constraint>{};
	const auto domain_size = input_csp.domain_size();
	const auto function_table_entries = std::pow(domain_size, operation.arity);

	// constraints produced by operation identites
	for (auto const& identity : operation.identities) {
		for (auto i = 0u; i < identity.elements.size(); ++i) {
			for (auto j = i + 1; j < identity.elements.size(); ++j) {
				for (auto k = 0u; k < function_table_entries; ++k) {
					const auto input = index_to_function_input(k, operation.arity, domain_size);
					if (!satisfies_identity(input, identity.elements[i])) {
						continue;
					}
					const auto mirror =
						apply_identity(input, identity.elements[i], identity.elements[j]);
					const auto k_mirror = function_input_to_index(mirror, domain_size);
					constraints.push_back(eq_constraint({k, k_mirror}, domain_size));
				}
			}
		}
	}

	// constraints from operation being polymorphism of all relations
	for (auto constraint : input_csp.constraints()) {
		const auto n_indices = constraint.relation.size();
		const auto n_iterations = (size_t)std::pow(n_indices, operation.arity);
		auto current_relation_indices = RuntimeArray<size_t>(operation.arity);

		// for each choice of rows in relation
		for (auto i = 0u; i < n_iterations; ++i) {

			// extract each column by index in the set of rows and get its index in the function
			// table
			auto indices = std::vector<size_t>(constraint.relation.arity());
			for (auto j = 0u; j < constraint.relation.arity(); ++j) {
				auto input = RuntimeArray<size_t>(operation.arity);
				for (auto k = 0u; k < operation.arity; ++k) {
					input[k] = constraint.relation[current_relation_indices[k]][j];
				}
				indices[j] = function_input_to_index(input, domain_size);
			}
			// the ordered values at the set of indices must be in the relation
			constraints.push_back(Constraint(constraint.relation, indices));

			increment_in_domain(current_relation_indices, n_indices);
		}
	}

	return CSP(std::move(constraints));
}

} // namespace factory

auto support_encoding(CSP const& csp) -> SAT {
	const auto n_constraints = csp.constraints().size();
	const auto n_csp_vars = csp.n_variables() * csp.domain_size();

	auto n_clauses = csp.n_variables() * util::n_choose_k(csp.domain_size(), 2) +
					 csp.n_variables() + n_constraints;
	for (auto i = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		const auto arity = csp.constraints()[i].relation.arity();
		n_clauses += util::n_choose_k(n_entries, 2) + n_entries * arity;
	}

	auto clauses = std::vector<Clause>(n_clauses);
	auto clause_offset = size_t{0};

	// At most one assignment to each CSP variable
	for (auto i = 0u, variable_offset = 0u; i < csp.n_variables(); ++i) {
		for (auto j = 0u; j < csp.domain_size(); ++j) {
			for (auto k = j + 1; k < csp.domain_size(); ++k) {
				clauses[clause_offset] = {
					Literal{variable_offset + j, NEGATED},
					Literal{variable_offset + k, NEGATED},
				};
				clause_offset += 1;
			}
		}
		variable_offset += csp.domain_size();
	}

	// At least one assignment to each CSP variable
	for (auto i = 0u, variable_offset = 0u; i < csp.n_variables(); ++i) {
		clauses[clause_offset].resize(csp.domain_size());
		for (auto j = 0u; j < csp.domain_size(); ++j) {
			clauses[clause_offset][j] = Literal{variable_offset + j, NON_NEGATED};
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
					Literal{entry_variable, NEGATED}, Literal{other_entry_variable, NEGATED}};
				clause_offset += 1;
			}
		}
		variable_offset += n_entries;
	}
	// At least one row in constraint relation true
	for (auto i = 0u, variable_offset = 0u; i < n_constraints; ++i) {
		const auto n_entries = csp.constraints()[i].relation.size();
		clauses[clause_offset].resize(n_entries);
		for (auto j = 0u; j < n_entries; ++j) {
			const auto relation_entry_true = n_csp_vars + variable_offset + j;
			clauses[clause_offset][j] = Literal{relation_entry_true, NON_NEGATED};
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
					Literal{relation_entry_true, NEGATED},
					Literal{xk_eq_reljk, NON_NEGATED},
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
		csp.constraints(), std::back_inserter(nogoods),
		[domain_size = csp.domain_size()](Constraint const& constraint) {
			return inverse(constraint.relation, domain_size);
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
			clauses[offset].resize(arity);
			for (auto k = 0u; k < arity; ++k) {
				const auto xk_eq_nogoodjk =
					csp.constraints()[i].variables[k] * csp.domain_size() + nogoods[i][j][k];
				clauses[offset][k] = Literal{xk_eq_nogoodjk, NEGATED};
			}
			offset += 1;
		}
		for (auto j = 0u; j < arity; ++j) {
			clauses[offset].resize(n_goods);
			for (auto k = 0u; k < n_goods; ++k) {
				const auto xj_eq_relkj = csp.constraints()[i].variables[j] * csp.domain_size() +
										 csp.constraints()[i].relation[k][j];
				clauses[offset][k] = Literal{xj_eq_relkj, NON_NEGATED};
			}
			offset += 1;
		}
	}
	return SAT(std::move(clauses));
}

auto to_preserves_operation_csp(Operation const& operation, Relation const& relation) -> CSP {
	auto variables = std::vector<size_t>(relation.arity());
	std::iota(variables.begin(), variables.end(), 0);
	const auto input = CSP({Constraint(relation, variables)});
	return factory::has_polymorphism_csp(input, operation);
}
} // namespace poly
