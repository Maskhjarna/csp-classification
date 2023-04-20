#include "gautil/math.hpp"

namespace gautil {
auto n_choose_k(u64 n, u64 k) -> u64 {
	if (k == 0) {
		return 1;
	}
	return (n * n_choose_k(n - 1, k - 1)) / k;
}

auto round_up_to_power_of_2(u64 v) -> u64 {
	// https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	return v++;
}
} // namespace gautil