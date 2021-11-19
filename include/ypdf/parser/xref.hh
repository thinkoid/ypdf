// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_XREF_HH
#define YPDF_PARSER_XREF_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/xref.hh>

#include <vector>

namespace ypdf::parser {

template< typename Iterator > bool startxref(Iterator, Iterator, off_t &);

template< typename Iterator > bool rstartxref(Iterator, Iterator, off_t &);

template< typename Iterator >
bool xrefs(Iterator, Iterator &, Iterator, std::vector< ast::xref_t > &);

template< typename Iterator >
bool xrefs_section(Iterator, Iterator &, Iterator, std::vector< ast::xref_t > &);

} // ypdf::parser

#include <ypdf/parser/xref.cc>

#endif // YPDF_PARSER_XREF_HH
