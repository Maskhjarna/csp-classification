#include "cspc/data_structures.hpp"
#include "cspc/encodings/direct.hpp"
#include "gautil/functional.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/binary.hpp>
#include <cspc/encodings/direct.hpp>
#include <cspc/encodings/label_cover.hpp>
#include <cspc/kissat.hpp>

auto duration_to_precise_ms(auto duration) -> f64 {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / std::pow(10, 6);
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
};

auto time_encode_ms(std::function<SAT(CSP)> encoding, std::vector<CSP> const& csps) -> f64 {
	const auto time_before = std::chrono::system_clock::now();
	auto sats = std::vector<SAT>{};
	sats.reserve(csps.size());
	std::ranges::transform(csps, std::back_inserter(sats), encoding);
	const auto time_after = std::chrono::system_clock::now();
	return duration_to_precise_ms(time_after - time_before);
}

auto time_encode_and_solve_ms(std::function<SAT(CSP)> encoding, std::vector<CSP> const& csps)
	-> f64 {
	const auto time_before = std::chrono::system_clock::now();
	auto satisfiable = std::vector<Satisfiability>{};
	satisfiable.reserve(csps.size());
	std::ranges::transform(csps, std::back_inserter(satisfiable), [&](auto const& csp) {
		return solvers::solve_kissat(encoding(csp));
	});
	const auto time_after = std::chrono::system_clock::now();
	return duration_to_precise_ms(time_after - time_before);
}

auto create_profile_for_encoding(
	std::function<SAT(CSP)> encoding,
	std::vector<Labeled<std::vector<Relation>>> const& labeled_relation_bundles)
	-> std::vector<Labeled<Profile>> {
	const auto siggers = cspc::siggers_operation();
	auto profiles = std::vector<Labeled<Profile>>{};
	profiles.reserve(labeled_relation_bundles.size());
	std::ranges::transform(
		labeled_relation_bundles, std::back_inserter(profiles), [&](auto const& labeled_relations) {
			spdlog::info("Relation: {}", labeled_relations.label);
			return labeled_relations.map([&](auto const& relations) {
				auto csps = std::vector<CSP>{};
				csps.reserve(relations.size());
				spdlog::info("Setting up meta-CSP...");
				std::ranges::transform(
					relations, std::back_inserter(csps), [&](auto const& relation) {
						return cspc::to_preserves_operation_csp(siggers, relation);
					});

				spdlog::info("Encoding...");
				const auto time_elapsed_encoding_ms = time_encode_ms(encoding, csps);
				spdlog::info("Encoding and solving...");
				const auto time_elapsed_total_ms = time_encode_and_solve_ms(encoding, csps);
				return Profile{
					.time_elapsed_encoding_ms = time_elapsed_encoding_ms,
					.time_elapsed_solving_ms = time_elapsed_total_ms - time_elapsed_encoding_ms,
					.time_elapsed_total_ms = time_elapsed_total_ms,
				};
			});
		});

	return profiles;
}

auto show_profile(Labeled<std::vector<Labeled<Profile>>> const& labeled_profiles) -> void {
	spdlog::info(
		"╒══════════════════════════════════╤══════════════╤══════════════╤══════════════╕");
	spdlog::info(
		"│ {:32} │ {:12} │ {:12} │ {:12} │", labeled_profiles.label, "Encoding", "~Solving",
		"Total");
	spdlog::info(
		"├──────────────────────────────────┼──────────────┼──────────────┼──────────────┤");
	std::ranges::for_each(labeled_profiles.value, [&](auto const& labeled_profile) {
		spdlog::info(
			"│ {:32} │ {:>10.2f}ms │ {:>10.2f}ms │ {:>10.2f}ms │", labeled_profile.label,
			labeled_profile.value.time_elapsed_encoding_ms,
			labeled_profile.value.time_elapsed_solving_ms,
			labeled_profile.value.time_elapsed_total_ms);
	});
	spdlog::info(
		"╘══════════════════════════════════╧══════════════╧══════════════╧══════════════╛");
}

auto main() -> int {
	const auto labeled_encodings = std::vector<Labeled<std::function<SAT(CSP)>>>{
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

	std::ranges::for_each(profiles, show_profile);

	return EXIT_SUCCESS;
}