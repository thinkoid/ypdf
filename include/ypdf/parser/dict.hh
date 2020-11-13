// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_DICT_HH
#define YPDF_PARSER_DICT_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast.hh>

#include <string>
#include <tuple>

namespace ypdf::parser {

namespace ast {
struct obj_t;
} // namespace ast

template< typename Iterator >
bool definition(Iterator, Iterator &, Iterator,
                std::tuple< std::string, ast::obj_t > &);

template< typename Iterator >
bool dictionary(Iterator, Iterator &, Iterator, ast::dict_t &);

} // ypdf::parser

#include <ypdf/parser/dict.cc>

#endif // YPDF_PARSER_DICT_HH
