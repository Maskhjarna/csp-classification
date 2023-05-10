#include "common.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>

auto main() -> int {
	check_all_nary_on_domain(3, 3, encoding_siggers_checker());
	return EXIT_SUCCESS;
}
