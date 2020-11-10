// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/dict.hh>
#include <ypdf/parser/comment.hh>
#include <ypdf/parser/lit.hh>
#include <ypdf/parser/name.hh>
#include <ypdf/parser/obj.hh>
#include <ypdf/parser/skip.hh>

namespace ypdf::parser {

template< typename Iterator >
bool definition(Iterator first, Iterator &iter, Iterator last,
                 std::tuple< ast::name_t, ast::obj_t >& attr) {
    std::tuple< ast::name_t, ast::obj_t > def;

    if (name(first, iter, last, std::get< 0 >(def))) {
        skipws(first, iter, last);

        if (obj(first, iter, last, std::get< 1 >(def)))
            return attr = std::move(def), true;
    }

    return false;
}

template< typename Iterator >
bool dictionary(Iterator first, Iterator &iter, Iterator last,
                 ast::dict_t& attr) {
    if (lit(first, iter, last, "<<")) {
        skipws(first, iter, last);
        comment(first, iter, last);

        skipws(first, iter, last);

        ast::dict_t dict;

        for (; iter != last;) {
            switch(*iter) {
            case '/': {
                std::tuple< ast::name_t, ast::obj_t > def;

                if (!definition(first, iter, last, def)) {
                    return false;
                }

                dict.push_back(std::move(def));
            }
                break;

            case '>':
                if (lit(first, iter, last, ">>")) {
                    return attr = std::move(dict), true;
                }

            default:
                return false;
            }

            skipws(first, iter, last);
            comment(first, iter, last);

            skipws(first, iter, last);
        }
    }

    return false;
}

} // ypdf::parser
