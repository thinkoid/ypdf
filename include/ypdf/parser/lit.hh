// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_LIT_HH
#define YPDF_PARSER_LIT_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator >
bool lit(Iterator, Iterator &, Iterator, const std::string &);

template< typename Iterator > bool lit(Iterator, Iterator &, Iterator, char);

} // ypdf::parser

#include <ypdf/parser/lit.cc>

#endif // YPDF_PARSER_LIT_HH
