// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_COMMENT_HH
#define YPDF_PARSER_COMMENT_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/lit.hh>

namespace ypdf::parser {

template< typename Iterator > bool comment(Iterator, Iterator &, Iterator);

template< typename Iterator >
inline bool eof(Iterator first, Iterator &iter, Iterator last)
{
    return lit(first, iter, last, "%%EOF");
}

template< typename Iterator >
bool version(Iterator, Iterator &, Iterator, std::tuple< int, int > &);

} // ypdf::parser

#include <ypdf/parser/comment.cc>

#endif // YPDF_PARSER_COMMENT_HH
