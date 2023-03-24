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
	RuntimeArray(std::initializer_list<T> ts) : length{ts.size()}, m_data(std::move(ts)) {}
	RuntimeArray(size_t length) : length{length}, m_data{std::vector<T>(length)} {}
	auto operator[](size_t i) -> T& { return m_data.at(i); }
	auto operator[](size_t i) const -> T const& { return m_data.at(i); }
	auto operator==(RuntimeArray const& other) const -> bool {
		return length == other.length && std::ranges::equal(*this, other);
	}
	auto check() const -> void {
		spdlog::info(length);
		spdlog::info(m_data.size());
		assert(length == m_data.size());
		assert(length > 0);
	}
	constexpr auto begin() { return m_data.begin(); }
	constexpr auto end() { return m_data.end(); }
	constexpr auto begin() const { return m_data.begin(); }
	constexpr auto end() const { return m_data.end(); }
	auto size() const { return m_data.size(); }
	auto data() const -> std::vector<T> const& { return m_data; }

	const size_t length;

  private:
	std::vector<T> m_data{};
};

class Relation {
  public:
	Relation(size_t arity, std::initializer_list<RuntimeArray<size_t>> data)
		: m_arity{arity}, m_data{std::move(data)} {}
	Relation(size_t arity) : m_arity{arity} {}
	auto operator[](size_t i) -> RuntimeArray<size_t>& { return m_data.at(i); }
	auto operator[](size_t i) const -> RuntimeArray<size_t> const& { return m_data.at(i); }
	constexpr auto begin() { return m_data.begin(); }
	constexpr auto end() { return m_data.end(); }
	constexpr auto begin() const { return m_data.begin(); }
	constexpr auto end() const { return m_data.end(); }
	auto reserve(size_t s) -> void { m_data.reserve(s); }
	auto size() const -> size_t { return m_data.size(); }
	auto add_entry(RuntimeArray<size_t> entry) -> void { m_data.push_back(std::move(entry)); }
	auto arity() const -> size_t { return m_arity; }

  private:
	size_t m_arity;
	std::vector<RuntimeArray<size_t>> m_data{};
};

struct Identity {
	Identity(std::initializer_list<std::vector<size_t>> elements) : elements{std::move(elements)} {}
	const std::vector<std::vector<size_t>> elements{};
};

struct Operation : RuntimeArray<std::optional<size_t>> {
	Operation(size_t arity, size_t domain_size, std::initializer_list<Identity> identities)
		: RuntimeArray<std::optional<size_t>>((size_t)std::pow(domain_size, arity)), arity{arity},
		  domain_size{domain_size}, identities{std::move(identities)} {}
	// TODO calculate from identities and values
	const size_t arity;
	const size_t domain_size;
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