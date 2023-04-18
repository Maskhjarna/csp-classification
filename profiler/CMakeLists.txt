project("cspc_profiling")

add_library(cspc_profiler_base INTERFACE)
target_link_libraries(cspc_profiler_base INTERFACE
	cspc)

function(add_cspc_profiler name files)
	add_executable(cspc_profiler_${name} ${files})
	target_link_libraries(cspc_profiler_${name} PRIVATE cspc_profiler_base)
endfunction()

add_cspc_profiler (
	compare_encodings
	"compare_encodings.cpp")
