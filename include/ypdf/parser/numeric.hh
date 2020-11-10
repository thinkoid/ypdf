// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_NUMERIC_HH
#define YPDF_PARSER_NUMERIC_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator >
bool bool_ (Iterator, Iterator &, Iterator, bool&);

template< typename Iterator >
bool digit (Iterator, Iterator &, Iterator, int&);

template< typename Iterator, typename T >
typename std::enable_if< std::is_integral_v< T >, bool >::type
int_ (Iterator, Iterator &, Iterator, T&);

template< typename Iterator >
bool double_ (Iterator, Iterator &, Iterator, double&);

template< typename Iterator, typename T, typename U >
typename std::enable_if<
    std::is_integral_v< T > &&
    std::is_integral_v< U >, bool >::type
ints (Iterator, Iterator &, Iterator, T&, U&);

} // ypdf::parser

#include <ypdf/parser/numeric.cc>

#endif // YPDF_PARSER_NUMERIC_HH
