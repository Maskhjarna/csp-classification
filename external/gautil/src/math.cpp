#include "gautil/math.hpp"

namespace gautil {
constexpr auto comptime_pow(u64 base, u64 exponent) -> u64 {
	if (exponent == 0) {
		return 1;
	}
	return base * comptime_pow(base, exponent - 1);
}

auto n_choose_k(u64 n, u64 k) -> u64 {
	if (k == 0) {
		return 1;
	}
	return (n * n_choose_k(n - 1, k - 1)) / k;
}
} // namespace gautil