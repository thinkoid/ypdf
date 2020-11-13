// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/ref.hh>
#include <ypdf/parser/numeric.hh>
#include <ypdf/parser/skip.hh>

namespace ypdf::parser {

template< typename Iterator >
bool ref(Iterator first, Iterator &iter, Iterator last, ast::ref_t &attr)
{
    int a = 0, b = 0;

    if (int_(first, iter, last, a) && skipws(first, iter, last) &&
        int_(first, iter, last, b) && skipws(first, iter, last) &&
        lit(first, iter, last, 'R')) {
        return attr = { a, b }, true;
    }

    return false;
}

} // ypdf::parser
