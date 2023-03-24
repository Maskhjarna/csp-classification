#include "test_kissat.hpp"
#include "test_polymorphisms.hpp"
#include <algorithm>
#include <numeric>
#include <ranges>
#include <spdlog/spdlog.h>

auto run_and_print(Test const& test) -> TestResult {
	const auto result = run_test(test);
	std::visit(
		Overload{
			[&](TestSuccess const& t) { spdlog::info("✓ PASSED {}", get_description(test)); },
			[&](TestFailure const& t) {
				spdlog::info(
					"✗ FAILED {}\n\t==> {}", get_description(test),
					fmt::join(t.messages, "\n\t==> "));
			}},
		result);
	return result;
}

auto main() -> int {
	// with std::views this whole thing would be a lot cleaner
	spdlog::info("Concatenating all tests...");
	const auto all_test_modules = std::vector<TestModule>{
		test_polymorphisms,
		test_kissat,
	};
	const auto n_tests = std::transform_reduce(
		all_test_modules.begin(), all_test_modules.end(), 0, std::plus{},
		[](auto a) { return a.tests.size(); });

	auto all_tests = std::vector<Test>();
	all_tests.reserve(n_tests);
	std::ranges::for_each(all_test_modules, [&](TestModule const& module) {
		all_tests.insert(all_tests.end(), module.tests.begin(), module.tests.end());
	});

	spdlog::info("Running tests...");
	auto results = std::vector<TestResult>(n_tests);
	std::ranges::transform(all_tests, results.begin(), run_and_print);

	const auto n_failures = std::ranges::count_if(results, [](TestResult const& result) {
		return std::holds_alternative<TestFailure>(result);
	});

	if (n_failures > 0) {
		spdlog::error("{} test(s) failed", n_failures);
	} else {
		spdlog::info("All {} tests from {} modules passed", n_tests, all_test_modules.size());
	}
	return EXIT_SUCCESS;
}