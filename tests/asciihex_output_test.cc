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

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

#include <ypdf/stream/asciihex_output_filter.hh>

BOOST_AUTO_TEST_SUITE(asciihex_output)

static const std::vector< std::tuple< std::string, std::string, bool, bool > >
output_dataset = {
    {          "",     ">",  false, false },
    {      "\xA0",   "A0>",  false, false },
    {  "\xA0\x72",  "A072>", false, false },
};

BOOST_DATA_TEST_CASE(
    output, data::make(output_dataset), output, result, bad, fail)
{
    using namespace ypdf;

    std::string buf;
    io::filtering_ostream str;

    str.push(ypdf::iostreams::asciihex_output_filter_t());
    str.push(io::back_insert_device< std::string >(buf));

    for (auto c : output)
        str.put(c);

    boost::iostreams::close(str);

    BOOST_TEST(result == buf);

    BOOST_TEST(str.bad() == bad);
    BOOST_TEST(str.fail() == fail);
}

BOOST_AUTO_TEST_SUITE_END()
