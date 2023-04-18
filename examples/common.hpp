#pragma once

#include <algorithm>
#include <cspc/data_structures.hpp>
#include <ranges>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/spdlog.h>

extern auto duration_to_precise_ms(auto duration) -> f64;
extern auto
siggers_all_nary_on_domain_parallell(size_t n, size_t domain_size, std::function<SAT(CSP)> encoding)
	-> void;
extern auto siggers_all_nary_on_domain_sequential(
	size_t n, size_t domain_size, std::function<SAT(CSP)> encoding) -> void;
