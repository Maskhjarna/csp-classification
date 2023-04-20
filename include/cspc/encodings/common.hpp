#pragma once

#include "../data_structures.hpp"

namespace cspc {
namespace __internal {
extern auto
at_least_one_clauses(Constraint const& constraint, size_t domain_size, std::vector<Clause>& result)
	-> void;

extern auto
at_most_one_clauses(Constraint const& constraint, size_t domain_size, std::vector<Clause>& result)
	-> void;
} // namespace __internal
} // namespace cspc