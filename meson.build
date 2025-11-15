project(
    'NoHub',
    ['cpp'],
    version: '0.1',
    default_options: [
        'cpp_std=c++23',
        'c_std=c11',
        'warning_level=3',
        'werror=false',
        'b_sanitize=address,undefined,leak',
    ],
)

# --- Common directories ---
core_dir = 'src/core'
#utils_dir = 'src/utils'
#include_dir = 'src/include'

# --- Library sources ---
subdir(core_dir)
#subdir(utils_dir)
sources = core_sources #+ utils_sources
incdir = [include_directories('src')]

# C++ arguments
cxx_args = [
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Wpedantic',
    '-Wshadow',
    '-Wnon-virtual-dtor',
    '-Wold-style-cast',
    '-Wcast-align',
    '-Wunused',
    '-Woverloaded-virtual',
    '-Wconversion',
    '-Wsign-conversion',
    '-Wnull-dereference',
    '-Wdouble-promotion',
    '-Wformat=2',
    '-Wmisleading-indentation',
    '-Wduplicated-cond',
    '-Wduplicated-branches',
    '-Wlogical-op',
    '-Wuseless-cast',
]
cxx_args_release = cxx_args + ['-O3']
cxx_args_debug = cxx_args + ['-g', '-O0']

if get_option('buildtype') == 'release'
    cpp_args = cxx_args_release
else
    cpp_args = cxx_args_debug
endif

# Dependencies
thread_dep = dependency('threads')

# --- Library ---
lib_nohub = static_library('libnohub', sources, include_directories: incdir, dependencies: thread_dep)

# --- Main executable ---
executable(
    'nohub',
    ['src/main.cpp', 'src/program.cpp'],
    cpp_args: cpp_args,
    include_directories: incdir,
    link_with: lib_nohub,
    install: true,
)

# --- Subdirectory for tests ---
# if get_option('enable-tests')
#   subdir('test')
# else
