// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/name.hh>

namespace ypdf::parser {

template< typename Iterator >
bool name(Iterator first, Iterator &iter, Iterator last, ast::name_t &attr)
{
    if (iter != last && *iter == '/') {
        std::stringstream ss;
        ss << *iter;

        for (++iter; iter != last && *iter; ++iter) {
            if (*iter == '#') {
                ss << *iter;

                if (++iter == last || !std::isxdigit(*iter))
                    return false;

                ss << *iter;

                if (++iter == last || !std::isxdigit(*iter))
                    return false;

                ss << *iter;
            } else if (is_regular(*iter)) {
                ss << *iter;
            } else {
                break;
            }
        }

        return attr = { ss.str() }, true;
    }

    return false;
}

} // ypdf::parser
