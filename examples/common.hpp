#pragma once

#include <algorithm>
#include <ranges>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/spdlog.h>

extern auto siggers_all_nary_on_domain(size_t n, size_t domain_size) -> void;