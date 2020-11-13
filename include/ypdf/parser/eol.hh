// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_EOL_HH
#define YPDF_PARSER_EOL_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator > bool eol(Iterator, Iterator &, Iterator);

} // ypdf::parser

#include <ypdf/parser/eol.cc>

#endif // YPDF_PARSER_EOL_HH
