#include "test_kissat.hpp"

#include "kissat.hpp"
#include "util.hpp"

namespace {
const auto test_kissat_simple = TestBundle{
	"test kissat",
	{
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, NON_NEGATED}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, NON_NEGATED}, Literal{1, NON_NEGATED}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{1, NON_NEGATED}},
				Clause{Literal{1, NEGATED}},
			}),
			UNSATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, NEGATED}, Literal{1, NON_NEGATED}},
				Clause{Literal{1, NEGATED}, Literal{2, NON_NEGATED}},
				Clause{Literal{2, NEGATED}, Literal{0, NON_NEGATED}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, NON_NEGATED}, Literal{1, NON_NEGATED}},
				Clause{Literal{1, NEGATED}, Literal{0, NON_NEGATED}},
				Clause{Literal{0, NEGATED}},
			}),
			UNSATISFIABLE),
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
