#include "common.hpp"
#include "cspc/algorithms.hpp"

auto main() -> int {
	siggers_all_nary_on_domain_sequential(3, 3, cspc::multivalued_direct_encoding);
	return EXIT_SUCCESS;
}