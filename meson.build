project('ypdf', 'cpp', version : '0.0.1', license : 'GPL',
        default_options : ['cpp_std=c++20', 'build.cpp_std=c++20'])

boost_dep = dependency('boost', modules : [
    'filesystem',
    'iostreams',
    'program_options',
    'system'
])

test_dep = dependency('boost', modules : ['unit_test_framework'])

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('examples')
subdir('tests')

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : ypdflib,
                 version : '0.0.1',
                 name : 'libypdf',
                 filebase : 'ypdf',
                 description : 'A PDF library.')
