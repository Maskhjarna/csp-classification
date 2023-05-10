#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto binary_encoding(csp const& csp) -> sat;
extern auto log_encoding(csp const& csp) -> sat;
} // namespace cspc
