#include "cspc/kissat.hpp"

#include "cspc/data_structures.hpp"
#include <memory>
#include <spdlog/spdlog.h>

namespace solvers {
auto solve_kissat(SAT const& sat) -> Satisfiability {
	auto solver = std::unique_ptr<kissat, void (*)(kissat*)>(kissat_init(), kissat_release);
	for (auto const& clause : sat.clauses()) {
		for (auto const& lit : clause) {
			kissat_add(solver.get(), lit.variable);
		}
		kissat_add(solver.get(), 0); // clauses are terminated with a 0
	}
	const auto result = kissat_solve(solver.get());
	if (result == 10) {
		return SATISFIABLE;
	} else if (result == 20) {
		return UNSATISFIABLE;
	}
	spdlog::critical("Unexpected KISSAT response encountered");
	exit(EXIT_FAILURE);
}
} // namespace solvers