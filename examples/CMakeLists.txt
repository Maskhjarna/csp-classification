project("cspc_examples")

add_library(cspc_example_base INTERFACE)
target_link_libraries(cspc_example_base INTERFACE
	cspc)

function(add_cspc_example name files)
	add_executable(cspc_example_${name} ${files} "common.cpp")
	target_link_libraries(cspc_example_${name} PRIVATE cspc_example_base)
endfunction()

add_cspc_example(
	siggers_all_nary_on_domain_d
	"siggers_all_nary_on_domain_d.cpp")

add_cspc_example(
	siggers_nary_neq_on_domain_d
	"siggers_nary_neq_on_domain_d.cpp")

add_cspc_example(
	minizinc_siggers_all_binary_on_domain_2.cpp
	"minizinc_siggers_all_binary_on_domain_2.cpp")

add_cspc_example(
	minizinc_code_binary_neq_on_domain_2
	"minizinc_code_binary_neq_on_domain_2.cpp")

add_cspc_example(
	elements_in_relations
	"elements_in_relations.cpp")
