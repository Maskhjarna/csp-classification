#pragma once

#include <optional>
#include <string>

namespace gautil {
extern auto call(std::string const& command, std::string const& input = "")
	-> std::optional<std::string>;
}
