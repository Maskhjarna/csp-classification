#include "common.hpp"
#include "cspc/minizinc.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>

auto minizinc_siggers_checker() -> cspc::polymorphism_checker {
	return [&](cspc::relation const& relation) {
		const auto csp =
			cspc::construct_preserves_operation_csp(cspc::siggers_operation(), relation);
		return cspc::minizinc_is_satisfiable(csp).value();
	};
}

auto main(int argc, char* argv[]) -> int {
	check_all_nary_on_domain(2, 2, minizinc_siggers_checker());
	return EXIT_SUCCESS;
}
