FetchContent_Declare(spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog
	GIT_TAG ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # 1.11.0
)
FetchContent_MakeAvailable(spdlog)

add_library(external_spdlog INTERFACE)
target_include_directories(external_spdlog SYSTEM INTERFACE)
target_link_libraries(external_spdlog INTERFACE spdlog)
