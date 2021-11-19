// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_IOBJ_HH
#define YPDF_PARSER_IOBJ_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast/iobj.hh>

namespace ypdf::parser {

template< typename Iterator >
bool iobj(Iterator, Iterator &, Iterator, ast::iobj_t &);

} // ypdf::parser

#include <ypdf/parser/iobj.cc>

#endif // YPDF_PARSER_IOBJ_HH
