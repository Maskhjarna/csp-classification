#pragma once

#include "types.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <optional>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace gautil {
extern auto repeat(size_t n, std::function<void()> fn) -> void;

template <std::ranges::input_range InputRange>
auto all_subsets(InputRange&& in)
	-> std::vector<std::vector<std::ranges::range_value_t<InputRange>>> {
	using T = std::ranges::range_value_t<InputRange>;
	auto result = std::vector<std::vector<T>>{std::vector<T>{}};
	result.reserve(std::pow(2, in.size()));
	std::ranges::for_each(in, [&](T const& t) {
		std::ranges::for_each(result, [&](std::vector<T> const& v) {
			auto n = std::vector<T>{};
			n.reserve(v.size() + 1);
			std::ranges::copy(v, std::back_inserter(n));
			n.push_back(t);
			result.push_back(std::move(n));
		});
	});
	return result;
}

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
	constexpr auto PROGRESS_BAR_LENGTH = uint32_t{20};
	const auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
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

template <
	typename T,
	std::ranges::input_range Range,
	std::copy_constructible BinaryOperator,
	typename Proj = std::identity>
auto fold(Range&& r, T initial, BinaryOperator op = {}, Proj proj = {}) -> T {
	for (auto const& val : r) {
		initial = op(initial, std::invoke(proj, val));
	}
	return initial;
}

} // namespace gautil