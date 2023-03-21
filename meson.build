project('csp-complexity', 'cpp')

deps = [
  dependency('kissat', fallback : ['kissat', 'kissat_dep']),
  dependency('spdlog'),
  dependency('fmt'),
  dependency('threads'),
  dependency('tbb'),
]
executable('csp-complexity',
  [
    'src/kissat.cpp',
    'src/main.cpp',
    'src/minizinc.cpp',
    'src/polymorphisms.cpp',
    'src/test.cpp',
    'src/util.cpp',
  ],
  dependencies : deps,
  override_options : [
    'cpp_std=c++20'
  ],
  install : true)