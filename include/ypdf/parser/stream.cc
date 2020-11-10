// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/stream.hh>
#include <ypdf/parser/eol.hh>

namespace ypdf::parser {

template< typename Iterator >
bool streambuf_(Iterator first, Iterator &iter, Iterator last, ast::stream_t& attr) {
    ast::stream_t str;

    for (; iter != last; ++iter) {
        {
            // TODO: eval
            YPDF_ITERATOR_GUARD(iter);

            if (eol(first, iter, last) && lit(first, iter, last, "endstream")) {
                attr = std::move(str);
                YPDF_PARSE_SUCCESS;
            }
        }

        str.emplace_back(*iter);
    }

    return false;
}

template< typename Iterator >
bool stream_(Iterator first, Iterator &iter, Iterator last, ast::stream_t& attr) {
    {
        // TODO: eval
        YPDF_ITERATOR_GUARD(iter);

        if (!lit(first, iter, last, "stream"))
            return true;

        YPDF_ITERATOR_RELEASE;
    }

    return eol(first, iter, last) && streambuf_(first, iter, last, attr);
}

} // ypdf::parser
