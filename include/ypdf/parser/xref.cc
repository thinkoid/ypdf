// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/xref.hh>
#include <ypdf/parser/lit.hh>
#include <ypdf/parser/numeric.hh>
#include <ypdf/parser/skip.hh>
#include <ypdf/parser/skip.hh>
#include <ypdf/parser/xreftbl.hh>
#include <ypdf/parser/xrefstm.hh>

namespace ypdf::parser {
namespace detail {

template< typename Iterator >
bool xref(Iterator first, Iterator &iter, Iterator last, int num,
          ast::xref_t &attr)
{
    int off, gen;

    if (!ints(first, iter, last, off, gen))
        return false;

    SKIP;

    if (iter == last)
        return false;

    if (*iter == 'f') {
        attr = ast::xref_t(ast::free_xref_t{ num, gen });
        return ++iter, true;
    }
    else if (*iter == 'n') {
        attr = ast::xref_t(ast::basic_xref_t{ { num, gen }, off });
        return ++iter, true;
    }
    else
        return false;
}

template< typename Iterator >
bool xrefs(Iterator first, Iterator &iter, Iterator last,
           int i, int n, std::vector< ast::xref_t > &attr)
{
    std::vector< ast::xref_t > subsection;

    for (n += i; i < n; ++i, SKIP) {
        ast::xref_t xref_;

        if (!xref(first, iter, last, i, xref_))
            return false;

        subsection.push_back(std::move(xref_));
    }

    return attr = std::move(subsection), true;
}

template< typename Iterator >
bool xrefs(Iterator first, Iterator &iter, Iterator last,
           std::vector< ast::xref_t > &attr)
{
    int i, n;

    if (!ints(first, iter, last, i, n))
        return false;

    SKIP;

    std::vector< ast::xref_t > subsection;

    if (!xrefs(first, iter, last, i, n, subsection))
        return false;

    return attr = std::move(subsection), true;
}

} // namespace detail

template< typename Iterator >
bool rstartxref(Iterator first, Iterator &iter, Iterator last, off_t& attr)
{
    off_t off = 0;

    {
        const char text[] = "ferxtrats";

        auto rbeg = std::reverse_iterator(last);
        auto rend = std::reverse_iterator(first);

        auto riter = std::search(rbeg, rend, text, text + sizeof text - 1);

        if (riter == rend)
            return false;

        iter = riter.base();
    }

    if (SKIP && int_(first, iter, last, off))
        return attr = off, true;

    return false;
}

template< typename Iterator >
bool startxref(Iterator first, Iterator &iter, Iterator last, off_t& attr)
{
    int n = 0;

    if (lit  (first, iter, last, "startxref") && SKIP &&
        int_ (first, iter, last, n)) {
        return attr = n, true;
    }

    return false;
}

template< typename Iterator >
bool xrefs(Iterator first, Iterator &iter, Iterator last,
           std::vector< ast::xref_t > &attr)
{
    std::vector< ast::xref_t > section;

    if (!lit(first, iter, last, "xref"))
        return false;

    SKIP;

    int c = 0;

    for (; iter != last && std::isdigit(*iter); ++c, SKIP) {
        std::vector< ast::xref_t > subsection;

        if (!detail::xrefs(first, iter, last, subsection))
            break;

        section.insert(section.end(), subsection.begin(), subsection.end());
    }

    if (0 == c)
        return false;

    return attr = section, true;
}

template< typename Iterator >
bool xrefs_section(Iterator first, Iterator &iter, Iterator last,
                   std::vector< ast::xref_t > &attr)
{
    if (iter == last)
        return false;

    using namespace parser;

    if (std::isdigit(*iter))
        return xrefstm(first, iter, last, attr);
    else if ('x' == *iter)
        return xreftbl(first, iter, last, attr);
    else
        return false;
}

} // ypdf::parser
