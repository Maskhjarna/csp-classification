#include "test_encodings.hpp"
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
				spdlog::warn(
					"✗ FAILED {}\n\t==> {}", get_description(test),
					fmt::join(t.messages, "\n\t==> "));
			}},
		result);
	return result;
}

auto main() -> int {
	const auto test_modules = std::vector<TestModule>{
		std::move(test_kissat),
		std::move(test_polymorphisms),
		std::move(test_encodings),
	};
	spdlog::info("Running tests...");
	auto results = std::vector<TestResult>{};
	std::ranges::for_each(test_modules, [&](auto const& test_module) {
		spdlog::info("Module {} =>", test_module.description);
		std::ranges::transform(test_module.tests, std::back_inserter(results), run_and_print);
	});

	const auto n_failures = std::ranges::count_if(results, [](TestResult const& result) {
		return std::holds_alternative<TestFailure>(result);
	});

	if (n_failures > 0) {
		spdlog::warn("{} of {} test(s) failed", n_failures, results.size());
	} else {
		spdlog::info("All {} tests from {} module(s) passed", results.size(), test_modules.size());
	}
	return EXIT_SUCCESS;
}