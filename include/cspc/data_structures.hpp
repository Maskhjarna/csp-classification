#pragma once

#include <algorithm>
#include <gautil/types.hpp>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

using DomainValue = u32;
using Variable = u32;

enum Polarity {
	NEGATED = -1,
	REGULAR = 1,
};

struct Literal {
	Literal(u32 variable, Polarity polarity) : variable{(i64)polarity * (i64)(variable + 1)} {}
	i64 variable;
};

using Clause = std::vector<Literal>;

enum Satisfiability {
	UNSATISFIABLE = 0,
	SATISFIABLE,
};

template <typename T> class FixedSizeVector {
  public:
	FixedSizeVector(std::initializer_list<T> data)
		: m_data{std::move(data)}, m_length{data.size()} {}
	FixedSizeVector(size_t length) : m_data(length), m_length{length} {}

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
	auto operator==(FixedSizeVector const& other) const -> bool {
		return m_length == other.m_length && m_data == other.m_data;
	}

  private:
	std::vector<T> m_data;
	size_t m_length;
};

class Relation {
  public:
	using Entry = FixedSizeVector<DomainValue>;
	Relation() : m_arity{0} {}
	Relation(size_t arity) : m_arity{arity} {}
	Relation(std::initializer_list<Entry> data)
		: m_data{std::move(data)}, m_arity{std::ranges::min(m_data, {}, &Entry::size).size()} {}
	Relation(std::vector<Entry> data)
		: m_data{std::move(data)}, m_arity{std::ranges::min(m_data, {}, &Entry::size).size()} {}

	auto arity() const -> size_t { return m_arity; }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	auto insert(std::vector<Entry>::const_iterator it, Entry val) {
		m_data.insert(it, std::move(val));
	}
	auto add_entry(Entry entry) -> void { m_data.push_back(entry); }
	auto reserve(size_t n) { m_data.reserve(n); }
	auto size() const { return m_data.size(); }
	auto empty() const -> bool { return m_data.empty(); }
	auto erase(Entry const& value) {
		// TODO: Relation would probably be better represented by an unordered_set, but there is no
		// stdlib hash function for vectors
		m_data.erase(std::ranges::find(m_data, value));
	}
	auto operator[](size_t i) -> Entry& { return m_data[i]; };
	auto operator[](size_t i) const -> Entry const& { return m_data[i]; };

  private:
	std::vector<Entry> m_data;
	size_t m_arity;
};

struct Identity {
	Identity(std::initializer_list<std::vector<DomainValue>> elements)
		: elements{std::move(elements)} {}
	const std::vector<std::vector<DomainValue>> elements{};
};

struct Operation {
	Operation(size_t arity, std::initializer_list<Identity> identities)
		: arity{arity}, identities{std::move(identities)} {}
	const size_t arity;
	const std::vector<Identity> identities;
};

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
		Relation const& relation, std::vector<Variable> const& variables, ConstraintTag tag = OTHER)
		: relation{relation}, variables{variables}, tag{tag} {}
	auto relation_size() const -> size_t { return relation.size(); }
	auto arity() const -> size_t { return variables.size(); }

	const Relation relation;
	const std::vector<Variable> variables;
	const ConstraintTag tag;
};

class CSP {
  public:
	CSP(std::vector<Constraint> const& constraints)
		: _n_variables{[&constraints]() {
			  auto max_variable = Variable{0};
			  for (auto const& constraint : constraints) {
				  if (constraint.variables.empty()) {
					  continue;
				  }
				  max_variable = std::max(max_variable, std::ranges::max(constraint.variables));
			  }
			  return max_variable + 1;
		  }() + 1},
		  _domain_size{[&constraints]() {
			  auto max_domain_size = Variable{0};
			  for (auto const& constraint : constraints) {
				  if (constraint.relation.empty()) {
					  continue;
				  }
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