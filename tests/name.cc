// -*- mode: c++ -*-

#define BOOST_TEST_MODULE parser

#include <ypdf/parser.hh>
namespace ast = ypdf::parser::ast;

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

BOOST_AUTO_TEST_SUITE(name)

static const std::vector< std::tuple< std::string, size_t, bool > >
dataset {
    { "/",         1, true  }, // yep, it should pass
    { "/a",        2, true  },
    { "/a ",       2, true  },
    { "/a\t",      2, true  },
    { "/a\r",      2, true  },
    { "/a\n",      2, true  },
    { "/a/ ",      2, true  },
    { "/a# ",      3, false },
    { "/a#0 ",     4, false },
    { "/a#01 ",    5, true  },
    { "/a<",       2, true  },
};

BOOST_DATA_TEST_CASE(name, data::make (dataset), s, n, b)
{
    auto first = s.begin (), iter = first, last = s.end ();

    ast::name_t attr;
    const auto result = ypdf::parser::name(first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (n == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
