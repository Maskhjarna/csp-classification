#include "test_encodings.hpp"

#include "cspc/algorithms.hpp"
#include "cspc/data_structures.hpp"
#include <cspc/encodings/binary.hpp>
#include <cspc/encodings/direct.hpp>
#include <cspc/encodings/label_cover.hpp>
#include <cspc/kissat.hpp>
#include <gautil/formatters.hpp>
#include <gautil/math.hpp>
#include <gautil/misc.hpp>

constexpr auto EXPECTED_BINARY_ON_DOMAIN_2_SATISFIABILITY = std::array<cspc::satisfiability, 3>{
	cspc::SATISFIABLE, // [(1, 0)]
	cspc::SATISFIABLE, // [(0, 1)]
	cspc::SATISFIABLE, // [(1, 0), (0, 1)]
};

constexpr auto EXPECTED_BINARY_ON_DOMAIN_3_SATISFIABILITY = std::array<cspc::satisfiability, 63>{
	cspc::SATISFIABLE,	 // [(1, 0)]
	cspc::SATISFIABLE,	 // [(2, 0)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0)]
	cspc::SATISFIABLE,	 // [(0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (0, 1)]
	cspc::SATISFIABLE,	 // [(2, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 1)]
	cspc::SATISFIABLE,	 // [(2, 0), (2, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (2, 1)]
	cspc::SATISFIABLE,	 // [(0, 1), (2, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1), (2, 1)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 1), (2, 1)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (0, 1), (2, 1)]
	cspc::SATISFIABLE,	 // [(0, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (0, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (0, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(2, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (2, 1), (0, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (2, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (2, 1), (0, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (0, 1), (2, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 1), (2, 1), (0, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (0, 1), (2, 1), (0, 2)]
	cspc::SATISFIABLE,	 // [(1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (1, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 1), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (0, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1), (2, 1), (1, 2)]
	cspc::UNSATISFIABLE, // [(2, 0), (0, 1), (2, 1), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (0, 1), (2, 1), (1, 2)]
	cspc::SATISFIABLE,	 // [(0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (2, 0), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(1, 0), (0, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(2, 0), (0, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (0, 1), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 1), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(2, 0), (2, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (2, 1), (0, 2), (1, 2)]
	cspc::SATISFIABLE,	 // [(0, 1), (2, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (0, 1), (2, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(2, 0), (0, 1), (2, 1), (0, 2), (1, 2)]
	cspc::UNSATISFIABLE, // [(1, 0), (2, 0), (0, 1), (2, 1), (0, 2), (1, 2)]
};

constexpr auto EXPECTED_TERNARY_ON_DOMAIN_2_SATISFIABILITY = std::array<cspc::satisfiability, 63>{
	cspc::SATISFIABLE,	 // [(1, 0, 0)]
	cspc::SATISFIABLE,	 // [(0, 1, 0)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0)]
	cspc::SATISFIABLE,	 // [(1, 1, 0)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (1, 1, 0)]
	cspc::SATISFIABLE,	 // [(0, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (0, 0, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (0, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 1, 0), (0, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0), (0, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0), (0, 0, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (1, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(0, 1, 0), (1, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(1, 0, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::SATISFIABLE,	 // [(0, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(0, 1, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
	cspc::UNSATISFIABLE, // [(1, 0, 0), (0, 1, 0), (1, 1, 0), (0, 0, 1), (1, 0, 1), (0, 1, 1)]
};

template <typename Encoding>
auto solve_with_encoding(std::vector<cspc::relation> const& relations, Encoding encoding)
	-> std::vector<cspc::satisfiability> {
	auto satisfiable = std::vector<cspc::satisfiability>{};
	satisfiable.reserve(relations.size());
	std::ranges::transform(relations, std::back_inserter(satisfiable), [&](auto const& relation) {
		const auto csp = cspc::to_preserves_operation_csp(cspc::siggers_operation(), relation);
		const auto sat = encoding(csp);
		return cspc::kissat_is_satisfiable(sat);
	});
	return satisfiable;
}

template <typename Encoding>
auto test_encoding_full(std::string const& name, Encoding encoding) -> TestBundle {
	return TestBundle{
		name,
		{
			[encoding]() {
				return test_eq(
					solve_with_encoding(cspc::all_nary_relations(2, 2), encoding),
					gautil::array_to_vector(EXPECTED_BINARY_ON_DOMAIN_2_SATISFIABILITY));
			},
			[encoding]() {
				return test_eq(
					solve_with_encoding(cspc::all_nary_relations(2, 3), encoding),
					gautil::array_to_vector(EXPECTED_BINARY_ON_DOMAIN_3_SATISFIABILITY));
			},
			[encoding]() {
				return test_eq(
					solve_with_encoding(cspc::all_nary_relations(3, 2), encoding),
					gautil::array_to_vector(EXPECTED_TERNARY_ON_DOMAIN_2_SATISFIABILITY));
			},
		}};
}

template <typename Encoding>
auto test_encoding_simple(std::string const& name, Encoding encoding) -> TestBundle {
	return TestBundle{
		name,
		{
			[encoding]() {
				const auto constraints = std::vector<cspc::constraint>{
					cspc::constraint(cspc::eq_relation(2, 2), {0, 1}),
					cspc::constraint(cspc::eq_relation(2, 2), {1, 2}),
				};
				const auto csp = cspc::csp(constraints);
				const auto sat = encoding(csp);
				return test_eq(cspc::kissat_is_satisfiable(sat), cspc::SATISFIABLE);
			},
			[encoding]() {
				const auto constraints = std::vector<cspc::constraint>{
					cspc::constraint(cspc::eq_relation(3, 2), {0, 1, 2}),
					cspc::constraint(cspc::neq_relation(3, 2), {0, 1, 2}),
				};
				const auto csp = cspc::csp(constraints);
				const auto sat = encoding(csp);
				return test_eq(cspc::kissat_is_satisfiable(sat), cspc::UNSATISFIABLE);
			},
			[encoding]() {
				const auto constraints = std::vector<cspc::constraint>{
					cspc::constraint(cspc::eq_relation(2, 2), {0, 1}),
					cspc::constraint(cspc::eq_relation(2, 2), {1, 2}),
					cspc::constraint(cspc::neq_relation(2, 2), {0, 2}),
				};
				const auto csp = cspc::csp(constraints);
				const auto sat = encoding(csp);
				return test_eq(cspc::kissat_is_satisfiable(sat), cspc::UNSATISFIABLE);
			},
		},
	};
}

const TestModule test_encodings = {
	"test encodings",
	{
		test_encoding_simple("test direct encoding simple", cspc::direct_encoding),
		test_encoding_simple(
			"test multivalued direct encoding simple", cspc::multivalued_direct_encoding),
		test_encoding_simple("test binary encoding simple", cspc::binary_encoding),
		test_encoding_simple("test log encoding simple", cspc::log_encoding),
		test_encoding_simple("test label cover encoding simple", cspc::label_cover_encoding),
		test_encoding_simple(
			"test multivalued label cover encoding", cspc::multivalued_label_cover_encoding),
		test_encoding_full("test direct encoding pipeline", cspc::direct_encoding),
		test_encoding_full(
			"test multivalued direct encoding pipeline", cspc::multivalued_direct_encoding),
		test_encoding_full("test binary encoding pipeline", cspc::binary_encoding),
		test_encoding_full("test log encoding pipeline", cspc::log_encoding),
		test_encoding_full("test label cover encoding pipeline", cspc::label_cover_encoding),
		test_encoding_full(
			"test multivalued label cover encoding pipeline",
			cspc::multivalued_label_cover_encoding),
	}};
