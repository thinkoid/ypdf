// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/array.hh>
#include <ypdf/parser/dict.hh>
#include <ypdf/parser/iterator_guard.hh>
#include <ypdf/parser/lookahead.hh>
#include <ypdf/parser/numeric.hh>
#include <ypdf/parser/obj.hh>
#include <ypdf/parser/ref.hh>
#include <ypdf/parser/stream.hh>
#include <ypdf/parser/string.hh>

namespace ypdf::parser {

template< typename Iterator >
bool obj(Iterator first, Iterator &iter, Iterator last, ast::obj_t& attr) {
    if (iter != last) {
        switch(const int c = *iter) {
        case '(': {
            ast::string_t s;

            if (string_(first, iter, last, s))
                return attr = std::move(s), true;
        }
            break;

        case '<': {
            if (lookahead(iter, last, "<<")) {
                ast::dict_t dict;

                if (dictionary(first, iter, last, dict))
                    return attr = std::move(dict), true;
            }
            else {
                ast::string_t s;

                if (angular_string(first, iter, last, s))
                    return attr = std::move(s), true;
            }
        }
            break;

        case '/': {
            ast::name_t s;

            if (name(first, iter, last, s))
                return attr = std::move(s), true;
        }
            break;

        case '[': {
            ast::array_t arr;

            if (array(first, iter, last, arr))
                return attr = std::move(arr), true;
        }
            break;

        case 's': {
            if (lookahead(iter, last, "stream")) {
                ast::stream_t stream;

                if (stream_(first, iter, last, stream))
                    return attr = std::move(stream), true;
            }
        }  break;

        case 't':
        case 'f': {
            bool b;

            if (bool_(first, iter, last, b)) {
                return attr = b, true;
            }
        }
            break;

        case 'n':
            if (lit(first, iter, last, "null")) {
                return attr = ast::null_t{ }, true;
            }

            break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            // reference, ...
            YPDF_ITERATOR_GUARD(iter);

            ast::ref_t reference;

            if (ref(first, iter, last, reference)) {
                attr = reference;
                YPDF_PARSE_SUCCESS;
            }
        }

        case '+': case '-': {
            {
                // ... double, or ...
                YPDF_ITERATOR_GUARD(iter);

                double d = 0;

                if (double_(first, iter, last, d)) {
                    attr = d;
                    YPDF_PARSE_SUCCESS;
                }
            }

            // ... int:
            int n = 0;

            if (int_(first, iter, last, n))
                return attr = n, true;
        }
            break;

        case '.': {
            double d;

            if (double_(first, iter, last, d)) {
                return attr = d, true;
            }
        }
            break;

        default:
            break;
        }
    }

    return false;
}

} // ypdf::parser
