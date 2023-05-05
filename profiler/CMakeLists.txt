project("cspc_profiling")

find_package(Threads REQUIRED)
find_package(TBB REQUIRED COMPONENTS tbb)

add_library(cspc_profiler_base INTERFACE)
target_link_libraries(cspc_profiler_base INTERFACE
	Threads::Threads
	tbb
	cspc)

function(add_cspc_profiler name files)
	add_executable(cspc_profiler_${name} ${files})
	target_link_libraries(cspc_profiler_${name} PRIVATE cspc_profiler_base)
endfunction()

add_cspc_profiler (
	compare_encodings
	"compare_encodings.cpp")
