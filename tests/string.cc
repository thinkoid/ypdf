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

BOOST_AUTO_TEST_SUITE(string_)

static const std::vector< std::tuple< std::string, std::string, bool > >
dataset {
    {  " (a)",                 "",  false }, // 0
    {  "(",                   "(",  false }, // 1
    {  "(a",                 "(a",  false }, // 2
    {  "(a>",               "(a>",  false }, // 3
    {  "(a)",               "(a)",  true  }, // 4
    {  "(a) ",              "(a)",  true  }, // 5
    {  "((a) ",           "((a) ",  false }, // 6
    {  "(((a) ",         "(((a) ",  false }, // 7
    {  "(((a)) ",       "(((a)) ",  false }, // 8
    {  "((a) ",           "((a) ",  false }, // 9
    {  "((a)) ",          "((a))",  true  }, // 0
    {  "(((a))) ",      "(((a)))",  true  }, // 1
    {  "<",                   "<",  false }, // 2
    {  "<",                   "<",  false }, // 3
    {  "<a",                 "<a",  false }, // 4
    {  "<a.",                 "<a", false }, // 5
    {  "<ab",                "<ab", false }, // 6
    {  "<ab>",              "<ab>", true  }, // 7
    {  "<abc>",            "<abc>", true  }, // 8
};

BOOST_DATA_TEST_CASE(string_, data::make (dataset), s, v, b)
{
    using namespace ypdf::parser;

    auto first = s.begin (), iter = first, last = s.end ();

    std::string attr;
    const auto result = ypdf::parser::string_ (first, iter, last, attr);

    BOOST_TEST (b == result);
    BOOST_TEST (v.size () == std::distance (first, iter));
}

BOOST_AUTO_TEST_SUITE_END()
