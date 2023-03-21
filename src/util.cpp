#include "util.hpp"

#include <functional>
#include <ranges>

namespace util {
constexpr auto comptime_pow(u64 base, u64 exponent) -> u64 {
	if (exponent == 0) {
		return 1;
	}
	return base * comptime_pow(base, exponent - 1);
}

auto n_choose_k(u64 n, u64 k) -> u64 {
	if (k == 0) {
		return 1;
	}
	return (n * n_choose_k(n - 1, k - 1)) / k;
}

auto call(std::string const& command, std::string const& input) -> std::optional<std::string> {
	const auto process =
		std::unique_ptr<FILE, decltype(&pclose)>(popen(command.data(), "r"), pclose);
	if (!process) {
		spdlog::error("Failed to call popen(...) with error: {}", strerror(errno));
		return std::nullopt;
	}
	if (input.size() > 0) {
		fwrite(input.data(), sizeof(char), input.size(), process.get());
	}
	auto result = std::string{};
	for (auto buffer = std::array<char, 128>{};
		 fread(buffer.data(), sizeof(char), buffer.size(), process.get());) {
		result += buffer.data();
	}
	return result;
}

// static constexpr auto remove_nullopts =
// 	std::views::filter([](auto const& maybe_csp) -> bool { return maybe_csp.has_value(); }) |
// 	std::views::transform([](auto const& maybe_csp) { return maybe_csp.value(); });

// template <typename T> auto invoke(std::function<void(T const&)> fn) {
// 	return std::views::filter([&](T const& t) {
// 		fn(t);
// 		return true;
// 	});
// }
} // namespace util