project(
    'vixen',
    'c',
    'cpp',
    version : '0.0.0'
)

# Go ahead and optimize the build if not in
# release. Just ensure to include debug symbols.
if get_option('buildtype') != 'release'
    add_project_arguments('-Ofast', '-g3', language : 'cpp')
else
    add_project_arguments('-Ofast', language: 'cpp')
endif

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

# Exclude tests target from the release build.
if get_option('buildtype') != 'release'
    # Tests libvixen-dev.
    tst_include_dirs = include_dirs + [ include_directories('tests') ]
    tst_exe = executable(
        'vixen_test',
        'vixen_test.cpp',
        include_directories : tst_include_dirs,
        override_options : ['c_std=23', 'cpp_std=c++20'])
    test('library test', tst_exe)
endif
