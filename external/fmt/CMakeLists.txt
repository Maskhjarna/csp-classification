FetchContent_Declare(fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt
	GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc # 8.0.1
)
FetchContent_MakeAvailable(fmt)

add_library(external_fmt INTERFACE)
target_include_directories(external_fmt SYSTEM INTERFACE $<TARGET_PROPERTY:fmt,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(external_fmt INTERFACE fmt)
