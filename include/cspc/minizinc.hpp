#pragma once

#include "data_structures.hpp"

constexpr auto EXPORT_DIR = std::string{"output/"};

namespace solvers {
extern auto csp_to_minizinc(CSP const& csp) -> std::optional<std::string>;
extern auto write_csp_as_minizinc(CSP const& csp) -> std::optional<std::string>;
extern auto solve_minizinc(CSP const& csp) -> std::optional<Satisfiability>;
} // namespace solvers
