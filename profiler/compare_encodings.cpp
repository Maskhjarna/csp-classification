#include "cspc/data_structures.hpp"
#include "cspc/encodings/direct.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/binary.hpp>
#include <cspc/encodings/direct.hpp>
#include <cspc/encodings/label_cover.hpp>
#include <cspc/kissat.hpp>
#include <execution>
#include <gautil/functional.hpp>
#include <gautil/math.hpp>

constexpr auto N_SAMPLES = 1;

auto duration_to_precise_ms(auto duration) -> f64 {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() /
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
	size_t sat_n_literals;
};

auto clause_n_literals(Clause const& clause) -> size_t { return sizeof(Literal) * clause.size(); }

auto sat_n_literals(SAT const& sat) -> size_t {
	return gautil::fold(sat.clauses(), 0ul, std::plus{}, clause_n_literals);
}

template <typename Encoding>
auto time_encode_ms(Encoding encoding, std::vector<CSP> const& csps) -> std::tuple<f64, size_t> {
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	const auto time_before = std::chrono::system_clock::now();
	for (auto i = 0u; i < N_SAMPLES; ++i) {
		std::transform(
			std::execution::par_unseq, csps.begin(), csps.end(), sats.begin(),
			[&](auto const& csp) { return encoding(csp); });
	}
	const auto time_after = std::chrono::system_clock::now();
	const auto n_literals = gautil::fold(
		sats, 0ul, std::plus{}, [&](auto const& sat) { return sat_n_literals(sat.value()); });
	return {
		duration_to_precise_ms(time_after - time_before) / N_SAMPLES,
		n_literals,
	};
}

template <typename Encoding>
auto time_encode_and_solve_ms(Encoding encoding, std::vector<CSP> const& csps) -> f64 {
	auto sats = std::vector<std::optional<SAT>>(csps.size());
	auto satisfiable = std::vector<Satisfiability>(csps.size());
	const auto time_before = std::chrono::system_clock::now();
	for (auto i = 0u; i < N_SAMPLES; ++i) {
		std::transform(
			std::execution::par_unseq, csps.begin(), csps.end(), sats.begin(),
			[&](auto const& csp) { return encoding(csp); });
		std::transform(
			std::execution::par_unseq, sats.begin(), sats.end(), satisfiable.begin(),
			[&](auto const& sat) { return solvers::solve_kissat(sat.value()); });
	}
	const auto time_after = std::chrono::system_clock::now();
	return duration_to_precise_ms(time_after - time_before) / N_SAMPLES;
}

template <typename Encoding>
auto create_profile_for_encoding(
	Encoding encoding, std::vector<Labeled<std::vector<Relation>>> const& labeled_relation_bundles)
	-> std::vector<Labeled<Profile>> {
	const auto siggers = cspc::siggers_operation();
	auto profiles = std::vector<Labeled<Profile>>(labeled_relation_bundles.size());
	std::transform(
		std::execution::par_unseq, labeled_relation_bundles.begin(), labeled_relation_bundles.end(),
		profiles.begin(), [&](auto const& labeled_relations) {
			return labeled_relations.map([&](auto const& relations) {
				auto csps = std::vector<CSP>{};
				csps.reserve(relations.size());
				std::ranges::transform(
					relations, std::back_inserter(csps), [&](auto const& relation) {
						return cspc::to_preserves_operation_csp(siggers, relation);
					});

				const auto [time_elapsed_encoding_ms, sat_n_literals] =
					time_encode_ms(encoding, csps);
				const auto time_elapsed_total_ms = time_encode_and_solve_ms(encoding, csps);
				return Profile{
					.time_elapsed_encoding_ms = time_elapsed_encoding_ms,
					.time_elapsed_solving_ms = time_elapsed_total_ms - time_elapsed_encoding_ms,
					.time_elapsed_total_ms = time_elapsed_total_ms,
					.sat_n_literals = sat_n_literals,
				};
			});
		});

	return profiles;
}

auto show_profile(Labeled<std::vector<Labeled<Profile>>> const& labeled_profiles) -> void {
	spdlog::info("╒══════════════════════════════════╤══════════════╤══════════════╤══════════════╤"
				 "════════════════╤════════════════╕");
	spdlog::info(
		"│ {:32} │ {:12} │ {:12} │ {:12} │ {:14} │ {:14} │", labeled_profiles.label, "Encoding",
		"~Solving", "Total", "Literals", "Size");
	spdlog::info("├──────────────────────────────────┼──────────────┼──────────────┼──────────────┼"
				 "────────────────┼────────────────┤");
	std::ranges::for_each(labeled_profiles.value, [&](auto const& labeled_profile) {
		const auto size_mb =
			f64(labeled_profile.value.sat_n_literals * sizeof(Literal)) / std::pow(10, 6);
		spdlog::info(
			"│ {:32} │ {:>10.2f}ms │ {:>10.2f}ms │ {:>10.2f}ms │ {:14L} │ {:>11.2f} MB │",
			labeled_profile.label, labeled_profile.value.time_elapsed_encoding_ms,
			labeled_profile.value.time_elapsed_solving_ms,
			labeled_profile.value.time_elapsed_total_ms, labeled_profile.value.sat_n_literals,
			size_mb);
	});
	spdlog::info("╘══════════════════════════════════╧══════════════╧══════════════╧══════════════╧"
				 "════════════════╧════════════════╛");
}

auto main() -> int {
	const auto labeled_encodings = std::vector<Labeled<std::function<SAT(CSP const&)>>>{
		{"Direct encoding", cspc::direct_encoding},
		{"Multivalued direct encoding", cspc::multivalued_direct_encoding},
		{"Binary encoding", cspc::binary_encoding},
		{"Log encoding", cspc::log_encoding},
		{"Label cover encoding", cspc::label_cover_encoding},
		{"Multivalued label cover encoding", cspc::multivalued_label_cover_encoding},
	};
	const auto labeled_relations = std::vector<Labeled<std::vector<Relation>>>{
		{"Binary on domain [0, 2)", cspc::all_nary_relations(2, 2)},
		{"Binary on domain [0, 3)", cspc::all_nary_relations(2, 3)},
		// {"Binary on domain [0, 4)", cspc::all_nary_relations(2, 4)}, // NOTE: slow
		{"Ternary on domain [0, 2)", cspc::all_nary_relations(3, 2)},
	};
	auto profiles = std::vector<Labeled<std::vector<Labeled<Profile>>>>{};
	std::ranges::transform(
		labeled_encodings, std::back_inserter(profiles), [&](auto const& labeled_encoding) {
			spdlog::info("Profiling {}...", labeled_encoding.label);
			return labeled_encoding.map([&](auto const& encoding) {
				return create_profile_for_encoding(encoding, labeled_relations);
			});
		});

	std::locale::global(std::locale("en_US.UTF-8")); // NOTE: for comma sep thousands
	std::ranges::for_each(profiles, show_profile);

	return EXIT_SUCCESS;
}