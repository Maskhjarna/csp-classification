#include "common.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>

auto main() -> int {
	check_all_nary_on_domain(2, 3, minizinc_siggers_checker());
	return EXIT_SUCCESS;
}
