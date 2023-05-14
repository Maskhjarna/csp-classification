#pragma once

#include "data_structures.hpp"
#include <fmt/format.h>

template <> struct fmt::formatter<cspc::relation_entry> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::relation_entry const& array, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(array.begin(), array.end(), ", "));
	}
};

template <> struct fmt::formatter<cspc::relation> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::relation const& relation, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "[{}]", fmt::join(relation.begin(), relation.end(), ", "));
	}
};

template <> struct fmt::formatter<cspc::constraint> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::constraint const& constraint, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "{{ variables: ({}), relation: {}}}", constraint.variables(),
			constraint.get_relation());
	}
};

template <> struct fmt::formatter<cspc::csp> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::csp const& csp, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "csp{{\n\t{}\n}}",
			fmt::join(csp.constraints().begin(), csp.constraints().end(), ",\n\t"));
	}
};

template <> struct fmt::formatter<cspc::literal> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::literal const& literal, FormatContext& ctx) const -> decltype(ctx.out()) {
		const auto prefix = literal.value < 0 ? "¬" : "";
		return fmt::format_to(ctx.out(), "{}{}", prefix, std::abs(literal.value));
	}
};

template <> struct fmt::formatter<cspc::clause> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::clause const& clause, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(clause.begin(), clause.end(), " ∨ "));
	}
};

template <> struct fmt::formatter<cspc::sat> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::sat const& sat, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "\n\t{}\n", fmt::join(sat.clauses().begin(), sat.clauses().end(), " ∧\n\t"));
	}
};

template <> struct fmt::formatter<cspc::satisfiability> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(cspc::satisfiability const& satisfiability, FormatContext& ctx) const
		-> decltype(ctx.out()) {
		auto str = std::string{};
		switch (satisfiability) {
		case cspc::SATISFIABLE:
			str = "Satisfiable";
			break;
		case cspc::UNSATISFIABLE:
			str = "Unatisfiable";
			break;
		}

		return fmt::format_to(ctx.out(), "({})", str);
	}
};
