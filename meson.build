project(
    'vixen',
    'c',
    'cpp',
    version : '0.0.0'
)

include_dirs = [ include_directories('include') ]
source_files = [ 'vixen.cpp' ]

# Builds libvixen-dev.
vxn_lib = library(
    'vixen-dev',
    'include/vixen.hpp',
    include_directories : include_dirs,
    soversion : '0.0.0')

# Builds vixen
executable(
    'vixen',
    source_files,
    link_with : vxn_lib,
    override_options : ['c_std=23', 'cpp_std=c++20'])

# Tests libvixen-dev.
tst_include_dirs = include_dirs + [ include_directories('tests') ]
tst_exe = executable(
    'vixen_test',
    'vixen_test.cpp',
    include_directories : tst_include_dirs,
    override_options : ['c_std=23', 'cpp_std=c++20'])
test('library test', tst_exe)
