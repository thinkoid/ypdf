// -*- mode: c++; -*-

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE streams

#include <cassert>

#include <iostream>
#include <string>
using namespace std::string_literals;

#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>
namespace utf = boost::unit_test;

#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
namespace data = boost::unit_test::data;

#include <boost/iostreams/filtering_stream.hpp>
namespace io = boost::iostreams;

#include <ypdf/stream/ascii85_input_filter.hh>

BOOST_AUTO_TEST_SUITE(ascii85_input)

static const std::vector< std::tuple< std::string, std::string, bool, bool, bool > >
input_dataset = {
#define T true
#define F false

    { "",                 "",             T, F, T },                //  0
    { "BE",               "h",            T, F, T },                //  1
    { "BOq",              "he",           T, F, T },                //  2
    { "BOtu",             "hel",          T, F, T },                //  3
    { "BOu!r",            "hell",         T, F, T },                //  4
    { "BOu!rDZ",          "hello",        T, F, T },                //  5
    { "BOu!rD]f",         "hello ",       T, F, T },                //  6
    { "BOu!rD]j6",        "hello w",      T, F, T },                //  7
    { "BOu!rD]j7B",       "hello wo",     T, F, T },                //  8
    { "BOu!rD]j7BEW",     "hello wor",    T, F, T },                //  9
    { "BOu!rD]j7BEbk",    "hello worl",   T, F, T },                // 10
    { "BOu!rD]j7BEbo7",   "hello world",  T, F, T },                // 11
    { "BOu!rD]j7BEbo80",  "hello world!", T, F, T },                // 12

    //
    // ASCII85 EOD marker:
    //
    { "~>",                 "",             T, F, T },              // 13
    { "BE~>",               "h",            T, F, T },              // 14
    { "BOq~>",              "he",           T, F, T },              // 15
    { "BOtu~>",             "hel",          T, F, T },              // 16
    { "BOu!r~>",            "hell",         T, F, T },              // 17
    { "BOu!rDZ~>",          "hello",        T, F, T },              // 18
    { "BOu!rD]f~>",         "hello ",       T, F, T },              // 19
    { "BOu!rD]j6~>",        "hello w",      T, F, T },              // 20
    { "BOu!rD]j7B~>",       "hello wo",     T, F, T },              // 21
    { "BOu!rD]j7BEW~>",     "hello wor",    T, F, T },              // 22
    { "BOu!rD]j7BEbk~>",    "hello worl",   T, F, T },              // 23
    { "BOu!rD]j7BEbo7~>",   "hello world",  T, F, T },              // 24
    { "BOu!rD]j7BEbo80~>",  "hello world!", T, F, T },              // 25

    //
    // Allow incomplete ASCII85 EOD marker:
    //
    { "~",                 "",             T, F, T },               // 26
    { "BE~",               "h",            T, F, T },               // 27
    { "BOq~",              "he",           T, F, T },               // 28
    { "BOtu~",             "hel",          T, F, T },               // 29
    { "BOu!r~",            "hell",         T, F, T },               // 30
    { "BOu!rDZ~",          "hello",        T, F, T },               // 31
    { "BOu!rD]f~",         "hello ",       T, F, T },               // 32
    { "BOu!rD]j6~",        "hello w",      T, F, T },               // 33
    { "BOu!rD]j7B~",       "hello wo",     T, F, T },               // 34
    { "BOu!rD]j7BEW~",     "hello wor",    T, F, T },               // 35
    { "BOu!rD]j7BEbk~",    "hello worl",   T, F, T },               // 36
    { "BOu!rD]j7BEbo7~",   "hello world",  T, F, T },               // 37
    { "BOu!rD]j7BEbo80~",  "hello world!", T, F, T },               // 38

    //
    // Ignore embedded space characters (zero is space):
    //
    { "BO\0 u!rD]\tj7BE\nbo80\0"s,    "hello world!", T, F, T },    // 39
    { "BOu !rD]j\0""7B\nE\tbo80\0"s,  "hello world!", T, F, T },    // 40
    { "BOu!rD ]\nj7\0BEb\to80\0"s,    "hello world!", T, F, T },    // 41
    { "BOu!\t\nrD]j7 B\0Ebo80\0"s,    "hello world!", T, F, T },    // 42
    { "B\nOu!rD\t]j7BEb o\080\0"s,    "hello world!", T, F, T },    // 43
    { "B\tOu!rD]j7BEb o8\0""0\0\0"s,  "hello world!", T, F, T },    // 44
    { "BO\nu\t!rD]j 7BEb\no80\0"s,    "hello world!", T, F, T },    // 45

#undef T
#undef F
};

BOOST_DATA_TEST_CASE(input, data::make(input_dataset),
                     from, to, eof, bad, fail)
{
    using namespace ypdf;

    std::string s;

    {
        io::filtering_istream str;

        str.push(ypdf::iostreams::ascii85_input_filter_t());
        str.push(io::array_source(from.c_str(), from.size()));

        for (int c; EOF != (c = str.get());)
            s.append(1, c);

        io::close(str);

        BOOST_TEST(str.eof() == eof);
        BOOST_TEST(str.bad() == bad);
        BOOST_TEST(str.fail() == fail);
    }

    BOOST_TEST(to == s);
}

BOOST_AUTO_TEST_SUITE_END()
