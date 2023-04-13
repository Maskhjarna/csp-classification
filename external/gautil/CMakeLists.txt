cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(
  gautil
  LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_library(gautil
  "include/gautil/formatters.hpp"
  "include/gautil/functional.hpp"
  "include/gautil/math.hpp"
  "include/gautil/system.hpp"
  "include/gautil/types.hpp"

	"src/math.cpp"
	"src/functional.cpp"
	"src/system.cpp")
target_compile_features(gautil PUBLIC cxx_std_20)
target_compile_options(gautil PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>)
target_include_directories(gautil PUBLIC
  "include/"
)
target_link_libraries(gautil
  fmt
	spdlog
)

