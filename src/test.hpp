#include "data_structures.hpp"
#include "overload.hpp"
#include "polymorphisms.hpp"
#include "src/kissat.hpp"
#include "util.hpp"
#include <fmt/core.h>
#include <variant>

struct TestSuccess {};
struct TestFailure {
	std::string message;
};
using TestResult = std::variant<TestSuccess, TestFailure>;
struct Test {
	std::string description;
	std::function<TestResult(void)> run;
};
extern auto run_tests() -> bool;
extern auto print_test_result(TestResult const& result) -> void;

template <typename T> extern auto test_eq(T const& lhs, T const& rhs) -> TestResult {
	static_assert(fmt::has_formatter<T, fmt::format_context>());
	if (lhs == rhs) {
		return TestSuccess{};
	}
	return TestFailure{fmt::format("Assertion failed {} != {}", lhs, rhs)};
}
// clang-format off
const auto tests = std::vector<Test>{
	{	
		"index to function input",
		[]() {
			const auto lhs = poly::index_to_function_input(44, 4, 3);
			const auto rhs = RuntimeArray<size_t>({1, 1, 2, 2});
			return test_eq(lhs, rhs);
		}
	}, {	
		"function input to index",
		[]() {
			const auto lhs = poly::function_input_to_index({1, 1, 2, 2}, 3);
			const auto rhs = size_t(44);
			return test_eq(lhs, rhs);
		}
	}, {	
		"satisfies identity rare",
		[]() {
			const auto rare = std::vector<size_t>{0, 1, 0, 2};
			const auto satisfies_rare = [&rare](RuntimeArray<size_t> const& input) {
				return poly::satisfies_identity(input, rare);
			};
			const auto should_satisfy = std::vector<RuntimeArray<size_t>>{
				{0, 0, 0, 0},
				{0, 1, 0, 1},
				{0, 0, 0, 5},
				{5, 2, 5, 3},
			};
			const auto should_not_satisfy = std::vector<RuntimeArray<size_t>>{
				{0, 0, 1, 0},
				{0, 1, 1, 0},
				{0, 1, 1, 1},
				{5, 1, 2, 3},
			};
			for (auto input : should_satisfy) {
				if (!satisfies_rare(input)) {
					return (TestResult)TestFailure{
						fmt::format("{} should satisfy identity RARE but does not", input)};
				}
			}
			for (auto input : should_not_satisfy) {
				if (satisfies_rare(input)) {
					return (TestResult)TestFailure{
						fmt::format("{} should not satisfy identity RARE but does ", input)};
				}
			}
			return (TestResult)TestSuccess{};
		 }
	}, {	
		"apply identity rare",
		[]() {
			const auto rare = std::vector<size_t>{0, 1, 0, 2};
			const auto area = std::vector<size_t>{1, 0, 2, 1};
			const auto inputs = std::vector<RuntimeArray<size_t>>{
				{0, 0, 0, 0},
				{0, 1, 0, 1},
				{0, 0, 0, 5},
				{5, 2, 5, 3},
			};
			const auto expected_mirror = std::vector<RuntimeArray<size_t>>{
				{0, 0, 0, 0},
				{1, 0, 1, 1},
				{0, 0, 5, 0},
				{2, 5, 3, 2},
			};
			for (auto i = 0u; i < inputs.size(); ++i) {
				const auto mirror = poly::apply_identity(inputs[i], rare, area);
				const auto result = test_eq(mirror, expected_mirror[i]);
				if (!std::holds_alternative<TestSuccess>(result)) {
					return result;
				}
			}
			return (TestResult)TestSuccess{};
		 }
	}, {
		"test kissat",
		[]() {
			const auto sats = std::vector<SAT>{
				SAT {
					Clause{Literal{0, NON_NEGATED}},
				},
				SAT {
					Clause{Literal{0, NON_NEGATED}, Literal{1, NON_NEGATED}},
				},
				SAT {
					Clause{Literal{1, NON_NEGATED}},
					Clause{Literal{1, NEGATED}},
				},
				SAT{
					Clause{Literal{0, NEGATED}, Literal{1, NON_NEGATED}},
					Clause{Literal{1, NEGATED}, Literal{2, NON_NEGATED}},
					Clause{Literal{2, NEGATED}, Literal{0, NON_NEGATED}},
				},
				SAT{
					Clause{Literal{0, NON_NEGATED}, Literal{1, NON_NEGATED}},
					Clause{Literal{1, NEGATED}, Literal{0, NON_NEGATED}},
					Clause{Literal{0, NEGATED}},
				},
			};
			const auto expected = std::vector<Satisfiability>{
				SATISFIABLE,
				SATISFIABLE,
				UNSATISFIABLE,
				SATISFIABLE,
				UNSATISFIABLE,
			};

			for (auto i = 0u; i < sats.size(); ++i) {
				const auto result = test_eq(solvers::solve_kissat(sats[i]), expected[i]);
				if (!std::holds_alternative<TestSuccess>(result)) {
					return result;
				}
			}
			return (TestResult)TestSuccess{};
		}
	}
};
// clang-format on
