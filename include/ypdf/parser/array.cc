// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/array.hh>
#include <ypdf/parser/lit.hh>
#include <ypdf/parser/obj.hh>
#include <ypdf/parser/skip.hh>

namespace ypdf::parser {

template< typename Iterator >
bool array(Iterator first, Iterator &iter, Iterator last, ast::array_t &attr)
{
    ast::array_t arr;

    if (lit(first, iter, last, '[')) {
        skipws(first, iter, last);

        for (; iter != last && !lit(first, iter, last, ']');) {
            ast::obj_t elem;

            if (obj(first, iter, last, elem))
                arr.emplace_back(std::move(elem));
            else
                return false;

            skipws(first, iter, last);
        }

        return attr = std::move(arr), true;
    }

    return false;
}

} // ypdf::parser
