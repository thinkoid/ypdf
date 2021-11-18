// -*- mode: c++; -*-

#include <ypdf/ypdf.hh>
using namespace ypdf::parser::ast;

#include "io.hh"
#include "options.hh"

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

#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
using namespace ranges;

#include <boost/hana/functional/overload.hpp>
#include <boost/hana/functional/partial.hpp>
namespace hana = boost::hana;

////////////////////////////////////////////////////////////////////////

options_t::options_t(int argc, char **argv)
{
    {
        auto tmp = std::make_pair(
            "program", ::po::variable_value(std::string(argv[0]), false));
        map_.insert(tmp);
    }

    ::po::options_description generic("Generic options");
    ::po::options_description config("Configuration options");

    generic.add_options()("version,v", "version")("help,h", "this");

    config.add_options()
        ( "hex", "write binary data as ASCII hex")
        ( "deflate", "deflate compressed streams")
        ( "type,t", ::po::value< std::string >(), "filter objects by type")
        ( "ref,r",  ::po::value< int >(), "filter objects by ref number")
        ("input,i", ::po::value< std::string >(), "input (file).");

    desc_ = std::make_shared< ::po::options_description >();

    desc_->add(generic);
    desc_->add(config);

    store(::po::command_line_parser(argc, argv).options(*desc_).run(), map_);

    ::po::positional_options_description pos;
    pos.add("input", -1);

    store(::po::command_line_parser(argc, argv).
          options(*desc_).positional(pos).run(), map_);

    notify(map_);
}

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

template< typename T > inline auto to_array(const obj_t &obj)
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

static auto
make_filtering_istream(const options_t &opts, const dict_t &dict)
{
    namespace io_ = ypdf::iostreams;

    auto ptr = std::make_unique< io::filtering_istream >();

    if (opts.have("deflate") && dict.has("/Filter")) {
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

static auto make_filtering_ostream(const options_t &opts)
{
    auto ptr = std::make_unique< io::filtering_ostream >();

    if (opts.have("hex"))
        ptr->push(example::asciihex_output_filter_t());

    ptr->push(boost::ref(std::cout));

    return ptr;
}

template< typename Xs >
void run_with(const options_t &opts, Xs &&xs)
{
    auto out = make_filtering_ostream(opts);

    auto copy = [](auto &&src, auto &&dst) {
        for (int c; EOF != (c = src.get()); dst.put(char(c))) { }
    };

    for (const auto &x : xs) {
        std::cout << x.xref.ref() << std::endl;

        const auto &arr = as< array_t >(x.obj);
        ASSERT(arr.size());

        if (is< dict_t >(arr[0])) {
            const auto &dict = as< dict_t >(arr[0]);
            std::cout << dict << std::endl;

            if (1 < arr.size()) {
                auto in = make_filtering_istream(opts, dict);

                const auto &stream = as< stream_t >(arr[1]);
                in->push(io::array_source(stream.data(), stream.size()));

                copy(*in, *out);
                out->flush();
            }
        } else {
            std::cout << arr[0] << std::endl;
        }

        std::cout << std::endl;
    }
}

static auto by_type(name_t what) {
    return views::filter([=](const iobj_t &iobj) {
        try {
            const auto &dict = as< dict_t >(as< array_t >(iobj.obj)[0]);
            return what == as< name_t >(dict.at("/Type"));
        } catch(...) {
            return false;
        }
    });
};

template< typename Xs >
void run_with_type_filter(const options_t &opts, Xs &&xs)
{
    run_with(opts, xs | by_type(opts["type"].as< std::string >()));
}

static inline auto by_ref(int what)
{
    return views::filter([=](const iobj_t &iobj) {
        return as< basic_xref_t >(iobj.xref).ref.num == what;
    });
};

template< typename Xs >
void run_with_ref_filter(const options_t &opts, Xs &&xs)
{
    auto ys = xs | by_ref(opts["ref"].as< int >());

    if (opts.have("type")) {
        run_with_type_filter(opts, ys);
    } else {
        run_with(opts, ys);
    }
}

static std::vector< iobj_t >
iobjs_from(const std::string &filename)
{
    io::mapped_file_source src(filename.c_str());

    auto iter = src.data(), end = iter;
    std::advance(end, src.size());

    std::vector< iobj_t > iobjs;

    if (!ypdf::parse(iter, iter, end, iobjs))
        throw std::runtime_error("malformed document");

    return iobjs;
}

static void run_with_iobjs_from(const options_t &opts)
{
    auto xs = iobjs_from(opts["input"].as< std::string >());

    if (opts.have("ref")) {
        run_with_ref_filter(opts, xs);
    } else if (opts.have("type")) {
        run_with_type_filter(opts, xs);
    } else {
        run_with(opts, xs);
    }
}

static void program_options_from(int &argc, char **argv)
{
    bool complete_invocation = false;

    options_t program_options(argc, argv);

    if (program_options.have("version")) {
        std::cout << "ypdf v0.1\n";
        complete_invocation = true;
    }

    if (program_options.have("help")) {
        std::cout << program_options.description() << std::endl;
        complete_invocation = true;
    }

    if (complete_invocation)
        exit(0);

    global_options(program_options);
}

int main(int argc, char **argv)
{
    program_options_from(argc, argv);

    try {
        run_with_iobjs_from(global_options());
    } catch (const std::exception &x) {
        std::cerr << x.what() << std::endl;
        return 1;
    }

    return 0;
}
