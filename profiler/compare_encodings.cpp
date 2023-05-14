#include "cspc/data_structures.hpp"
#include "cspc/encodings/direct.hpp"
#include <algorithm>
#include <cspc/algorithms.hpp>
#include <cspc/encodings/binary.hpp>
#include <cspc/encodings/direct.hpp>
#include <cspc/encodings/label_cover.hpp>
#include <cspc/kissat.hpp>
#include <execution>
#include <gautil/functional.hpp>
#include <gautil/math.hpp>

constexpr auto N_SAMPLES = 10;

auto duration_to_precise_ms(auto duration) -> f64 {
	return (f64)std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() /
		   gautil::comptime_pow<10, 6>;
}

template <typename T> struct Labeled {
	std::string label;
	T value;
	template <typename Callable>
	auto map(Callable fn) const -> Labeled<std::invoke_result_t<Callable, T>> {
		return {label, std::invoke(fn, value)};
	}
};

struct Profile {
	f64 time_elapsed_encoding_ms;
	f64 time_elapsed_solving_ms;
	f64 time_elapsed_total_ms;
	size_t sat_n_variables;
	size_t sat_n_literals;
	size_t sat_n_bytes;
};

auto sat_n_variables(cspc::sat const& sat) -> size_t {
	const auto result = gautil::fold(
		sat.clauses(), u64(0), [&](u64 lhs, u64 rhs) { return std::max(lhs, rhs); },
		[&](auto const& clause) {
			return std::ranges::max(clause, std::greater{}, &cspc::literal::variable).variable();
		});
	return result;
}

auto sat_n_literals(cspc::sat const& sat) -> size_t {
	return gautil::fold(sat.clauses(), 0ul, std::plus{}, &cspc::clause::size);
}

auto sat_n_bytes(cspc::sat const& sat) -> size_t {
	return sizeof(cspc::sat) + sat.clauses().size() * sizeof(cspc::clause) +
		   sat_n_literals(sat) * sizeof(cspc::literal);
}

template <typename Encoding>
auto profile_encode_and_encode_plus_solve(Encoding encoding, std::vector<cspc::csp> const& csps)
	-> Profile {

	// time encode
	const auto time_before = std::chrono::system_clock::now();
	for (auto i = 0u; i < N_SAMPLES; ++i) {
		std::for_each(std::execution::par_unseq, csps.begin(), csps.end(), encoding);
	}
	const auto time_between = std::chrono::system_clock::now();

	// time encode and solve
	for (auto i = 0u; i < N_SAMPLES; ++i) {
		std::for_each(std::execution::par_unseq, csps.begin(), csps.end(), [&](auto const& csp) {
			cspc::kissat_is_satisfiable(encoding(csp));
		});
	}
	const auto time_after = std::chrono::system_clock::now();

	// count literals, variables, and bytes
	const auto n_variables = gautil::fold(
		csps, 0u, std::plus{}, [&](auto const& csp) { return sat_n_variables(encoding(csp)); });
	const auto n_literals = gautil::fold(
		csps, 0u, std::plus{}, [&](auto const& csp) { return sat_n_literals(encoding(csp)); });
	const auto n_bytes = gautil::fold(
		csps, 0u, std::plus{}, [&](auto const& csp) { return sat_n_bytes(encoding(csp)); });

	// calculate time spans
	const auto time_elapsed_encoding_ms =
		duration_to_precise_ms(time_between - time_before) / N_SAMPLES;
	const auto time_elapsed_total_ms =
		duration_to_precise_ms(time_after - time_between) / N_SAMPLES;
	const auto time_elapsed_solving_ms = time_elapsed_total_ms - time_elapsed_encoding_ms;

	return Profile{
		.time_elapsed_encoding_ms = time_elapsed_encoding_ms,
		.time_elapsed_solving_ms = time_elapsed_solving_ms,
		.time_elapsed_total_ms = time_elapsed_total_ms,
		.sat_n_variables = n_variables,
		.sat_n_literals = n_literals,
		.sat_n_bytes = n_bytes,
	};
}

template <typename Encoding>
auto create_profile_for_encoding(
	Encoding encoding,
	std::vector<Labeled<std::vector<cspc::relation>>> const& labeled_relation_bundles)
	-> std::vector<Labeled<Profile>> {
	const auto siggers = cspc::siggers_operation();
	auto profiles = std::vector<Labeled<Profile>>{};
	profiles.reserve(labeled_relation_bundles.size());
	std::ranges::transform(
		labeled_relation_bundles, std::back_inserter(profiles), [&](auto const& labeled_relations) {
			spdlog::info("    ... {}", labeled_relations.label);
			return labeled_relations.map([&](auto const& relations) {
				auto csps = std::vector<cspc::csp>{};
				csps.reserve(relations.size());
				std::ranges::transform(
					relations, std::back_inserter(csps), [&](auto const& relation) {
						return cspc::to_preserves_operation_csp(siggers, relation);
					});
				return profile_encode_and_encode_plus_solve(encoding, csps);
			});
		});

	return profiles;
}

