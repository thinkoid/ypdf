// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/eol.hh>

namespace ypdf::parser {

template< typename Iterator >
bool eol(Iterator, Iterator &iter, Iterator last) {
    if (iter != last) {
        if (*iter == '\r') {
            if (++iter != last && *iter == '\n')
                ++iter;

            return true;
        }
        else if (*iter == '\n') {
            return ++iter, true;
        }
    }

    return false;
}

} // ypdf::parser
