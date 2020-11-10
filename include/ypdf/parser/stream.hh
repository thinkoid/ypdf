// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_STREAM_HH
#define YPDF_PARSER_STREAM_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator >
bool streambuf_ (Iterator, Iterator &, Iterator, ast::stream_t&);

template< typename Iterator >
bool stream_ (Iterator, Iterator &, Iterator, ast::stream_t&);

} // ypdf::parser

#include <ypdf/parser/stream.cc>

#endif // YPDF_PARSER_STREAM_HH
