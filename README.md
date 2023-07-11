# csp-classification
This repository hosts a C++ library with tools for finding algebraic polymorphisms of constraint languages. A family of operations commonly of interest is _Siggers operations_ defined by the identity `f(r, a, r, e) = f(a, r, e, a)`, since the existance of such a polymorphism implies a CSP on the language is solvable in polynomial time.

## Examples
Examples can be found under `examples/`.
```cpp
#include <cspc/algorithms.hpp>
#include <cspc/encodings/direct.hpp>
#include <cspc/kissat.hpp>

auto main() -> int {
  // create a relation containing every tuple (a, b) where a != b on domain [0. 3)
  const auto neq = cspc::neq_relation(2, 3);

  // create a closure checking for the existance of a siggers operation using the
  // multivalued direct SAT encoding and the kissat SAT solver
  const auto has_siggers_operation =
    cspc::create_encoding_solver(
      cspc::siggers_operation(),
      cspc::multivalued_direct_encoding,
      cspc::kissat_is_satisfiable);

  assert(has_siggers_operation(neq) == cspc::UNSATISFIABLE);

  return EXIT_SUCCESS;
}
```

## Building
```sh
mkdir build
cmake . build/
cmake --build build/
```

## Dependencies
All dependencies are fetched automatically by CMake when building.
* fmt
* spdlog
* kissat
