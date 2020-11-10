// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_ERROR_HH
#define YPDF_PARSER_ERROR_HH

#include <ypdf/detail/defs.hh>

#include <iostream>

namespace ypdf::parser {

template< typename Iterator >
void expected (Iterator, Iterator, Iterator, Iterator,
               const std::string&, std::ostream& out = std::cerr);

template< typename Iterator >
inline void
expected (Iterator first, Iterator start, Iterator iter, Iterator last,
          char what, std::ostream& out = std::cerr) {
    return expected (first, start, iter, last, std::string (1U, what), out);
}

} // ypdf::parser

#include <ypdf/parser/error.cc>

#endif // YPDF_PARSER_ERROR_HH
