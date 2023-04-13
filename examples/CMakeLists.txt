project("cspc_examples")

add_library(cspc_example_base INTERFACE)
target_link_libraries(cspc_example_base INTERFACE
	cspc)

function(add_cspc_example name files)
	add_executable(cspc_example_${name} ${files} "common.cpp")
	target_link_libraries(cspc_example_${name} PRIVATE cspc_example_base)
endfunction()

add_cspc_example(
	siggers_all_binary_on_domain_3
	"siggers_all_binary_on_domain_3.cpp")

add_cspc_example(
	siggers_all_binary_on_domain_4
	"siggers_all_binary_on_domain_4.cpp")

add_cspc_example(
	siggers_all_tetriary_on_domain_3
	"siggers_all_tetriary_on_domain_3.cpp")