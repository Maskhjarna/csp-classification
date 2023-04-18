#include "common.hpp"
#include "cspc/encodings/direct.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/binary.hpp>
#include <cspc/encodings/direct.hpp>

auto main() -> int {
	siggers_all_nary_on_domain_sequential(2, 3, cspc::multivalued_direct_encoding);
	return EXIT_SUCCESS;
}