#pragma once

#include "types.hpp"
#include <functional>

namespace gautil {
template <u64 base, u64 exponent>
constexpr auto comptime_pow = base * comptime_pow<base, exponent - 1>;
template <u64 base> constexpr auto comptime_pow<base, 0> = 1;

extern auto n_choose_k(u64 n, u64 k) -> u64;
extern auto repeat(size_t n, std::function<void(void)> fn) -> void;
extern auto round_up_to_power_of_2(u64 v) -> u64;
} // namespace gautil
