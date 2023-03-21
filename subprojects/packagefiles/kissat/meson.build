project('kissat', 'c')

run_command('./configure', '--quiet', '--shared', check : true)
run_command('make', 'build.h', '-C', './build', check : true)
run_command('cp', 'build/build.h', 'src/', check : true)

sources = []
subdir('src')

inc = include_directories('src')

cc = meson.get_compiler('c')
deps = [
  cc.find_library('m', required : false)
]

lib = library('kissat',
  sources,
  include_directories : inc,
  dependencies : deps,
  c_args : '-DQUIET',
  install : true)

kissat_dep = declare_dependency(
  include_directories : inc,
  link_with : lib)