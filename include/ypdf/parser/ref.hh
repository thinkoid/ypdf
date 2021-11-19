// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_REF_HH
#define YPDF_PARSER_REF_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/ref.hh>

namespace ypdf::parser {

template< typename Iterator >
bool ref(Iterator, Iterator &, Iterator, ast::ref_t &);

} // ypdf::parser

#include <ypdf/parser/ref.cc>

#endif // YPDF_PARSER_REF_HH
