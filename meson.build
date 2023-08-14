project('vixen', 'c', version : '0.0.0')

include_dirs = [ include_directories('include') ]
source_files = [ 'vixen.c' ]

vxn_lib = library(
    'vixen-dev',
    'include/vixen.h',
    include_directories : include_dirs,
    soversion : '0.0.0')
vxn_exe = executable('vixen', source_files, link_with : vxn_lib)

# Tests libvixen-dev.
tst_exe = executable(
    'vixen_test',
    'vixen_test.c',
    include_directories : include_dirs)
test('library test', tst_exe)
