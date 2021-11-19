// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_ARRAY_HH
#define YPDF_PARSER_ARRAY_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/array.hh>

namespace ypdf::parser {

template< typename Iterator >
bool array(Iterator, Iterator &, Iterator, ast::array_t &);

} // ypdf::parser

#include <ypdf/parser/array.cc>

#endif // YPDF_PARSER_ARRAY_HH
