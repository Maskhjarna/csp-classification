#include "gautil/functional.hpp"

namespace gautil {
auto repeat(size_t n, std::function<void()> fn) -> void {
	while (n--) {
		fn();
	}
}
} // namespace gautil
