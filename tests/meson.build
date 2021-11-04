templates = [
    'ascii85_input_test',
    'ascii85_output_test',
    'asciihex_input_test',
    'asciihex_output_test',
    'comment',
    'eol',
    'lit',
    'lookahead',
    'name',
    'numeric',
    'obj',
    'ref',
    'skip',
    'string',
    'xref' ]

foreach template : templates
    exe = executable(
        template + '_exe', template + '.cc',
        include_directories : includes,
        link_with : libypdf,
        dependencies : [ boost_deps, libturbojpeg_dep, libjpeg_dep ])
    test(template + '_test', exe)
endforeach
