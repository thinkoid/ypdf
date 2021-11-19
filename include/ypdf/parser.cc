// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/detail/defs.hh>
#include <ypdf/parser.hh>

#include <vector>

namespace ypdf {

template< typename Iterator >
bool parse(Iterator first, Iterator &iter, Iterator last,
           std::vector< parser::ast::xref_t > &attr)
{
    using namespace parser;

    auto  start = iter;
    off_t off;

    if (!rstartxref(first, iter, last, off)) {
        expected(first, start, iter, last, "startxref");
        return false;
    }

    iter = first;
    std::advance(iter, off);

    std::vector< ast::xref_t > table;

    for (auto start = iter; iter != last; start = iter) {
        std::vector< ast::xref_t > section;

        if (!xrefs_section(first, iter, last, section))
            return expected(first, start, iter, last, "xref section"), false;

        table.insert(table.end(), section.begin(), section.end());
    }

    return attr = std::move(table), true;
}

template< typename Iterator >
bool parse(Iterator first, Iterator &iter, Iterator last,
           std::vector< parser::ast::iobj_t > &attr)
{
    using namespace ypdf::parser;

    std::vector< ast::iobj_t > xs;

    for (SKIP; iter != last; SKIP) {
        const auto start = iter;

        switch (*iter) {
        case '%':
            if (!comment(first, iter, last))
                return false;

            break;

        case 's': {
            //
            // startxref
            //
            off_t ignore;

            if (!startxref(first, iter, last, ignore)) {
                expected(first, start, iter, last, "startxref");
                return false;
            }
        } break;

        case 't': {
            //
            // trailer
            //
            ast::dict_t ignore;

            if (!trailer(first, iter, last, ignore)) {
                expected(first, start, iter, last, "trailer");
                return false;
            }
        } break;

        case 'x': {
            //
            // xref
            //
            std::vector< ast::xref_t > ignore;

            if (!xrefs(first, iter, last, ignore)) {
                expected(first, start, iter, last, "xref");
                return false;
            }
        } break;

        default:
            if (std::isdigit(*iter)) {
                ast::iobj_t iobj_;

                if (!iobj(first, iter, last, iobj_)) {
                    expected(first, start, iter, last, "object");
                    return false;
                }

                xs.emplace_back(std::move(iobj_));
            } else {
                expected(first, start, iter, last, "anything but this");
                return false;
            }
            break;
        }
    }

    return std::swap(xs, attr), true;
}

} // namespace ypdf
