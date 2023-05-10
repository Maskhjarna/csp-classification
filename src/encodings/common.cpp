#include "cspc/encodings/common.hpp"

namespace cspc {
auto create_encoding_solver(operation const& _operation, encoding _encoding, solver _solver)
	-> polymorphism_checker {
	return [_operation, _encoding, _solver](cspc::relation const& relation) {
		const auto csp = to_preserves_operation_csp(_operation, relation);
		const auto sat = _encoding(csp);
		return _solver(sat);
	};
}
} // namespace cspc
