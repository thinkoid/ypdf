ypdf_example = executable(
    'ypdf', ['ypdf.cc', 'options.cc'],
    include_directories : includes,
    link_with : libypdf,
    dependencies : [ boost_deps, libturbojpeg_dep, libjpeg_dep ],
    install : true)
