// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE iostreams

#include <cassert>
#include <iostream>

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace data = boost::unit_test::data;

#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

#include <ypdf/stream/asciihex_input_filter.hh>

BOOST_AUTO_TEST_SUITE(asciihex_input)

static const std::vector< std::tuple< std::string, std::string, bool, bool, bool > >
input_dataset = {
#define T true
#define F false

    {      ">",                         "",  T,  F,  T },
    {     " >",                         "",  T,  F,  T },
    {    "  >",                         "",  T,  F,  T },
    {   "   >",                         "",  T,  F,  T },
    {     "0>",                         "",  F,  T,  T }, // divergent behavior
    {    "00>",     std::string(  "\0", 1),  T,  F,  T },
    {   "000>",                         "",  F,  T,  T }, // divergent behavior
    {  "0000>",     std::string("\0\0", 2),  T,  F,  T },
    {     "A>",                         "",  F,  T,  T }, // divergent behavior
    {    "AA>", std::string(    "\xAA", 1),  T,  F,  T },
    {   "AAA>",                         "",  F,  T,  T }, // divergent behavior
    {  "AAAA>", std::string("\xAA\xAA", 2),  T,  F,  T },

#undef T
#undef F
};

BOOST_DATA_TEST_CASE(
    input, data::make(input_dataset), input, result, eof, bad, fail)
{
    using namespace ypdf;

    io::filtering_istream str;

    str.push(ypdf::iostreams::asciihex_input_filter_t());
    str.push(io::array_source(input.c_str(), input.size()));

    std::string buf;

    for (int c; str && EOF != (c = str.get());)
        buf.append(1, c);

    BOOST_TEST(result == buf);

    BOOST_TEST(str.eof() == eof);
    BOOST_TEST(str.bad() == bad);
    BOOST_TEST(str.fail() == fail);
}

BOOST_AUTO_TEST_SUITE_END()
