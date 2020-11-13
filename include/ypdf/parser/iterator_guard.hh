// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_ITERATOR_GUARD_HH
#define YPDF_PARSER_ITERATOR_GUARD_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser {

template< typename Iterator > struct iterator_guard_t
{
    iterator_guard_t(Iterator &iter)
        : iter(iter)
        , save(iter)
        , restore(true)
    {
    }

    ~iterator_guard_t()
    {
        if (restore) {
            iter = save;
        }
    }

    void release() { restore = false; }

    Iterator &iter, save;
    bool      restore;
};

#define YPDF_ITERATOR_GUARD(x) iterator_guard_t iterator_guard(x)
#define YPDF_ITERATOR_RELEASE iterator_guard.release()
#define YPDF_PARSE_SUCCESS                                                       \
    YPDF_ITERATOR_RELEASE;                                                       \
    return true

} // ypdf::parser

#endif // YPDF_PARSER_ITERATOR_GUARD_HH
