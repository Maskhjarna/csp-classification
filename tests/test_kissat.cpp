#include "test_kissat.hpp"

#include <cspc/kissat.hpp>

namespace {
const auto test_kissat_simple = TestBundle{
	"test kissat",
	{
		[]() {
			return test_eq(
				cspc::kissat_is_satisfiable(cspc::sat{
					cspc::clause{cspc::literal{0, cspc::REGULAR}},
				}),
				cspc::SATISFIABLE);
		},
		[]() {
			return test_eq(
				cspc::kissat_is_satisfiable(cspc::sat{
					cspc::clause{cspc::literal{0, cspc::REGULAR}, cspc::literal{1, cspc::REGULAR}},
				}),
				cspc::SATISFIABLE);
		},
		[]() {
			return test_eq(
				cspc::kissat_is_satisfiable(cspc::sat{
					cspc::clause{cspc::literal{1, cspc::REGULAR}},
					cspc::clause{cspc::literal{1, cspc::NEGATED}},
				}),
				cspc::UNSATISFIABLE);
		},
		[]() {
			return test_eq(
				cspc::kissat_is_satisfiable(cspc::sat{
					cspc::clause{cspc::literal{0, cspc::NEGATED}, cspc::literal{1, cspc::REGULAR}},
					cspc::clause{cspc::literal{1, cspc::NEGATED}, cspc::literal{2, cspc::REGULAR}},
					cspc::clause{cspc::literal{2, cspc::NEGATED}, cspc::literal{0, cspc::REGULAR}},
				}),
				cspc::SATISFIABLE);
		},
		[]() {
			return test_eq(
				cspc::kissat_is_satisfiable(cspc::sat{
					cspc::clause{cspc::literal{0, cspc::REGULAR}, cspc::literal{1, cspc::REGULAR}},
					cspc::clause{cspc::literal{1, cspc::NEGATED}, cspc::literal{0, cspc::REGULAR}},
					cspc::clause{cspc::literal{0, cspc::NEGATED}},
				}),
				cspc::UNSATISFIABLE);
		},
	},
};
}

const TestModule test_kissat = {
	.description = "Kissat tests",
	.tests =
		{
			test_kissat_simple,
		},
};
