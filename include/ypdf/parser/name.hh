// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_NAME_HH
#define YPDF_PARSER_NAME_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator >
bool name(Iterator, Iterator &, Iterator, ast::name_t &);

} // ypdf::parser

#include <ypdf/parser/name.cc>

#endif // YPDF_PARSER_NAME_HH
