// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/string.hh>

namespace ypdf::parser {

template< typename Iterator >
bool parenthesized_string(Iterator first, Iterator &iter, Iterator last,
                          ast::string_t &attr)
{
    if (*iter != '(') {
        return false;
    }

    ast::string_t s(1U, *iter++);

    for (; iter != last;) {
        switch (*iter) {
        case '(': {
            ast::string_t tmp;

            if (!parenthesized_string(first, iter, last, tmp))
                return false;

            s += tmp;
        } break;

        case ')':
            s += *iter++;
            return attr = std::move(std::move(s)), true;

        case '\\': {
            s += *iter;

            if (++iter == last)
                return false;

            switch (*iter) {
            case 'n':
            case 'r':
            case 't':
            case 'b':
            case 'f':
            case '\\':
            case '(':
            case ')':
                s += *iter++;
                break;

            case '\r':
                s += *iter;

                if (++iter == last)
                    return false;

                if (*iter != '\n')
                    //
                    // Allow \CRLF
                    //
                    s += *iter++;

                break;

            case '\n':
                s += *iter++;
                break;

            default:
                s += *iter;

                if ('0' <= *iter && *iter <= '7') {
                    if (++iter == last || *iter < '0' || '7' < *iter)
                        return false;

                    s += *iter;

                    if (++iter == last || *iter < '0' || '7' < *iter)
                        return false;

                    s += *iter++;
                }

                break;
            }
        } break;

        default:
            s += *iter++;
            break;
        }
    }

    return false;
}

template< typename Iterator >
bool angular_string(Iterator, Iterator &iter, Iterator last, ast::string_t &attr)
{
    assert(*iter == '<');
    ast::string_t s(1U, *iter++);

    for (; iter != last; ++iter) {
        const int c = *iter;

        if (std::isxdigit(c)) {
            s += c;
        } else if (c == '>') {
            ++iter;

            s += c;
            attr = std::move(s);

            return true;
        } else
            return false;
    }

    return false;
}

template< typename Iterator >
bool string_(Iterator first, Iterator &iter, Iterator last, ast::string_t &attr)
{
    if (iter != last) {
        const int c = *iter;

        switch (c) {
        case '(':
            return parenthesized_string(first, iter, last, attr);

        case '<':
            return angular_string(first, iter, last, attr);

        default:
            break;
        }
    }

    return false;
}

} // ypdf::parser
