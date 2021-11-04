project('ypdf', 'cpp', version : '0.0.1', license : 'GPL',
        default_options : [ 'cpp_std=c++20' ])

includes = include_directories('include')

boost_deps = dependency('boost', modules : [
    'filesystem', 'iostreams', 'system', 'program_options', 'unit_test_framework' ])

libjpeg_dep = dependency('libjpeg')
libturbojpeg_dep = dependency('libturbojpeg')

subdir('include')
subdir('src')
subdir('examples')
subdir('tests')

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : libypdf,
                 version : '0.0.1',
                 name : 'libypdf',
                 filebase : 'ypdf',
                 description : 'Y another PDF library.')