auto show_profile(Labeled<std::vector<Labeled<Profile>>> const& labeled_profiles) -> void {
	spdlog::info("╒══════════════════════════════════╤══════════════╤══════════════╤══════════════╤"
				 "════════════════╤════════════════╤════════════════╕");
	spdlog::info(
		"│ {:32} │ {:12} │ {:12} │ {:12} │ {:14} │ {:14} │ {:14} │", labeled_profiles.label,
		"Encoding", "~Solving", "Total", "Variables", "cspc::literals", "Size");
	spdlog::info("├──────────────────────────────────┼──────────────┼──────────────┼──────────────┼"
				 "────────────────┼────────────────┼────────────────┤");
	std::ranges::for_each(labeled_profiles.value, [&](auto const& labeled_profile) {
		const auto size_mb =
			f64(labeled_profile.value.sat_n_literals * sizeof(cspc::literal)) / std::pow(10, 6);
		spdlog::info(
			"│ {:32} │ {:>10.2f}ms │ {:>10.2f}ms │ {:>10.2f}ms │ {:14L} │ {:14L} │ {:>11.2f} MB │",
			labeled_profile.label, labeled_profile.value.time_elapsed_encoding_ms,
			labeled_profile.value.time_elapsed_solving_ms,
			labeled_profile.value.time_elapsed_total_ms, labeled_profile.value.sat_n_variables,
			labeled_profile.value.sat_n_literals, size_mb);
	});
	spdlog::info("╘══════════════════════════════════╧══════════════╧══════════════╧══════════════╧"
				 "════════════════╧════════════════╧════════════════╛");
}

auto show_profiles_raw(
	std::vector<Labeled<std::vector<Labeled<Profile>>>> const& labeled_profile_bundles) -> void {
	for (auto i = 0u; i < labeled_profile_bundles.size(); ++i) {
		std::ranges::for_each(labeled_profile_bundles, [&](auto const& labeled_profile_bundle) {
			const auto labeled_profile = labeled_profile_bundle.value[i];
			const auto size_mb =
				f64(labeled_profile.value.sat_n_literals * sizeof(cspc::literal)) / std::pow(10, 6);
			spdlog::info(
				"│ {:32} │ {:>10.2f}ms │ {:>10.2f}ms │ {:>10.2f}ms │ {:14L} │ {:14L} │ {:>11.2f} "
				"MB │",
				labeled_profile.label, labeled_profile.value.time_elapsed_encoding_ms,
				labeled_profile.value.time_elapsed_solving_ms,
				labeled_profile.value.time_elapsed_total_ms, labeled_profile.value.sat_n_variables,
				labeled_profile.value.sat_n_literals, size_mb);
		});
	}
}

auto main() -> int {
	const auto labeled_encodings = std::vector<Labeled<std::function<cspc::sat(cspc::csp const&)>>>{
		{"Direct encoding", cspc::direct_encoding},
		{"Multivalued direct encoding", cspc::multivalued_direct_encoding},
		{"Binary encoding", cspc::binary_encoding},
		{"Log encoding", cspc::log_encoding},
		{"Label cover encoding", cspc::label_cover_encoding},
		{"Multivalued label cover encoding", cspc::multivalued_label_cover_encoding},
	};
	const auto labeled_relations = std::vector<Labeled<std::vector<cspc::relation>>>{
		{"All binary on domain [0, 2)", cspc::all_nary_relations(2, 2)},
		{"All binary on domain [0, 3)", cspc::all_nary_relations(2, 3)},
		// {"All binary on domain [0, 4)", cspc::all_nary_relations(2, 4)}, // NOTE: slow
		{"All ternary on domain [0, 2)", cspc::all_nary_relations(3, 2)},
		// {"Quaternary on domain [0, 2)", cspc::all_nary_relations(2, 4)}, // NOTE: slooow
		{"{!=} on domain [0, 4)", {cspc::neq_relation(2, 4)}},
		{"{!=} on domain [0, 5)", {cspc::neq_relation(2, 5)}},
		{"{!=, ==} on domain [0, 4)", {cspc::neq_relation(2, 4), cspc::eq_relation(2, 4)}},
	};
	auto profiles = std::vector<Labeled<std::vector<Labeled<Profile>>>>{};
	std::ranges::transform(
		labeled_encodings, std::back_inserter(profiles), [&](auto const& labeled_encoding) {
			spdlog::info("Profiling {}...", labeled_encoding.label);
			return labeled_encoding.map([&](auto const& encoding) {
				return create_profile_for_encoding(encoding, labeled_relations);
			});
		});

	// NOTE: for comma sep thousands
	try {
		std::locale::global(std::locale("en_US.UTF8"));
	} catch (std::runtime_error e) {
		spdlog::warn("Failed to set global locale to \"en_US.UTF8\".");
	}
	show_profiles_raw(profiles);
	// std::ranges::for_each(profiles, show_profile);

	return EXIT_SUCCESS;
}