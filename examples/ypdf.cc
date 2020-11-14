// -*- mode: c++; -*-

#include <ypdf/ypdf.hh>
using namespace ypdf::parser::ast;

#include "io.hh"
#include "options.hh"

#include <iostream>

#include <filesystem>
namespace fs = std::filesystem;

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
namespace io = ::boost::iostreams;

#include <range/v3/view/any_view.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
using namespace ranges;

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

static std::vector< iobj_t >
iobjs_from(const std::string &filename)
{
    ::io::mapped_file_source f(filename.c_str());
    auto iter = f.data();

    std::vector< iobj_t > iobjs;

    if (!ypdf::parse(iter, iter, iter + f.size(), iobjs))
        throw std::runtime_error("parse failed");

    return iobjs;
}

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

static auto make_filtering_stream(const options_t &opts)
{
    auto ptr = std::make_unique< ::io::filtering_ostream >();

    if (opts.have("hex"))
        ptr->push(asciihex_output_filter_t());

    ptr->push(boost::ref(std::cout));

    return ptr;
}

template< typename Xs >
void run_with(const options_t &opts, Xs &&xs)
{
    auto ptr = make_filtering_stream(opts);
    auto &filtering_stream = *ptr;

    for (const auto &x : xs) {
        std::cout << x.xref.ref() << "\n";

        const auto &arr = as< array_t >(x.obj);
        ASSERT(arr.size());

        std::cout << arr[0] << "\n";

        if (1 < arr.size()) {
            const auto &str = as< stream_t >(arr[1]);
            filtering_stream << str << "\n";
        }

        std::cout << "\n";
    }
}

auto by_ref = [](int what) {
    return views::filter([=](const iobj_t &iobj) {
        const auto &xref = as< basic_xref_t >(iobj.xref);
        return xref.ref.num == what;
    });
};
template< typename Xs >
void run_with_ref(const options_t &opts, Xs &&xs)
{
    if (opts.have("ref")) {
        run_with(opts, xs | by_ref(opts["ref"].as< int >()));
    } else {
        run_with(opts, xs);
    }
}

auto dict_of = [](const auto &iobj) {
    return as< dict_t >(as< array_t >(iobj.obj)[0]);
};

auto by_type = [](name_t what) {
    return views::filter([=](const iobj_t &iobj) {
        const auto &dict = dict_of(iobj);
        return dict.has("/Type") && what == as< name_t >(dict.at("/Type"));
    });
};

template< typename Xs >
void run_with_type(const options_t &opts, Xs &&xs)
{
    if (opts.have("type")) {
        auto what = opts["type"].as< std::string >();
        run_with_ref(opts, xs | by_type(what));
    } else {
        run_with_ref(opts, xs);
    }
}

static void run_with(const options_t &opts)
{
    auto iobjs = iobjs_from(opts["input"].as< std::string >());
    run_with_type(opts, subrange(iobjs));
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
        return run_with(global_options()), 0;
    } catch (const std::exception &x) {
        std::cerr << x.what() << std::endl;
        return 1;
    }
}
