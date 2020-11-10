// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/lookahead.hh>

namespace ypdf::parser {

template< typename Iterator >
bool lookahead(Iterator iter, Iterator last, const std::string& s) {
    auto other = s.begin();

    for (; iter != last && other != s.end() && *iter == *other;
         ++iter, ++other) ;

    return other == s.end();
}

} // ypdf::parser
