#pragma once

#include "types.hpp"
#include <algorithm>
#include <functional>
#include <optional>
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

template <typename T, typename U, typename V>
concept HasInsert = requires(T t, U u, V v) { t.insert(u, v); };

template <std::ranges::input_range Range, std::input_or_output_iterator OutputIterator>
	requires std::default_initializable<std::iter_value_t<OutputIterator>> &&
			 std::constructible_from<
				 std::iter_value_t<OutputIterator>,
				 std::initializer_list<std::ranges::range_value_t<Range>>> &&
			 HasInsert<
				 std::iter_value_t<OutputIterator>,
				 std::ranges::iterator_t<std::iter_value_t<OutputIterator>>,
				 std::ranges::range_value_t<Range>>
			 auto all_subsets(Range r, OutputIterator begin) -> OutputIterator {
	if (r.begin() + 1 == r.end()) {
		*begin = std::iter_value_t<OutputIterator>{};
		return begin + 1;
	}
	const auto end = all_subsets(Range{r.begin() + 1, r.end()}, begin);
	auto new_end = end;
	std::for_each(begin, end, [&](std::iter_value_t<OutputIterator> container) {
		container.insert(container.end(), *r.begin());
		*new_end++ = std::iter_value_t<OutputIterator>{container};
	});
	return new_end;
}

} // namespace gautil