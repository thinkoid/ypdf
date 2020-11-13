// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_SKIP_HH
#define YPDF_PARSER_SKIP_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator > bool skipws(Iterator, Iterator &, Iterator);

} // ypdf::parser

#include <ypdf/parser/skip.cc>

#define SKIP skipws(first, iter, last)

#endif // YPDF_PARSER_SKIP_HH
