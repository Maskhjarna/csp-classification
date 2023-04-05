cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(
  "libcspc"
  VERSION 0.0
  LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CSPC_BUILD_TESTS "Build tests" ON)
option(CSPC_BUILD_EXAMPLES "Build examples" ON)

add_subdirectory(external)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)

add_library(${PROJECT_NAME}
  "include/cspc/data_structures.hpp"
  "include/cspc/kissat.hpp"
  "include/cspc/minizinc.hpp"
  "include/cspc/polymorphisms.hpp"
  "include/cspc/types.hpp"

  "src/minizinc.cpp"
  "src/kissat.cpp"
  "src/polymorphisms.cpp"
  "src/util.cpp")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -fconcepts-diagnostics-depth=4
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Release>:-O3>)
include_directories(${PROJECT_NAME} 
  "include/cspc"
  "src")
target_link_libraries(${PROJECT_NAME} 
  fmt
  spdlog
  libkissat
  ${CMAKE_THREAD_LIBS_INIT}
)

if (CSPC_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if (CSPC_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()