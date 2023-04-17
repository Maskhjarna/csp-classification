#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto binary_encoding(CSP const& csp) -> SAT;
extern auto log_encoding(CSP const& csp) -> SAT;
} // namespace cspc
