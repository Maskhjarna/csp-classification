#pragma once

#include "data_structures.hpp"
#include <cspc/formatters.hpp>

extern "C" {
#include <kissat.h>
}

namespace solvers {
extern auto solve_kissat(SAT const& sat) -> Satisfiability;
}
