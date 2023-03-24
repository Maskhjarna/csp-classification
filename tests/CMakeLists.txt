cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project("libcspc_tests")

add_executable(cspc_tests
  "main.cpp"
  "test.cpp"
  "test_kissat.cpp"
  "test_polymorphisms.cpp"
)
target_link_libraries(cspc_tests
  libcspc
)
add_test(NAME cspc_tests COMMAND cspc_tests)
