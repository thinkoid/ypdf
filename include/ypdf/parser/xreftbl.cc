// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/xref.hh>

namespace ypdf::parser {

template< typename Iterator >
bool xreftbl(Iterator first, Iterator &iter, Iterator last,
             std::vector< ast::xref_t > &attr)
{
    std::vector< ast::xref_t > section;
    ast::dict_t trailer_;

    if (!parser::xrefs(first, iter, last, section))
        return false;

    SKIP;

    if (!trailer(first, iter, last, trailer_))
        return false;

    {
        iter = last;

        if (trailer_.has("/Prev")) {
            const auto off = as< int >(trailer_.at("/Prev"));
            std::advance((iter = first), off);
        }
    }

    return attr = std::move(section), true;
}

} // ypdf::parser
