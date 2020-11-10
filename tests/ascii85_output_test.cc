// -*- mode: c++; -*-

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE streams

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

#include <ypdf/stream/ascii85_output_filter.hh>

BOOST_AUTO_TEST_SUITE(ascii85_output)

static const std::vector< std::tuple< std::string, std::string, bool, bool, bool > >
output_encode_dataset = {
#define T true
#define F false

    { "",             "",                 T, F, F },
    { "h",            "BE",               T, F, F },
    { "he",           "BOq",              T, F, F },
    { "hel",          "BOtu",             T, F, F },
    { "hell",         "BOu!r",            T, F, F },
    { "hello",        "BOu!rDZ",          T, F, F },
    { "hello ",       "BOu!rD]f",         T, F, F },
    { "hello w",      "BOu!rD]j6",        T, F, F },
    { "hello wo",     "BOu!rD]j7B",       T, F, F },
    { "hello wor",    "BOu!rD]j7BEW",     T, F, F },
    { "hello worl",   "BOu!rD]j7BEbk",    T, F, F },
    { "hello world",  "BOu!rD]j7BEbo7",   T, F, F },
    { "hello world!", "BOu!rD]j7BEbo80",  T, F, F },

#undef T
#undef F
};

BOOST_DATA_TEST_CASE(
    output_encode, data::make(output_encode_dataset), output, result,
    eof, bad, fail)

{
    UNUSED(eof);
    UNUSED(bad);
    UNUSED(fail);

    using namespace ypdf;

    std::string buf;

    {
        io::filtering_ostream str;

        str.push(ypdf::iostreams::ascii85_output_filter_t());
        str.push(io::back_insert_device< std::string >(buf));

        for (auto c : output)
            str.put(c);

        io::close(str);

        BOOST_TEST(str.bad()  == bad);
        BOOST_TEST(str.fail() == fail);
    }

    BOOST_TEST(result.size() == buf.size());
    BOOST_TEST(result == buf);
}

BOOST_AUTO_TEST_SUITE_END()
