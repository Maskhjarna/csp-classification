#pragma once

#include <algorithm>
#include <array>
#include <vector>

namespace gautil {
template <typename T, std::size_t s>
auto array_to_vector(std::array<T, s> const& arr) -> std::vector<T> {
	auto result = std::vector<T>{};
	result.reserve(s);
	std::ranges::copy(arr, std::back_inserter(result));
	return result;
}
} // namespace gautil
