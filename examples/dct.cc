// -*- mode: c++; -*-

#include <ypdf/ypdf.hh>
using namespace ypdf::parser::ast;

#include "io.hh"

#include <iostream>
#include <optional>

#include <filesystem>
namespace fs = std::filesystem;

#include <boost/none.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
namespace io = ::boost::iostreams;

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/filter.hpp>
using namespace ranges;

#include <boost/hana/functional/overload.hpp>
#include <boost/hana/functional/partial.hpp>
namespace hana = boost::hana;

////////////////////////////////////////////////////////////////////////

namespace example {

struct asciihex_output_filter_t : public boost::iostreams::output_filter
{
    template< typename Sink >
    bool put(Sink &dst, char c)
    {
        static const char *s = "0123456789ABCDEF";

        if (eof_)
            return false;

        const auto u = static_cast< unsigned char >(c);

        return
            ::io::put(dst, s[(u & 0xF0) >> 4]) &&
            ::io::put(dst, s[ u & 0x0F]) &&
            ::io::put(dst, ' ');
    }

    template< typename Sink >
    void close(Sink &dst)
    {
        eof_ = true;
    }

private:
    bool eof_ = false;
};

template< typename T >
inline std::vector< T > to_array(const obj_t &obj)
{
    using namespace ranges;

    std::vector< T > xs;

    const auto visitor = hana::overload(
        [&](const array_t &x) {
            copy(as< array_t >(obj) | views::transform([](auto &x) {
                return as< T >(x);
            }), ranges::back_inserter(xs));
        },
        [&](const T &x) { xs.push_back(as< T >(obj)); },
        [&](const auto &)  { ASSERT(0); }
        );

    boost::apply_visitor(visitor, obj);

    return xs;
};

} // namespace example

template< typename T >
inline std::optional< T > maybe_at(const dict_t &xs, const char *key)
{
    try {
        return as< T >(xs.at(key));
    } catch(...) {
        return std::optional< T >{ };
    }
}

static auto make_filtering_istream(const dict_t &dict)
{
    namespace io_ = ypdf::iostreams;

    auto ptr = std::make_unique< io::filtering_istream >();

    if (dict.has("/Filter")) {
        for (auto &filter : example::to_array< name_t >(dict.at("/Filter"))) {
            if (filter == "/FlateDecode") {
                if (dict.has("/DecodeParms")) {
                    const auto &xs = as< dict_t >(dict.at("/DecodeParms"));

                    auto pred = as< int >(xs.at("/Predictor"));
                    assert(10 <= pred && pred <= 15);

                    size_t ppl = maybe_at< int >(xs, "/Columns").value_or(1);
                    size_t cpp = maybe_at< int >(xs, "/Colors").value_or(1);
                    size_t bpc = maybe_at< int >(xs, "/BitsPerComponent").value_or(8);

                    ptr->push(io_::png_predictor_input_filter_t(ppl, cpp, bpc));
                }

                ptr->push(io::zlib_decompressor());
            } else if (filter == "/ASCII85Decode") {
                ptr->push(io_::ascii85_input_filter_t());
            } else if (filter == "/LZWDecode") {
                ptr->push(io_::lzw_input_filter_t());
            } else if (filter == "/DCTDecode") {
                ptr->push(io_::dct_input_filter_t());
            } else {
                throw std::runtime_error("unsupported filter");
            }
        }
    }

    return ptr;
}

static auto iobjs_from(const char *s)
{
    io::mapped_file_source src(s);

    auto iter = src.data(), end = iter;
    std::advance(end, src.size());

    std::vector< iobj_t > iobjs;

    if (!ypdf::parse(iter, iter, end, iobjs))
        throw std::runtime_error("malformed document");

    return iobjs;
}

static void run_with(const char *s)
{
    for (const auto &x : iobjs_from(s)) {
        const auto &arr = as< array_t >(x.obj);
        ASSERT(!arr.empty());

        if (is< dict_t >(arr[0])) {
            const auto &dict = as< dict_t >(arr[0]);

            if (dict.has("/Filter")) {
                auto filters = example::to_array< name_t >(dict.at("/Filter"));

                if (ranges::contains(filters, name_t{ "/DCTDecode" })) {
                    auto src = make_filtering_istream(dict);

                    const auto &stream = as< stream_t >(arr[1]);
                    src->push(io::array_source(stream.data(), stream.size()));

                    auto fname = std::tmpnam(0);
                    std::cout << fname << std::endl;
                    std::ofstream dst(fname, std::ios_base::binary | std::ios_base::out);
                    for (int c; EOF != (c = src->get()); dst.put(char(c))) { }
                }
            }
        }
    }
}

int main(int, char **argv)
{
    if (0 == argv[1] || 0 == argv[1][0]) {
        std::cerr << "Usage: program filename" << std::endl;
        return 1;
    }

    return run_with(argv[1]), 0;
}
