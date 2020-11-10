// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_STRING_HH
#define YPDF_PARSER_STRING_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator >
bool parenthesized_string (Iterator, Iterator &, Iterator, ast::string_t&);

template< typename Iterator >
bool angular_string (Iterator, Iterator &, Iterator, ast::string_t&);

template< typename Iterator >
bool string_ (Iterator, Iterator &, Iterator, ast::string_t&);

} // ypdf::parser

#include <ypdf/parser/string.cc>

#endif // YPDF_PARSER_STRING_HH
