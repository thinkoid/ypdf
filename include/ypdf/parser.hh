// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_HH
#define YPDF_PARSER_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast.hh>
#include <ypdf/parser/array.hh>
#include <ypdf/parser/character.hh>
#include <ypdf/parser/comment.hh>
#include <ypdf/parser/dict.hh>
#include <ypdf/parser/eol.hh>
#include <ypdf/parser/error.hh>
#include <ypdf/parser/iobj.hh>
#include <ypdf/parser/lit.hh>
#include <ypdf/parser/lookahead.hh>
#include <ypdf/parser/name.hh>
#include <ypdf/parser/numeric.hh>
#include <ypdf/parser/obj.hh>
#include <ypdf/parser/ref.hh>
#include <ypdf/parser/skip.hh>
#include <ypdf/parser/stream.hh>
#include <ypdf/parser/string.hh>
#include <ypdf/parser/trailer.hh>
#include <ypdf/parser/xref.hh>
#include <ypdf/parser/xrefstm.hh>
#include <ypdf/parser/xreftbl.hh>

namespace ypdf {

template< typename Iterator >
bool parse(Iterator, Iterator &, Iterator,
           std::vector< parser::ast::xref_t > &);

template< typename Iterator >
bool parse(Iterator, Iterator &, Iterator,
           std::vector< parser::ast::iobj_t > &);

} // namespace ypdf

#include <ypdf/parser.cc>

#endif // YPDF_PARSER_HH
