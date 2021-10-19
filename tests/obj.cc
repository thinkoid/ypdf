// -*- mode: c++ -*-

#define BOOST_TEST_MODULE parser

#include <ypdf/parser.hh>
namespace ast = ypdf::parser::ast;

#include <ypdf/detail/overloaded.hh>

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

BOOST_AUTO_TEST_SUITE(obj)

static const std::vector< std::tuple< std::string, int, int, bool, bool > >
ref_dataset {
    { "1 0",   0, 0, true, false },
    { "1 2 R", 1, 2, true, true  },
};

BOOST_DATA_TEST_CASE(ref_, data::make (ref_dataset), s, a, b, success, valid) {
    static auto eq = [&](ast::ref_t arg, ast::obj_t& obj) {
        return arg == as< ast::ref_t >(obj);
    };

    auto first = s.begin (), iter = first, last = s.end ();

    ast::obj_t attr;
    const auto result = ypdf::parser::obj (first, iter, last, attr);

    BOOST_TEST (result == success);

    if (result && valid) {
        BOOST_TEST (is< ast::ref_t > (attr));
        ast::ref_t ref{ a, b };
        BOOST_TEST (eq (ref, attr));
    }
}

static const std::vector< std::tuple< std::string, int, bool, bool > >
int_dataset {
    { "+",    0,  false, false },
    { ".",    0,  false, false },
    { "+5",   5,  true,  true  },
    { "-5",  -5,  true,  true  },
    { "5.",   0,  true,  false },
    { "5 ",   5,  true,  true  },
};

BOOST_DATA_TEST_CASE(int_, data::make (int_dataset), s, n, success, valid) {
    static auto eq = [&](int arg, ast::obj_t& obj) {
        return arg == as< int >(obj);
    };

    auto first = s.begin (), iter = first, last = s.end ();

    ast::obj_t attr;
    const auto result = ypdf::parser::obj (first, iter, last, attr);

    BOOST_TEST (result == success);

    if (result && valid) {
        BOOST_TEST (is< int > (attr));
        BOOST_TEST (eq (n, attr));
    }
}

static const std::vector< std::tuple< std::string, double, bool, bool > >
double_dataset {
    { "+",     0,  false, false },
    { ".",     0,  false, false },
    { ".1",   .1,  true,  true  },
    { "1",     0,  true,  false },
    { "+5.",  5.,  true,  true  },
    { "-5.", -5.,  true,  true  },
    { "5.1", 5.1,  true,  true  },
    { "5. ",  5.,  true,  true  },
};

BOOST_DATA_TEST_CASE(double_, data::make (double_dataset), s, n, success, valid) {
    static auto eq = [&](double arg, ast::obj_t& obj) {
        return arg == as< double >(obj);
    };

    auto first = s.begin (), iter = first, last = s.end ();

    ast::obj_t attr;
    const auto result = ypdf::parser::obj (first, iter, last, attr);

    BOOST_TEST (result == success);

    if (result && valid) {
        BOOST_TEST (is< double > (attr));
        BOOST_TEST (eq (n, attr));
    }
}

BOOST_AUTO_TEST_SUITE_END()
