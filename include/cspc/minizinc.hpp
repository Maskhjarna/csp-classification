#pragma once

#include "data_structures.hpp"

namespace cspc {
constexpr auto EXPORT_FILE_PATH = "output/temp.mzn";
extern auto csp_to_minizinc(cspc::csp const& csp) -> std::string;
extern auto write_csp_as_minizinc(cspc::csp const& csp) -> std::optional<std::string>;
extern auto minizinc_is_satisfiable(cspc::csp const& csp) -> std::optional<cspc::satisfiability>;
} // namespace cspc
