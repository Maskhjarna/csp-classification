cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(
  external_gautil
  LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(external_gautil
  "include/gautil/formatters.hpp"
  "include/gautil/functional.hpp"
  "include/gautil/math.hpp"
  "include/gautil/system.hpp"
  "include/gautil/types.hpp"

	"src/math.cpp"
	"src/functional.cpp"
	"src/system.cpp")
target_compile_features(external_gautil PUBLIC cxx_std_20)
target_compile_options(external_gautil PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>)
target_include_directories(external_gautil PUBLIC
  "include/"
)
target_link_libraries(external_gautil
	external_spdlog
)

