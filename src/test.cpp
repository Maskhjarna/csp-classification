#include "test.hpp"

#include "overload.hpp"
#include "polymorphisms.hpp"
#include <ranges>

auto print_test_result(std::string_view description, TestResult const& result) -> void {
	// clang-format off
	std::visit(Overload{
		[&](TestSuccess const& t) { spdlog::info("PASSED \t| {}", description); },
		[&](TestFailure const& t) { spdlog::error("FAILED \t| {} | {}", description, t.message); }
	}, result);
	// clang-format on
}

auto run_tests() -> bool {
	auto test_results = std::vector<TestResult>{};
	test_results.reserve(tests.size());
	std::ranges::transform(
		tests.begin(), tests.end(), std::back_inserter(test_results), [](Test const& test) {
			const auto result = test.run();
			print_test_result(test.description, result);
			return result;
		});

	return std::ranges::all_of(test_results, [](TestResult const& result) {
		return std::holds_alternative<TestSuccess>(result);
	});
}