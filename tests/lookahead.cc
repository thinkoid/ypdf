// -*- mode: c++ -*-

#define BOOST_TEST_MODULE parser

#include <ypdf/parser.hh>

#include <boost/format.hpp>
using fmt = boost::format;

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace data = boost::unit_test::data;

#include <iostream>
#include <exception>

BOOST_AUTO_TEST_SUITE(lookahead)

static const std::vector< std::tuple< std::string, size_t, int > >
char_dataset {
    { "a", 0, 'a' },
    { "b", 0, 'b' }
};

BOOST_DATA_TEST_CASE(char_, data::make (char_dataset), s, n, c)
{
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::lookahead (iter, last);

    BOOST_TEST (c == result);
    BOOST_TEST (n == std::distance (first, iter));
}

static const std::vector< std::tuple< std::string, std::string, size_t, bool > >
string_dataset {
    { "a",   "a",     0, true },
    { "aa",  "a",     0, true },
    { "a",   "aa",    0, false },
    { "aa",  "aaa",   0, false }
};

BOOST_DATA_TEST_CASE(string_, data::make (string_dataset), s, v, n, b)
{
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::lookahead (iter, last, v);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
