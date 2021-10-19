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

BOOST_AUTO_TEST_SUITE(numeric)

static const std::vector< std::tuple< std::string, size_t, bool > >
bool_dataset {
    { "t",       1, false },
    { "tr",      2, false },
    { "tru",     3, false },
    { "true",    4, true  },
    { "f",       1, false },
    { "fa",      2, false },
    { "fal",     3, false },
    { "fals",    4, false },
    { "false",   5, true  }
};

BOOST_DATA_TEST_CASE(bool_, data::make (bool_dataset), s, n, b)
{
    auto first = s.begin (), iter = first, last = s.end ();

    bool attr;
    const auto result = ypdf::parser::bool_ (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

static const bool digit_dataset[256] = {
#define T true
#define F false
//  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 0
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 1
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 2
    T, T, T, T, T, T, T, T, T, T, F, F, F, F, F, F, // 3
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 4
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 5
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, // 6
    F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F  // 7
#undef T
#undef F
};

BOOST_DATA_TEST_CASE(digit, data::make (digit_dataset) ^ data::xrange (256), b, i)
{
    std::string s (1U, char (i));
    auto first = s.begin (), iter = first, last = s.end ();

    int attr = 0;
    const auto result = ypdf::parser::digit (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (std::distance (first, iter) == (b ? 1 : 0));
    BOOST_TEST (attr == (b ? i - '0' : 0));
}

static const std::vector< std::tuple< std::string, size_t, bool, int > >
int_dataset {
    { "",     0,  false,    0 },
    { "+",    1,  false,    0 },
    { "+ ",   1,  false,    0 },
    { "1",    1,  true,     1 },
    { "1x",   1,  true,     1 },
    { "1.",   1,  true,     1 },
    { "1,",   1,  true,     1 },
    { "-1",   2,  true,    -1 },
    { "-1\n", 2,  true,    -1 },
    { "+1\r", 2,  true,     1 },
    { "-1\t", 2,  true,    -1 },
    { "+1\b", 2,  true,     1 },
    { "-1\f", 2,  true,    -1 }
};

BOOST_DATA_TEST_CASE(int_, data::make (int_dataset), s, n, b, val)
{
    auto first = s.begin (), iter = first, last = s.end ();

    int attr = 0;
    const auto result = ypdf::parser::int_ (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
    BOOST_TEST (attr == (b ? val : 0));
}

static const std::vector< std::tuple< std::string, size_t, bool, double > >
double_dataset {
    { "",          0,     false,        0 }, //  0
    { "+",         1,     false,        0 }, //  1
    { "+ ",        1,     false,        0 }, //  2
    { "-",         1,     false,        0 }, //  3
    { "- ",        1,     false,        0 }, //  4
    { "1",         1,     false,        0 }, //  5
    { "1x",        1,     false,        0 }, //  6
    { "1.",        2,      true,        1 }, //  7
    { ".",         1,     false,        1 }, //  8
    { "1,",        1,     false,        1 }, //  9
    { "-1",        2,     false,       -1 }, // 10
    { "-1.",       3,      true,       -1 }, // 11
    { "+1.",       3,      true,        1 }, // 12
    { "+1.3 ",     4,      true,      1.3 }, // 13
    { "+1.e ",     4,     false,        0 }, // 14
    { "+1.e3 ",    5,      true,    1000. }, // 15
    { "+1.e0 ",    5,      true,       1. }, // 16
    { "+1.E ",     4,     false,        0 }, // 17
    { "+1.E3 ",    5,      true,    1000. }, // 18
    { "+1.E0 ",    5,      true,       1. }, // 19
    { "+1.E0 ",    5,      true,       1. }, // 20
    { "+1.e+ ",    5,     false,        0 }, // 21
    { "+1.e+3 ",   6,      true,    1000. }, // 22
    { "+1.e+0 ",   6,      true,       1. }, // 23
    { "+1.E01 ",   6,      true,      10. }, // 24
    { "+1.e-1 ",   6,      true,       .1 }, // 25
    { "+1.e-02 ",  7,      true,      .01 }, // 26
    { "+1.e-03 ",  7,      true,     .001 }  // 27
};

BOOST_DATA_TEST_CASE(double_, data::make (double_dataset), s, n, b, val)
{
    auto first = s.begin (), iter = first, last = s.end ();

    double attr = 0;
    const auto result = ypdf::parser::double_ (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
    BOOST_TEST (attr == (b ? val : 0.));
}

BOOST_AUTO_TEST_SUITE_END()
