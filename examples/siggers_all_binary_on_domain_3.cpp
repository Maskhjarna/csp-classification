#include "common.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/binary.hpp>

auto main() -> int {
	siggers_all_nary_on_domain_sequential(2, 3, cspc::log_encoding);
	return EXIT_SUCCESS;
}