#include "common.hpp"
#include "cspc/minizinc.hpp"
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>
#include <spdlog/spdlog.h>

auto main() -> int {
	const auto siggers = cspc::siggers_operation();
	const auto neq = cspc::neq_relation(3, 3);
	const auto csp = cspc::to_preserves_operation_csp(siggers, neq);
	const auto minizinc_code = cspc::csp_to_minizinc(csp);
	spdlog::info("\n{}", minizinc_code);
	return EXIT_SUCCESS;
}
