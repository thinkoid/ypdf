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

BOOST_AUTO_TEST_SUITE(eol)

static const std::vector< std::tuple< std::string, size_t, bool > >
dataset {
    { "",              0, false },
    { "\n",            1, true },
    { "\r",            1, true },
    { "\r\n",          2, true },
    { "\r\r",          1, true },
    { "\n\r",          1, true },
    { "\n\n",          1, true }
};

BOOST_DATA_TEST_CASE(eol, data::make (dataset), s, n, b) {
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::eol (first, iter, last);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
