#include "kissat.hpp"

namespace solvers {
auto solve_kissat(SAT const& sat) -> Satisfiability {
	auto solver = std::unique_ptr<kissat, void (*)(kissat*)>(kissat_init(), kissat_release);
	for (auto const& clause : sat.clauses()) {
		for (auto const& lit : clause) {
			const auto sign = lit.negation == NEGATED ? -1 : 1;
			kissat_add(solver.get(), sign * (lit.variable + 1)); // 0 reserved
		}
		kissat_add(solver.get(), 0); // clauses are terminated with a 0
	}
	const auto result = kissat_solve(solver.get());
	assert(result == 10 || result == 20);
	return result == 10 ? SATISFIABLE : UNSATISFIABLE;
}
} // namespace solvers