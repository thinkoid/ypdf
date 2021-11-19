// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_DICT_HH
#define YPDF_PARSER_DICT_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast/dict.hh>
#include <ypdf/parser/ast/obj.hh>

#include <string>
#include <tuple>

namespace ypdf::parser {

template< typename Iterator >
bool definition(Iterator, Iterator &, Iterator,
                std::tuple< std::string, ast::obj_t > &);

template< typename Iterator >
bool dictionary(Iterator, Iterator &, Iterator, ast::dict_t &);

} // ypdf::parser

#include <ypdf/parser/dict.cc>

#endif // YPDF_PARSER_DICT_HH
