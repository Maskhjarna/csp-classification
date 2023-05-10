#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto direct_encoding(csp const& csp) -> sat;
extern auto multivalued_direct_encoding(csp const& csp) -> sat;
} // namespace cspc
