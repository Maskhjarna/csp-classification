#include "test_polymorphisms.hpp"

#include <cspc/algorithms.hpp>
#include <cspc/formatters.hpp>

namespace {
const auto test_index_to_function_input = TestSingle{
	"index to function input",
	[]() {
		return test_eq(cspc::__internal::index_to_function_input(44, 4, 3), {1, 1, 2, 2});
	},
};

const auto test_function_input_to_index = TestSingle{
	"function input to index",
	[]() {
		return test_eq(cspc::__internal::function_input_to_index({1, 1, 2, 2}, 3), DomainValue(44));
	},
};

const auto test_satisfies_identity_rare = TestBundle{
	"satisfies identity rare",
	{
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 0, 0, 0}, {0, 1, 0, 2}), true);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 1, 0, 1}, {0, 1, 0, 2}), true);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 0, 0, 5}, {0, 1, 0, 2}), true);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({5, 2, 5, 3}, {0, 1, 0, 2}), true);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 0, 1, 0}, {0, 1, 0, 2}), false);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 1, 1, 0}, {0, 1, 0, 2}), false);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({0, 1, 1, 1}, {0, 1, 0, 2}), false);
		},
		[]() {
			return test_eq(cspc::__internal::satisfies_identity({5, 1, 2, 3}, {0, 1, 0, 2}), false);
		},
	},
};

const auto test_apply_identity_rare = TestBundle{
	"apply identity rare",
	{
		[]() {
			return test_eq(
				cspc::__internal::apply_identity({0, 0, 0, 0}, {0, 1, 0, 2}, {1, 0, 2, 1}),
				{0, 0, 0, 0});
		},
		[]() {
			return test_eq(
				cspc::__internal::apply_identity({0, 1, 0, 1}, {0, 1, 0, 2}, {1, 0, 2, 1}),
				{1, 0, 1, 1});
		},
		[]() {
			return test_eq(
				cspc::__internal::apply_identity({0, 0, 0, 5}, {0, 1, 0, 2}, {1, 0, 2, 1}),
				{0, 0, 5, 0});
		},
		[]() {
			return test_eq(
				cspc::__internal::apply_identity({5, 2, 5, 3}, {0, 1, 0, 2}, {1, 0, 2, 1}),
				{2, 5, 3, 2});
		},
	},
};
} // namespace

const TestModule test_polymorphisms = {
	.description = "polymorphism tests",
	.tests =
		{
			test_index_to_function_input,
			test_function_input_to_index,
			test_satisfies_identity_rare,
			test_apply_identity_rare,
		},
};