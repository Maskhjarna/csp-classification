#include "test_kissat.hpp"

#include <cspc/kissat.hpp>

namespace {
const auto test_kissat_simple = TestBundle{
	"test kissat",
	{
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, REGULAR}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, REGULAR}, Literal{1, REGULAR}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{1, REGULAR}},
				Clause{Literal{1, NEGATED}},
			}),
			UNSATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, NEGATED}, Literal{1, REGULAR}},
				Clause{Literal{1, NEGATED}, Literal{2, REGULAR}},
				Clause{Literal{2, NEGATED}, Literal{0, REGULAR}},
			}),
			SATISFIABLE),
		test_eq(
			solvers::solve_kissat(SAT{
				Clause{Literal{0, REGULAR}, Literal{1, REGULAR}},
				Clause{Literal{1, NEGATED}, Literal{0, REGULAR}},
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
