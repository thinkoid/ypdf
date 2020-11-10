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

BOOST_AUTO_TEST_SUITE(skip)

static const bool arr [256] = {
#define T true
#define F false
//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    T, F, F, F, F, F, F, F, F, T, T, F, T, T, F, F, // 0
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 1
    T, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 2
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 3
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 4
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 5
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 6
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F  // 7
#undef T
#undef F
};

BOOST_DATA_TEST_CASE(is_space, data::make (arr) ^ data::xrange (256), b, ch)
{
    BOOST_TEST (b == ypdf::parser::is_space (ch));
}

static const std::vector< std::tuple< std::string, size_t, bool > >
dataset {
    { "",              0, false },
    { "\n",            1, true  },
    { "\r\n",          2, true  },
    { " \n ",          3, true  },
    { " \r\n ",        4, true  },
    { " \n\r\n ",      5, true  },
    { " \n \r\n ",     6, true  },
    { " \r\n \r\n ",   7, true  },
    { "2 ",            0, false },
    { "2\r",           0, false },
    { "2\n",           0, false },
};

BOOST_DATA_TEST_CASE(skipws, data::make (dataset), s, n, b)
{
    auto first = s.begin (), iter = first, last = s.end ();
    const auto result = ypdf::parser::skipws (first, iter, last);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
