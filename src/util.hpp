#pragma once

#include "data_structures.hpp"
#include "types.hpp"
#include <fmt/format.h>
#include <functional>
#include <ranges>
#include <vector>

namespace util {
extern constexpr auto comptime_pow(u64 base, u64 exponent) -> u64;
extern auto n_choose_k(u64 n, u64 k) -> u64;
extern auto call(std::string const& command, std::string const& input = "")
	-> std::optional<std::string>;

template <typename T>
extern auto try_unwrap(std::vector<std::optional<T>> const& v) -> std::optional<std::vector<T>> {
	if (!std::ranges::all_of(
			v, [](std::optional<T> const& maybe_val) { return maybe_val.has_value(); })) {
		return std::nullopt;
	}
	auto result = std::vector<T>(v.size());
	std::ranges::transform(
		v, result.begin(), [](std::optional<T> const& maybe_val) { return maybe_val.value(); });
	return result;
}

template <typename T, typename U>
auto map_maybe(std::optional<T> maybe_t, std::function<U(T)> const& fn) -> std::optional<U> {
	if (maybe_t.has_value()) {
		return fn(maybe_t.value());
	}
	return std::nullopt;
}

template <typename T, typename U>
auto transform_maybes(auto range, auto result, std::function<U(T)> const& fn) {
	std::ranges::transform(
		range, result, [fn](std::optional<T> const& maybe_t) -> std::optional<U> {
			return map_maybe(maybe_t, fn);
		});
}

template <typename T, typename U>
auto transform_maybes(auto range, auto result, std::function<std::optional<U>(T)> const& fn) {
	std::ranges::transform(
		range, result, [fn](std::optional<T> const& maybe_t) -> std::optional<U> {
			auto result = map_maybe(maybe_t, fn);
			if (result.has_value()) {
				return result.value();
			}
			return std::nullopt;
		});
}

template <typename T, typename U>
auto lift_optional(std::function<U(T)> fn) -> std::function<std::optional<U>(std::optional<T>)> {
	return [fn](std::optional<T> maybe_t) -> std::optional<U> {
		if (maybe_t.has_value()) {
			return std::invoke(fn, maybe_t.value());
		}
		return std::nullopt;
	};
}

} // namespace util

template <typename T> struct fmt::formatter<std::vector<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(std::vector<T> const& v, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "{}", fmt::join(v.begin(), v.end(), ", "));
	}
};

template <typename T> struct fmt::formatter<RuntimeArray<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(RuntimeArray<T> const& array, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(array.begin(), array.end(), ", "));
	}
};

template <> struct fmt::formatter<Relation> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(Relation const& relation, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "[{}]", fmt::join(relation.begin(), relation.end(), ", "));
	}
};

template <typename T> struct fmt::formatter<std::optional<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(std::optional<T> const& maybe_t, FormatContext& ctx) const -> decltype(ctx.out()) {
		if (maybe_t.has_value()) {
			return fmt::format_to(ctx.out(), "Just {}", maybe_t.value());
		}
		return fmt::format_to(ctx.out(), "Nothing");
	}
};

template <> struct fmt::formatter<Constraint> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(Constraint const& constraint, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "{{ variables: ({}), relation: {}}}", constraint.variables,
			constraint.relation);
	}
};

template <> struct fmt::formatter<CSP> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(CSP const& csp, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "CSP{{\n\t{}\n}}",
			fmt::join(csp.constraints().begin(), csp.constraints().end(), ",\n\t"));
	}
};

template <> struct fmt::formatter<Literal> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(Literal const& literal, FormatContext& ctx) const -> decltype(ctx.out()) {
		const auto prefix = literal.negation == NEGATED ? "¬" : "";
		return fmt::format_to(ctx.out(), "{}{}", prefix, literal.variable);
	}
};

template <> struct fmt::formatter<Clause> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(Clause const& clause, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(clause.begin(), clause.end(), " ∨ "));
	}
};

template <> struct fmt::formatter<SAT> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(SAT const& sat, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "SAT{{\n\t{}\n}}",
			fmt::join(sat.clauses().begin(), sat.clauses().end(), " ∧\n\t"));
	}
};

template <> struct fmt::formatter<Satisfiability> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	};

	template <typename FormatContext>
	auto format(Satisfiability const& satisfiability, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		auto str = std::string{};
		switch (satisfiability) {
		case SATISFIABLE:
			str = "Satisfiable";
			break;
		case UNSATISFIABLE:
			str = "Unatisfiable";
			break;
		}

		return fmt::format_to(ctx.out(), "({})", str);
	}
};
