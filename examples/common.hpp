#pragma once

#include <algorithm>
#include <ranges>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/spdlog.h>

constexpr auto PROGRESS_BAR_LENGTH = uint32_t{20};
template <
	std::ranges::input_range Range,
	std::weakly_incrementable OutputIterator,
	std::copy_constructible Function,
	class Proj = std::identity>
	requires std::indirectly_writable<
				 OutputIterator,
				 std::indirect_result_t<
					 Function&,
					 std::projected<std::ranges::iterator_t<Range>, Proj>>>
auto transform_and_print_progress(Range&& range, OutputIterator result, Function op, Proj proj = {})
	-> std::ranges::
		unary_transform_result<std::ranges::borrowed_iterator_t<Range>, OutputIterator> {
	// setup logger
	const auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
	auto carriage_return_logger = spdlog::logger("logger", {console_sink});
	carriage_return_logger.set_formatter(std::make_unique<spdlog::pattern_formatter>(
		"[%Y-%m-%d %H:%M:%S.%e] [%l] %v", spdlog::pattern_time_type::local, std::string("\r")));

	const auto distance = std::ranges::distance(range);
	auto input = std::ranges::begin(range);
	carriage_return_logger.info("[{}]", std::string(PROGRESS_BAR_LENGTH, ' '));
	for (auto i = 0u; i < distance; ++i) {
		*result++ = std::invoke(op, std::invoke(proj, *input++));
		const auto filled = float(i) / distance;
		const auto unfilled = float(1.0) - filled;
		carriage_return_logger.info(
			"[{}{}] {:.2f}%", std::string(filled * PROGRESS_BAR_LENGTH, '='),
			std::string(unfilled * PROGRESS_BAR_LENGTH, ' '), filled * 100.0);
	}
	carriage_return_logger.info("[{}] 100.00%\n", std::string(PROGRESS_BAR_LENGTH, '='));

	return {input, result};
}

extern auto siggers_all_nary_on_domain(size_t n, size_t domain_size) -> void;