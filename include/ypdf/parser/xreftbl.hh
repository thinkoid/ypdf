// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_XREFTBL_HH
#define YPDF_PARSER_XREFTBL_HH

#include <ypdf/detail/defs.hh>

#include <vector>

namespace ypdf::parser {

template< typename Iterator >
bool xreftbl(Iterator, Iterator &, Iterator, std::vector< ast::xref_t > &);

} // ypdf::parser

#include <ypdf/parser/xreftbl.cc>

#endif // YPDF_PARSER_XREFTBL_HH
