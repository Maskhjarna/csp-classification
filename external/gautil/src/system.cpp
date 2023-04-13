#include "gautil/system.hpp"

#include <array>
#include <memory>
#include <spdlog/spdlog.h>
#include <string.h>

namespace gautil {
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
} // namespace gautil
