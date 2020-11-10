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

BOOST_AUTO_TEST_SUITE(comment)

static const std::vector< std::tuple< std::string, size_t, bool > >
dataset {
    { "",       0, false },

    { "%",      1, true },
    { "% ",     2, true },
    { "%\n ",   2, true },
    { "% \n ",  3, true },

    { "%%",     2, true },
    { "%% ",    3, true },
    { "%% \n",  4, true },
    { "%% \n ", 4, true },

    { "%%P",    3, true },
    { "%%PD",   4, true },
    { "%%PDF",  5, true },

    { "%%E",    3, true },
    { "%%EO",   4, true },
    { "%%EOF",  5, true }
};

BOOST_DATA_TEST_CASE(comment_test2, data::make (dataset), s, n, b) {
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::comment (first, iter, last);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
