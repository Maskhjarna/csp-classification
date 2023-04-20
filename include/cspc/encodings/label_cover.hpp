#pragma once

#include "../data_structures.hpp"

namespace cspc {
extern auto label_cover_encoding(CSP const& csp) -> SAT;
extern auto multivalued_label_cover_encoding(CSP const& csp) -> SAT;
} // namespace cspc
