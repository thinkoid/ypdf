// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/dict.hh>
#include <ypdf/parser/lit.hh>
#include <ypdf/parser/trailer.hh>

namespace ypdf::parser {

template< typename Iterator >
bool trailer(Iterator first, Iterator &iter, Iterator last, ast::dict_t &attr)
{
    if (!lit(first, iter, last, "trailer"))
        return false;

    SKIP;

    ast::dict_t trailer;

    if (!dictionary(first, iter, last, trailer))
        return false;

    return attr = std::move(trailer), true;
}

} // ypdf::parser
