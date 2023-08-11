project('vixen', 'c', version : '0.0.0')

include_dirs = [ include_directories('include') ]
source_files = [ 'main.c' ]

vxn_lib = library('vixen-dev', 'include/vixen.h', include_directories: include_dirs, soversion: '0.0.0')
vxn_exe = executable('vixen', source_files, link_with : vxn_lib)
