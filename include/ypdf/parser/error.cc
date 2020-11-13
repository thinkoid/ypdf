// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/error.hh>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace ypdf::parser {
namespace detail {

template< typename Iterator >
inline std::tuple< off_t, off_t > streampos(Iterator iter, Iterator last)
{
    off_t line = 0, col = 0;

    for (; iter != last; ++iter) {
        if ('\n' == *iter) {
            ++line;
            col = 0;
        } else {
            ++col;
        }
    }

    return { line + 1, col + 1 };
}

template< typename Iterator, typename OutputIterator >
inline void copy_escaped(Iterator iter, Iterator last, OutputIterator out)
{
    char buf[8];

    for (; iter != last; ++iter) {
        if (std::isprint(*iter)) {
            buf[0] = *iter;
            buf[1] = 0;
        } else {
            sprintf(buf, "\\%03o", (unsigned char)*iter);
        }

        if (buf[1]) {
            *out++ = buf[0];
            *out++ = buf[1];
            *out++ = buf[2];
            *out++ = buf[3];
        } else {
            *out++ = buf[0];
        }
    }
}

template< typename Iterator, typename OutputIterator >
inline Iterator copy_cropped(Iterator iter, Iterator last, size_t nmax, int where,
                             OutputIterator     out,
                             const std::string &ellipsis = "[...]")
{
    ASSERT(nmax > ellipsis.size() + 2);

    const auto n = std::distance(iter, last);
    ASSERT(n >= 0);

    if (size_t(n) > nmax) {
        auto left = iter, right = iter;

        if (0 > where) {
            //
            // Crop left
            //
            std::advance(right, n - nmax + ellipsis.size());
        } else if (0 == where) {
            //
            // Crop middle
            //
            std::advance(left, (nmax - ellipsis.size()) / 2);
            std::advance(right, n - (nmax - ellipsis.size()) / 2);
        } else {
            //
            // Crop right
            //
            std::advance(left, nmax - ellipsis.size());
            right = last;
        }

        copy_escaped(iter, left, out);
        copy_escaped(ellipsis.begin(), ellipsis.end(), out);
        copy_escaped(right, last, out);
    } else {
        copy_escaped(iter, last, out);
    }

    return iter;
}

} // namespace detail

template< typename Iterator >
void expected(Iterator first, Iterator start, Iterator iter, Iterator last,
              const std::string &what, std::ostream &out)
{
    auto [line, col] = detail::streampos(first, iter);

    std::stringstream ss;
    ss.unsetf(std::ios_base::skipws);

    detail::copy_cropped(start, iter, 32UL, 0, std::ostream_iterator< char >(ss));

    const auto n = ss.tellp();

    detail::copy_cropped(iter, last, 32UL, 1, std::ostream_iterator< char >(ss));

    out << "error:" << line << ":" << col << ": parsing " << what << ":\n"
        << ss.rdbuf() << "\n"
        << std::string(n, '-') << '^' << std::endl;
}

} // ypdf::parser
