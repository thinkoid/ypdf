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

BOOST_AUTO_TEST_SUITE(ref)

static const std::vector< std::tuple< std::string, size_t, bool > >
dataset {
    { "0",         1, false },
    { "0 ",        2, false },
    { "0 0",       3, false },
    { "0 10",      4, false },
    { "0 100",     5, false },
    { "0 1 ",      4, false },
    { "0 1 S",     4, false },
    { "0 1 R",     5, true  },
    { "0 1 R ",    5, true  },
    { "0 1 Rh",    5, true  },
};

BOOST_DATA_TEST_CASE(ref, data::make (dataset), s, n, b)
{
    using namespace ypdf::parser;

    auto first = s.begin (), iter = first, last = s.end ();

    ast::ref_t attr;
    const auto result = ypdf::parser::ref (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
