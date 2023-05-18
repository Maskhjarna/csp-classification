project("cspc_profiling")

find_package(Threads REQUIRED)
if (NOT APPLE AND NOT WIN32 AND UNIX)
	find_package(TBB REQUIRED COMPONENTS tbb)
endif()

add_library(cspc_profiler_base INTERFACE)
target_link_libraries(cspc_profiler_base INTERFACE
	Threads::Threads
	cspc)

if (NOT APPLE AND NOT WIN32 AND UNIX AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	target_link_libraries(cspc_profiler_base INTERFACE tbb)
endif()

function(add_cspc_profiler name files)
	add_executable(cspc_profiler_${name} ${files})
	target_link_libraries(cspc_profiler_${name} PRIVATE cspc_profiler_base)
endfunction()

add_cspc_profiler (
	compare_encodings
	"compare_encodings.cpp")
