#pragma once

#include "data_structures.hpp"
#include <fmt/format.h>

template <typename T> struct fmt::formatter<FixedSizeVector<T>> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(FixedSizeVector<T> const& array, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(array.begin(), array.end(), ", "));
	}
};

template <> struct fmt::formatter<Relation> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(Relation const& relation, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "[{}]", fmt::join(relation.begin(), relation.end(), ", "));
	}
};

template <> struct fmt::formatter<Constraint> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(Constraint const& constraint, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "{{ variables: ({}), relation: {}}}", constraint.variables,
			constraint.relation);
	}
};

template <> struct fmt::formatter<CSP> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(CSP const& csp, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "CSP{{\n\t{}\n}}",
			fmt::join(csp.constraints().begin(), csp.constraints().end(), ",\n\t"));
	}
};

template <> struct fmt::formatter<Literal> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(Literal const& literal, FormatContext& ctx) const -> decltype(ctx.out()) {
		const auto prefix = literal.variable < 0 ? "¬" : "";
		return fmt::format_to(ctx.out(), "{}{}", prefix, literal.variable);
	}
};

template <> struct fmt::formatter<Clause> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(Clause const& clause, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(ctx.out(), "({})", fmt::join(clause.begin(), clause.end(), " ∨ "));
	}
};

template <> struct fmt::formatter<SAT> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(SAT const& sat, FormatContext& ctx) const -> decltype(ctx.out()) {
		return fmt::format_to(
			ctx.out(), "SAT{{\n\t{}\n}}",
			fmt::join(sat.clauses().begin(), sat.clauses().end(), " ∧\n\t"));
	}
};

template <> struct fmt::formatter<Satisfiability> {
	template <typename ParseContext> constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

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
