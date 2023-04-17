#pragma once

#include "types.hpp"
#include <algorithm>
#include <bits/iterator_concepts.h>
#include <functional>
#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

namespace gautil {
extern auto repeat(size_t n, std::function<void()> fn) -> void;

template <typename T>
auto try_unwrap(std::vector<std::optional<T>> const& v) -> std::optional<std::vector<T>> {
	if (!std::ranges::all_of(
			v, [](std::optional<T> const& optional_val) { return optional_val.has_value(); })) {
		return std::nullopt;
	}
	auto result = std::vector<T>(v.size());
	std::ranges::transform(v, result.begin(), [](std::optional<T> const& optional_val) {
		return optional_val.value();
	});
	return result;
}

template <typename T, typename U>
auto map_optional(std::optional<T> optional_t, std::function<U(T)> const& fn) -> std::optional<U> {
	if (optional_t.has_value()) {
		return fn(optional_t.value());
	}
	return std::nullopt;
}

template <typename T, typename U>
auto transform_optionals(auto range, auto result, std::function<U(T)> const& fn) {
	std::ranges::transform(
		range, result, [fn](std::optional<T> const& optional_t) -> std::optional<U> {
			return map_optional(optional_t, fn);
		});
}

template <typename T, typename U>
auto transform_optionals(auto range, auto result, std::function<std::optional<U>(T)> const& fn) {
	std::ranges::transform(
		range, result, [fn](std::optional<T> const& optional_t) -> std::optional<U> {
			auto result = map_optional(optional_t, fn);
			if (result.has_value()) {
				return result.value();
			}
			return std::nullopt;
		});
}

template <typename T, typename U>
auto lift_optional(std::function<U(T)> fn) -> std::function<std::optional<U>(std::optional<T>)> {
	return [fn](std::optional<T> optional_t) -> std::optional<U> {
		if (optional_t.has_value()) {
			return std::invoke(fn, optional_t.value());
		}
		return std::nullopt;
	};
}
template <std::ranges::input_range InputRange>
auto all_subsets(InputRange&& in)
	-> std::vector<std::vector<std::ranges::range_value_t<InputRange>>> {
	using T = std::ranges::range_value_t<InputRange>;
	auto result = std::vector<std::vector<T>>{std::vector<T>{}};
	result.reserve(std::pow(2, in.size()));
	std::ranges::for_each(in, [&](T const& t) {
		std::ranges::for_each(result, [&](std::vector<T> const& v) {
			auto n = std::vector<T>{};
			n.reserve(v.size() + 1);
			std::ranges::copy(v, std::back_inserter(n));
			n.push_back(t);
			result.push_back(std::move(n));
		});
	});
	return result;
}

} // namespace gautil