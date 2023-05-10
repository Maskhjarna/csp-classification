#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto label_cover_encoding(csp const& csp) -> sat;
extern auto multivalued_label_cover_encoding(csp const& csp) -> sat;
} // namespace cspc
