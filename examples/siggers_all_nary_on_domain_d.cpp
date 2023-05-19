#include "common.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>

auto main(int argc, char* argv[]) -> int {
	if (argc != 3) {
		spdlog::error("Incorrect number of arguments: expected arity and domain size");
		return EXIT_FAILURE;
	}
	// throws
	const auto n = std::stoi(argv[1]);
	const auto d = std::stoi(argv[2]);

	if (n < 2 || d < 2) {
		spdlog::error("Arity and domain must be >1");
		return EXIT_SUCCESS;
	}

	check_all_nary_on_domain(n, d, encoding_siggers_checker());
	return EXIT_SUCCESS;
}
