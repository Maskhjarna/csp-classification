#include "common.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>

auto main() -> int {
	check_single(cspc::neq_relation(4, 2), encoding_siggers_checker());
	return EXIT_SUCCESS;
}

