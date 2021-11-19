// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_XREFSTM_HH
#define YPDF_PARSER_XREFSTM_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/xref.hh>

#include <vector>

namespace ypdf::parser {

template< typename Iterator >
bool xrefstm(Iterator, Iterator &, Iterator, std::vector< ast::xref_t > &);

} // ypdf::parser

#include <ypdf/parser/xrefstm.cc>

#endif // YPDF_PARSER_XREFSTM_HH
