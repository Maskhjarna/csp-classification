#include "cspc/kissat.hpp"

#include "cspc/data_structures.hpp"
#include "gautil/functional.hpp"
#include <memory>
#include <spdlog/spdlog.h>

namespace cspc {
auto kissat_is_satisfiable(sat const& sat) -> satisfiability {
	auto solver = std::unique_ptr<kissat, void (*)(kissat*)>(kissat_init(), kissat_release);
	const auto n_literals = gautil::fold(sat.clauses(), int(0), std::plus{}, &clause::size);
	kissat_reserve(solver.get(), n_literals);
	for (auto const& clause : sat.clauses()) {
		for (auto const& lit : clause) {
			kissat_add(solver.get(), lit.value);
		}
		kissat_add(solver.get(), 0); // clauses are terminated with a 0
	}
	const auto result = kissat_solve(solver.get());
	switch (result) {
	case 10:
		return SATISFIABLE;
	case 20:
		return UNSATISFIABLE;
	default:
		spdlog::critical("Unhandled kissat response");
		exit(EXIT_FAILURE);
	}
}
} // namespace cspc