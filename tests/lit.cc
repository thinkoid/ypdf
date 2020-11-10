// -*- mode: c++ -*-

#define BOOST_TEST_DYN_LINK
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

BOOST_AUTO_TEST_SUITE(lit)

static const std::vector< std::tuple< std::string, int, size_t, bool > >
char_dataset {
    { "a", 'a', 1, true },
    { "b", 'a', 0, false }
};

BOOST_DATA_TEST_CASE(char_, data::make (char_dataset), s, c, n, b) {
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::lit (first, iter, last, c);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

static const std::vector< std::tuple< std::string, std::string, size_t, bool > >
string_dataset {
    { "a",   "a",     1, true },
    { "aa",  "a",     1, true },
    { "a",   "aa",    1, false },
    { "aa",  "aaa",   2, false }
};

BOOST_DATA_TEST_CASE(string_, data::make (string_dataset), s, v, n, b) {
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::lit (first, iter, last, v);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
