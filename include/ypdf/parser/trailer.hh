// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_TRAILER_HH
#define YPDF_PARSER_TRAILER_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/dict.hh>

#include <map>

namespace ypdf::parser {

template< typename Iterator >
bool trailer(Iterator, Iterator &, Iterator, ast::dict_t &);

} // ypdf::parser

#include <ypdf/parser/trailer.cc>

#endif // YPDF_PARSER_TRAILER_HH
