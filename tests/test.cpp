#include "test.hpp"

#include <algorithm>
#include <fmt/format.h>

auto run_test_bundle(TestBundle const& bundle) -> TestResult {
	auto results = std::vector<TestResult>(bundle.tests.size());
	std::ranges::transform(bundle.tests, results.begin(), [](auto f) { return f(); });
	auto fail_messages = std::vector<std::string>{};
	std::ranges::for_each(results, [&](TestResult const& result) {
		if (std::holds_alternative<TestFailure>(result)) {
			const auto messages = std::get<TestFailure>(result).messages;
			fail_messages.insert(fail_messages.end(), messages.begin(), messages.end());
		}
	});
	const auto fail_message = fmt::format("[{}]", fmt::join(fail_messages, ", "));
	return fail_messages.empty() ? (TestResult)TestSuccess{}
								 : (TestResult)TestFailure{fail_messages};
}

auto run_test(Test const& test) -> TestResult {
	return std::visit(
		Overload{
			[&](TestSingle const& single) { return single.run(); },
			[&](TestBundle const& bundle) { return run_test_bundle(bundle); }},
		test);
}

auto get_description(Test const& test) -> std::string {
	return std::visit(
		Overload{
			[&](TestSingle const& single) { return single.description; },
			[&](TestBundle const& bundle) { return bundle.description; }},
		test);
}
