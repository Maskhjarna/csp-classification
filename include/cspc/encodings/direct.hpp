#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto direct_encoding(CSP const& csp) -> SAT;
extern auto multivalued_direct_encoding(CSP const& csp) -> SAT;
} // namespace cspc
