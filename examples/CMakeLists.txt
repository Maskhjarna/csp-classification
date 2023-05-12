project("cspc_examples")

add_library(cspc_example_base INTERFACE)
target_link_libraries(cspc_example_base INTERFACE
	cspc)

function(add_cspc_example name files)
	add_executable(cspc_example_${name} ${files} "common.cpp")
	target_link_libraries(cspc_example_${name} PRIVATE cspc_example_base)
endfunction()

add_cspc_example(
	siggers_all_binary_on_domain_2
	"siggers_all_binary_on_domain_2.cpp")

add_cspc_example(
	siggers_all_binary_on_domain_3
	"siggers_all_binary_on_domain_3.cpp")

add_cspc_example(
	siggers_all_binary_on_domain_4
	"siggers_all_binary_on_domain_4.cpp")

add_cspc_example(
	siggers_all_ternary_on_domain_2
	"siggers_all_ternary_on_domain_2.cpp")

add_cspc_example(
	siggers_all_ternary_on_domain_3
	"siggers_all_ternary_on_domain_3.cpp")

add_cspc_example(
	siggers_all_quaternary_on_domain_2
	"siggers_all_quaternary_on_domain_2.cpp")

add_cspc_example(
	siggers_all_quaternary_on_domain_3
	"siggers_all_quaternary_on_domain_3.cpp")

add_cspc_example(
	siggers_binary_neq_on_domain_6
	"siggers_binary_neq_on_domain_6.cpp")

add_cspc_example(
	siggers_binary_neq_on_domain_7
	"siggers_binary_neq_on_domain_7.cpp")

add_cspc_example(
	siggers_ternary_neq_on_domain_3
	"siggers_ternary_neq_on_domain_3.cpp")

add_cspc_example(
	siggers_quaternary_neq_on_domain_3
	"siggers_quaternary_neq_on_domain_3.cpp")

add_cspc_example(
	siggers_quinary_neq_on_domain_2
	"siggers_quinary_neq_on_domain_2.cpp")

add_cspc_example(
	siggers_quinary_neq_on_domain_3
	"siggers_quinary_neq_on_domain_3.cpp")

add_cspc_example(
	siggers_senary_neq_on_domain_2
	"siggers_senary_neq_on_domain_2.cpp")

add_cspc_example(
	minizinc_siggers_all_binary_on_domain_2
	"minizinc_siggers_all_binary_on_domain_2.cpp")

add_cspc_example(
	minizinc_code_neq_on_domain_2
	"minizinc_code_neq_on_domain_2.cpp")

add_cspc_example(
	elements_in_relations
	"elements_in_relations.cpp")
