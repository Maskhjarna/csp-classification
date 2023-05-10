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
auto encoding_test_bundle(std::string const& name, Encoding encoding) -> TestBundle {
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

const TestModule test_encodings = {
	"test encodings",
	{
		encoding_test_bundle("test direct encoding", cspc::direct_encoding),
		encoding_test_bundle("test multivalued direct encoding", cspc::multivalued_direct_encoding),
		encoding_test_bundle("test binary encoding", cspc::binary_encoding),
		encoding_test_bundle("test log encoding", cspc::log_encoding),
		encoding_test_bundle("test label cover encoding", cspc::label_cover_encoding),
		encoding_test_bundle(
			"test multivalued label cover encoding", cspc::multivalued_label_cover_encoding),
	}};
