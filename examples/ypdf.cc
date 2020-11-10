// -*- mode: c++; -*-

#include <ypdf/ypdf.hh>
using namespace ypdf::parser::ast;

#include "io.hh"
#include "options.hh"

#include <iostream>

#include <filesystem>
namespace fs = std::filesystem;

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
void run_with(const options_t &opts, Xs &&xs)
{
    for (const auto &x : xs) {
        std::cout << x.ref << "\n";

        const auto &arr = as< array_t >(x.obj);
        ASSERT(arr.size());

        std::cout << arr[0] << std::endl;

        if (1 < arr.size()) {
            const auto &stream = as< stream_t >(arr[1]);

            if (opts.have("hex")) {
                ::ypdf::detail::ostream_guard_t guard(std::cout);

                std::cout << std::hex << std::setw(2) << std::setfill('0');

                for (auto [c, i] : views::zip(stream, views::iota(0))) {
                    if (i && 0 == (i % 16))
                        std::cout << "\n";

                    std::cout << std::setw(2) << int((unsigned char)c) << ' ';
                }
            } else {
                std::cout << stream;
            }

            std::cout << "\n";
        }

        std::cout << "\n";
    }
}

static void
run_with(const options_t &opts)
{
    auto iobjs = iobjs_from(opts["input"].as< std::string >());
    auto xs = subrange(iobjs);

    if (opts.have("type")) {
        auto what = opts["type"].as< std::string >();
        run_with(opts, xs | by_type(what));
    } else {
        run_with(opts, xs);
    }
}

static void
program_options_from(int &argc, char **argv) {
    bool complete_invocation = false;

    options_t program_options(argc, argv);

    if (program_options.have("version")) {
        std::cout << "ypdf v0.1\n";
        complete_invocation = true;
    }

    if (program_options.have("help")) {
        std::cout << program_options.description () << std::endl;
        complete_invocation = true;
    }

    if (complete_invocation)
        exit (0);

    global_options(program_options);
}


int main(int argc, char **argv)
{
    program_options_from(argc, argv);

    try {
        return run_with(global_options()), 0;
    } catch (const std::exception  &x) {
        std::cerr << x.what() << std::endl;
        return 1;
    }
}
