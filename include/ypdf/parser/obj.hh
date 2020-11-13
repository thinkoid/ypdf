// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_OBJ_HH
#define YPDF_PARSER_OBJ_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast.hh>

namespace ypdf::parser {

template< typename Iterator >
bool obj(Iterator, Iterator &, Iterator, ast::obj_t &);

} // ypdf::parser

#include <ypdf/parser/obj.cc>

#endif // YPDF_PARSER_OBJ_HH
