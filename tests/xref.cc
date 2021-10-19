// -*- mode: c++ -*-

#define BOOST_TEST_MODULE parser

#include <ypdf/parser.hh>
namespace ast = ::ypdf::parser::ast;

#include <ypdf/detail/overloaded.hh>

#include <boost/format.hpp>
using fmt = ::boost::format;

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = ::boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace data = ::boost::unit_test::data;

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/iterator/stream_iterators.hpp>

#include <iostream>
#include <exception>

namespace ypdf::parser::ast {

inline std::ostream &operator<<(std::ostream &s, const ref_t &x)
{
    return s << x.num << ' ' << x.gen << " R";
}

inline std::ostream &operator<<(std::ostream &s, const free_xref_t &arg)
{
    return s << arg.ref;
}

inline std::ostream &operator<<(std::ostream &s, const basic_xref_t &arg)
{
    return s << arg.ref << " " << arg.off;
}

inline std::ostream &operator<<(std::ostream &s, const stream_xref_t &arg)
{
    return s << arg.ref << " " << arg.stream << " " << arg.pos;
}

inline std::ostream &operator<<(std::ostream &s, const xref_t &arg)
{
    boost::apply_visitor([&s](const auto &x) { s << x; }, arg);
    return s;
}

} // namespace ypdf::parser::ast

namespace std {

inline ostream &operator<<(ostream &s, const vector< ast::xref_t > &xs)
{
    for (const auto &x : xs)
        s << x << ' ';

    return s;
}

} // namespace std

BOOST_AUTO_TEST_SUITE(xref)

const std::vector<
    std::tuple< std::string, size_t, bool, std::vector< ast::xref_t > >
    >
xreftbl_dataset
{
#define B(a, b, c) ast::xref_t(ast::basic_xref_t{ { a, b }, c })
#define F(a, b)    ast::xref_t(ast::free_xref_t{ a, b })

    { "xref\n",                                  5,   false, { } },
    { "xref\n0",                                 6,   false, { } },
    { "xref\n0 ",                                7,   false, { } },
    { "xref\n0 1",                               8,   false, { } },
    { "xref\n0 1 ",                              9,   false, { } },
    { "xref\n0 1 2",                            10,   false, { } },
    { "xref\n0 1\n",                             9,   false, { } },
    { "xref\n0 1\n0 1 n",                       14,    true, { B(0, 1, 0) } },
    { "xref\n0 1\n0 1 f",                       14,    true, { F(0, 1) } },
    { "xref\n0 1\n0 65535 f",                   18,    true, { F(0, 65535) } },
    { "xref\n0 1\n0 65535 n",                   18,    true, { B(0, 65535, 0)} },
    { "xref\n0 2\n0 65535 f\n1 1 f",            24,    true, { F(0, 65535), F(1, 1) } },

#undef B
#undef F
};

BOOST_DATA_TEST_CASE(xreftbl, data::make(xreftbl_dataset), s, n, b, xreftbl_)
{
    using namespace ypdf::parser;

    auto first = s.begin (), iter = first, last = s.end ();

    std::vector< ast::xref_t > attr;
    auto result = ::ypdf::parser::xrefs(first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));

    if (b) {
        BOOST_TEST(ranges::equal(xreftbl_, attr));
    }
}

BOOST_AUTO_TEST_SUITE_END()
