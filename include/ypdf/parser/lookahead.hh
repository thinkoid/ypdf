// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_LOOKAHEAD_HH
#define YPDF_PARSER_LOOKAHEAD_HH

#include <ypdf/detail/defs.hh>

#include <climits>

namespace ypdf::parser {

template< typename Iterator >
inline typename Iterator::value_type lookahead(Iterator &iter, Iterator last)
{
    return (iter == last) ? CHAR_MAX + 1 : *iter;
}

template< typename Iterator >
inline bool lookahead(Iterator &iter, Iterator last,
                      typename Iterator::value_type c)
{
    return c == lookahead(iter, last);
}

template< typename Iterator >
bool lookahead(Iterator, Iterator, const std::string &);

} // ypdf::parser

#include <ypdf/parser/lookahead.cc>

#endif // YPDF_PARSER_LOOKAHEAD_HH
