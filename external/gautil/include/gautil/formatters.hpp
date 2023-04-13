#pragma once

#include <fmt/format.h>
#include <optional>
#include <vector>

template <typename T> struct fmt::formatter<std::vector<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(std::vector<T> const& v, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "{}", fmt::join(v.begin(), v.end(), ", "));
	}
};

template <typename T> struct fmt::formatter<std::optional<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(std::optional<T> const& maybe_t, FormatContext& ctx) const -> decltype(ctx.out()) {
		if (maybe_t.has_value()) {
			return fmt::format_to(ctx.out(), "Just {}", maybe_t.value());
		}
		return fmt::format_to(ctx.out(), "Nothing");
	}
};
