// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/skip.hh>
#include <ypdf/parser/character.hh>

namespace ypdf::parser {

template< typename Iterator > bool skipws(Iterator, Iterator &iter, Iterator last)
{
    bool b = false;

    if (iter != last && is_space(*iter)) {
        b = true;
        for (++iter; iter != last && is_space(*iter); ++iter)
            ;
    }

    return b;
}

} // ypdf::parser
