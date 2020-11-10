// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_CHARACTER_HH
#define YPDF_PARSER_CHARACTER_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ctype.hh>

namespace ypdf::parser {

inline bool is_regular (char c) {
    return 0 == ctype_of (c);
}

inline bool is_delimiter (char c) {
    return 1 == ctype_of (c);
}

inline bool is_space (char c) {
    return 2 == ctype_of (c);
}

} // ypdf::parser

#endif // YPDF_PARSER_CHARACTER_HH
