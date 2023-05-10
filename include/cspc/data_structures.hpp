#pragma once

#include <algorithm>
#include <functional>
#include <gautil/types.hpp>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

namespace cspc {
using domain_value = u32;
using variable = u32;

// a fixed size container
class relation_entry {
  public:
	relation_entry(relation_entry const& other) = default;
	relation_entry(relation_entry&& other) = default;
	relation_entry(std::initializer_list<domain_value> data)
		: m_data{std::move(data)}, m_length{data.size()} {}
	relation_entry(size_t length) : m_data(length), m_length{length} {}

	auto length() const -> size_t { return m_length; }
	auto begin() { return m_data.begin(); }
	auto end() { return m_data.end(); }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	auto size() const { return m_data.size(); }
	auto operator[](size_t i) -> domain_value& { return m_data[i]; };
	auto operator[](size_t i) const -> domain_value const& { return m_data[i]; };
	auto operator=(relation_entry const& other) -> relation_entry& = default;
	auto operator=(relation_entry&& other) -> relation_entry& = default;
	auto operator==(relation_entry const& other) const -> bool {
		return m_length == other.m_length && m_data == other.m_data;
	}
	auto operator<(relation_entry const& other) const -> bool {
		if (m_length < other.m_length) {
			return true;
		}
		if (m_length > other.m_length) {
			return false;
		}
		for (auto i = 0u; i < other.size(); ++i) {
			if (m_data[i] == other.m_data[i]) {
				continue;
			}
			return m_data[i] < other.m_data[i];
		}
		return false;
	}
	auto operator>(relation_entry const& other) const -> bool {
		return !(*this == other) && !(*this < other);
	}
	auto operator>=(relation_entry const& other) const -> bool {
		return (*this == other) || (*this > other);
	}
	auto operator<=(relation_entry const& other) const -> bool {
		return (*this == other) || (*this < other);
	}

  private:
	std::vector<domain_value> m_data;
	size_t m_length;
};

class relation {
  public:
	relation() : m_arity{0} {}
	relation(size_t arity) : m_arity{arity} {}
	relation(std::initializer_list<relation_entry> data)
		: m_data{std::move(data)},
		  m_arity{std::ranges::min(m_data, {}, &relation_entry::size).size()} {}
	relation(std::vector<relation_entry> data)
		: m_data{std::move(data)},
		  m_arity{std::ranges::min(m_data, {}, &relation_entry::size).size()} {}

	auto arity() const -> size_t { return m_arity; }
	auto begin() const { return m_data.begin(); }
	auto end() const { return m_data.end(); }
	auto data() const { return m_data; }
	auto insert(std::vector<relation_entry>::const_iterator it, relation_entry val) {
		m_data.insert(it, std::move(val));
	}
	auto add_entry(relation_entry entry) -> void { m_data.push_back(entry); }
	auto reserve(size_t n) { m_data.reserve(n); }
	auto size() const { return m_data.size(); }
	auto empty() const -> bool { return m_data.empty(); }
	auto erase(relation_entry const& value) {
		// NOTE: relation would probably be better represented by an unordered_set, but there is no
		// stdlib hash function for vectors
		m_data.erase(std::ranges::find(m_data, value));
	}
	auto operator[](size_t i) -> relation_entry& { return m_data[i]; };
	auto operator[](size_t i) const -> relation_entry const& { return m_data[i]; };

  private:
	std::vector<relation_entry> m_data;
	size_t m_arity;
};

struct identity {
	identity(std::initializer_list<std::vector<domain_value>> elements)
		: elements{std::move(elements)} {}
	const std::vector<std::vector<domain_value>> elements{};
};

struct operation {
	operation(size_t arity, std::initializer_list<identity> identities)
		: arity{arity}, identities{std::move(identities)} {}
	const size_t arity;
	const std::vector<identity> identities;
};

enum constraint_tag {
	EQ,
	NE,
	GT,
	GE,
	LT,
	LE,
	IS,
	OTHER,
};

class constraint {
  public:
	// TODO: auto find constraint tag / construct from constraint tag
	constraint(
		relation const& relation,
		std::vector<variable> const& variables,
		constraint_tag tag = OTHER)
		: m_relation{relation}, m_variables{variables}, m_tag{tag} {}
	auto relation_size() const -> size_t { return m_relation.size(); }
	auto arity() const -> size_t { return m_variables.size(); }
	auto get_relation() const -> relation { return m_relation; }
	auto variables() const -> std::vector<variable> { return m_variables; }
	auto tag() const -> constraint_tag { return m_tag; }

  private:
	relation m_relation;
	std::vector<variable> m_variables;
	constraint_tag m_tag;
};

class csp {
  public:
	csp(std::vector<constraint> const& constraints)
		: m_n_variables{[&constraints]() {
			  auto max_variable = variable{0};
			  for (auto const& constraint : constraints) {
				  if (constraint.variables().empty()) {
					  continue;
				  }
				  max_variable = std::max(max_variable, std::ranges::max(constraint.variables()));
			  }
			  return max_variable + 1;
		  }() + 1},
		  m_domain_size{[&constraints]() {
			  auto max_domain_size = variable{0};
			  for (auto const& constraint : constraints) {
				  if (constraint.get_relation().empty()) {
					  continue;
				  }
				  for (auto const& entry : constraint.get_relation()) {
					  max_domain_size = std::max(max_domain_size, std::ranges::max(entry));
				  }
			  }
			  return max_domain_size;
		  }() + 1},
		  m_constraints{std::move(constraints)} {}
	csp(csp&& csp) = default;
	csp(csp const& csp) = default;
	auto operator=(csp const& other) -> csp& = default;
	auto operator=(csp&& other) -> csp& = default;

	auto n_variables() const -> size_t { return m_n_variables; }
	auto domain_size() const -> size_t { return m_domain_size; }
	auto constraints() const -> std::vector<constraint> const& { return m_constraints; }

  private:
	size_t m_n_variables;
	size_t m_domain_size;
	std::vector<constraint> m_constraints{};
};

enum polarity {
	NEGATED = -1,
	REGULAR = 1,
};

struct literal {
	literal(u32 variable, polarity pol) : value{(i64)pol * (i64)(variable + 1)} {}
	auto variable() const -> u32 { return std::abs(value); }
	i64 value;
};

using clause = std::vector<literal>;

class sat {
  public:
	sat(std::vector<clause> clauses) : _clauses{std::move(clauses)} {}
	sat(std::initializer_list<clause> clauses) : _clauses{std::move(clauses)} {}
	sat(sat&& other) = default;
	sat(sat const& other) = default;
	auto operator=(sat const& other) -> sat& = default;
	auto operator=(sat&& other) -> sat& = default;

	auto clauses() const -> std::vector<clause> const& { return _clauses; }

  private:
	std::vector<clause> _clauses{};
};

// returned by solvers
enum satisfiability {
	UNSATISFIABLE = 0,
	SATISFIABLE,
};

using solver = std::function<satisfiability(sat)>;

} // namespace cspc
