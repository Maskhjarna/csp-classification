#pragma once

#include "data_structures.hpp"
#include <cspc/formatters.hpp>

extern "C" {
#include <kissat.h>
}

namespace cspc {
extern auto kissat_is_satisfiable(sat const& sat) -> satisfiability;
}
