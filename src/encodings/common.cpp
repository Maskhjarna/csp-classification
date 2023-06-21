#include "cspc/encodings/common.hpp"

namespace cspc {
namespace __internal {
auto nogoods(std::vector<constraint> const& constraints, size_t domain_size)
	-> std::vector<constraint> {
	auto nogoods = std::vector<constraint>{};
	nogoods.reserve(constraints.size());
	std::ranges::transform(
		constraints, std::back_inserter(nogoods),
		[&](constraint const& constraint) { return inverse(constraint, domain_size); });
	return nogoods;
}
} // namespace __internal
auto create_encoding_solver(operation const& _operation, encoding _encoding, solver _solver)
	-> polymorphism_checker {
	return [_operation, _encoding, _solver](cspc::relation const& relation) {
		const auto csp = construct_preserves_operation_csp(_operation, relation);
		const auto sat = _encoding(csp);
		return _solver(sat);
	};
}
} // namespace cspc
