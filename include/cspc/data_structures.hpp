#pragma once

#include "types.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <ranges>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

enum Negation {
	NON_NEGATED = 0,
	NEGATED = 1,
};

struct Literal {
	size_t variable;
	Negation negation = NON_NEGATED;
};

using Clause = std::vector<Literal>;

enum Satisfiability {
	UNSATISFIABLE = 0,
	SATISFIABLE,
};

template <typename T> class RuntimeArray {
  public:
	RuntimeArray(std::initializer_list<T> data) : m_data{std::move(data)}, m_length{data.size()} {}
	RuntimeArray(size_t length) : m_data(length), m_length{length} {}

	auto length() const -> size_t { return m_length; }
	auto begin() { return m_data.begin(); }
	auto end() { return m_data.end(); }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	auto reserve(size_t n) { m_data.reserve(n); }
	auto size() const { return m_data.size(); }
	auto operator[](size_t i) -> T& { return m_data[i]; };
	auto operator[](size_t i) const -> T const& { return m_data[i]; };
	auto operator*() const -> T& { *this; };
	auto operator==(RuntimeArray const& other) const -> bool {
		return m_length == other.m_length && m_data == other.m_data;
	}

  private:
	std::vector<T> m_data;
	size_t m_length;
};

class Relation {
  public:
	Relation() : m_arity{0} {}
	Relation(std::initializer_list<RuntimeArray<size_t>> data)
		: m_data{std::move(data)},
		  m_arity{std::ranges::min(data, {}, &RuntimeArray<size_t>::size).size()} {}
	Relation(std::vector<RuntimeArray<size_t>> data)
		: m_data{std::move(data)},
		  m_arity{std::ranges::min(data, {}, &RuntimeArray<size_t>::size).size()} {}

	auto arity() const -> size_t { return m_arity; }
	auto begin() { return m_data.begin(); }
	auto end() { return m_data.end(); }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	auto insert(std::vector<RuntimeArray<size_t>>::const_iterator it, RuntimeArray<size_t> val) {
		m_data.insert(it, std::move(val));
	}
	auto reserve(size_t n) { m_data.reserve(n); }
	auto size() const { return m_data.size(); }
	auto erase(RuntimeArray<size_t> const& value) {
		// TODO: Relation would probably be better represented by an unordered_set, but there is no
		// stdlib hash function for vectors
		m_data.erase(std::ranges::find(m_data, value));
	}
	auto operator[](size_t i) -> RuntimeArray<size_t>& { return m_data[i]; };
	auto operator[](size_t i) const -> RuntimeArray<size_t> const& { return m_data[i]; };

  private:
	std::vector<RuntimeArray<size_t>> m_data;
	size_t m_arity;
};

struct Identity {
	Identity(std::initializer_list<std::vector<size_t>> elements) : elements{std::move(elements)} {}
	const std::vector<std::vector<size_t>> elements{};
};

struct Operation {
	Operation(size_t arity, std::initializer_list<Identity> identities)
		: arity{arity}, identities{std::move(identities)} {}
	const size_t arity;
	const std::vector<Identity> identities;
};

using variable = size_t;

enum ConstraintTag {
	EQ,
	NE,
	GT,
	GE,
	LT,
	LE,
	IS,
	OTHER,
};

class Constraint {
  public:
	// TODO: auto find constraint tag / construct from constraint tag
	Constraint(
		Relation const& relation, std::vector<variable> const& variables, ConstraintTag tag = OTHER)
		: relation{relation}, variables{variables}, tag{tag} {}

	const Relation relation;
	const std::vector<size_t> variables;
	const ConstraintTag tag;
};

class CSP {
  public:
	CSP(std::vector<Constraint> const& constraints)
		: _n_variables{[&constraints]() {
			  auto max_variable = size_t{0};
			  for (auto const& constraint : constraints) {
				  max_variable = std::max(max_variable, std::ranges::max(constraint.variables));
			  }
			  return max_variable + 1;
		  }() + 1},
		  _domain_size{[&constraints]() {
			  auto max_domain_size = size_t{0};
			  for (auto const& constraint : constraints) {
				  for (auto const& entry : constraint.relation) {
					  max_domain_size = std::max(max_domain_size, std::ranges::max(entry));
				  }
			  }
			  return max_domain_size;
		  }() + 1},
		  _constraints{std::move(constraints)} {}
	CSP(CSP&& csp) = default;
	CSP(CSP const& csp) = default;
	auto operator=(CSP const& other) -> CSP& = default;
	auto operator=(CSP&& other) -> CSP& = default;

	auto n_variables() const -> size_t { return _n_variables; }
	auto domain_size() const -> size_t { return _domain_size; }
	auto constraints() const -> std::vector<Constraint> const& { return _constraints; }

  private:
	size_t _n_variables;
	size_t _domain_size;
	std::vector<Constraint> _constraints{};
};

class SAT {
  public:
	SAT(std::vector<Clause> clauses) : _clauses{std::move(clauses)} {}
	SAT(std::initializer_list<Clause> clauses) : _clauses{std::move(clauses)} {}
	SAT(SAT&& other) = default;
	SAT(SAT const& other) = default;
	auto operator=(SAT const& other) -> SAT& = default;
	auto operator=(SAT&& other) -> SAT& = default;

	auto clauses() const -> std::vector<Clause> const& { return _clauses; }

  private:
	std::vector<Clause> _clauses{};
};