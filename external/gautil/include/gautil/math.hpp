#pragma once

#include "types.hpp"
#include <functional>

namespace gautil {
extern constexpr auto comptime_pow(u64 base, u64 exponent) -> u64;
extern auto n_choose_k(u64 n, u64 k) -> u64;
extern auto repeat(size_t n, std::function<void(void)> fn) -> void;
} // namespace gautil
