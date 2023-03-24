#pragma once

#include <fmt/core.h>
#include <functional>
#include <variant>

template <class... Ts> struct Overload : Ts... {
	using Ts::operator()...;
};
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

struct TestSuccess {};
struct TestFailure {
	std::vector<std::string> messages;
};
using TestResult = std::variant<TestSuccess, TestFailure>;

struct TestSingle {
	std::string description;
	std::function<TestResult(void)> run;
};
struct TestBundle {
	std::string description;
	std::vector<std::function<TestResult(void)>> tests;
};
using Test = std::variant<TestSingle, TestBundle>;

struct TestModule {
	std::string description;
	std::vector<Test> tests;
};

template <typename T> auto test_eq(T const& lhs, T const& rhs) -> std::function<TestResult(void)> {
	static_assert(fmt::has_formatter<T, fmt::format_context>());
	return [lhs, rhs]() {
		return lhs == rhs ? (TestResult)TestSuccess{}
						  : (TestResult)TestFailure{{fmt::format("{} should be {}", lhs, rhs)}};
	};
}

extern auto run_test(Test const& test) -> TestResult;
extern auto get_description(Test const& test) -> std::string;
