// -*- mode: c++; -*-

#include <type_traits>
#include <ypdf/ypdf.hh>

#include "ypdf/parser/ast/xref.hh"
namespace ast = ypdf::parser::ast;

#include "io.hh"
#include "options.hh"

#include <algorithm>
#include <iostream>
#include <optional>
#include <tuple>

#include <filesystem>
namespace fs = std::filesystem;

#include <boost/none.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
namespace io = ::boost::iostreams;

#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/filter.hpp>
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

struct ctx_t
{
    explicit ctx_t(const fs::path &path)
        : source_(path), xrefs_(xrefs_from(source_))
    { }

    auto &source() {
        return source_;
    }

    const auto &catalog() const {
        return xrefs_;
    }

    const auto &xrefs() const {
        return xrefs_;
    }

private:
    static std::vector< ast::xref_t >
    xrefs_from(io::stream< io::mapped_file_source > &source)
    {
        auto iter = source->data(), end = iter;
        std::advance(end, source->size());

        std::vector< ast::xref_t > xs;

        if (!ypdf::parse(iter, iter, end, xs))
            throw std::runtime_error("xrefs parsing failed");

        return xs;
    }

private:
    io::stream< io::mapped_file_source > source_;
    std::vector< ast::xref_t > xrefs_;
};

template< typename Iterator >
bool parse(Iterator first, Iterator &iter, Iterator last,
           ast::dict_t &attr)
{
    using ypdf::parser::comment;
    using ypdf::parser::dictionary;
    using ypdf::parser::int_;
    using ypdf::parser::lit;
    using ypdf::parser::lookahead;
    using ypdf::parser::skipws;
    
    int num;
    if (int_(first, iter, last, num)) {
        skipws(first, iter, last);

        int gen = 0;
        if (int_(first, iter, last, gen)) {
            skipws(first, iter, last);

            if (lit(first, iter, last, "obj")) {
                skipws(first, iter, last);
                comment(first, iter, last);

                if (lookahead(iter, last, "<<")) {
                    ast::dict_t dict;

                    if (dictionary(first, iter, last, dict))
                        return attr = std::move(dict), true;
                }
            }
        }
    }
    
    return false;
}

std::optional< ast::dict_t >
maybe_dict_from(const ast::basic_xref_t &xref, ctx_t &ctx)
{
    auto first = ctx.source()->data();
    std::advance(first, xref.off);

    auto iter = first, end = first;
    std::advance(end, ctx.source()->size());
    
    ast::dict_t dict;

    if (parse(first, iter, end, dict))
        return dict;

    return { };
}

static const ast::basic_xref_t *
find(const std::vector< ast::xref_t > &xs, const ast::ref_t &ref)
{
    using ranges::find_if;
    using ranges::end;

    auto iter = find_if(xs, [&](auto &x) { return x == ref; });
    return iter == end(xs) ? nullptr : &as< ast::basic_xref_t >(*iter);
}

static std::optional< ast::dict_t >
maybe_dict_from(const ast::stream_xref_t &xref, ctx_t &ctx)
{
    if (auto ptr = ::find(ctx.catalog(), xref.stream)) {
        return maybe_dict_from(*ptr, ctx);
    }

    return { };
}

static std::optional< ast::dict_t >
maybe_dict_from(const ast::xref_t &xref, ctx_t &ctx)
{
    using result_type = std::optional< ast::dict_t >;

    auto visitor = hana::overload(
        [&](const ast::free_xref_t &) { return result_type{ }; },
        [&](const auto &arg) { return maybe_dict_from(arg, ctx); });

    return boost::apply_visitor(visitor, xref);
}

auto by_ref = [](int arg) {
    return [=](const ast::xref_t &xref) { return xref.ref().num == arg; };
};

auto by_type = [](std::string arg) {
    return [type = std::move(arg)](const auto &arg) {
        const auto &[ref, opt] = arg;

        if (opt) {
            auto &dict = opt.value();
            return dict.has("/Type") && ast::as< ast::name_t >(dict.at("/Type")) == type;
        }

        return false;
    };
};

template< typename Xs >
inline void do_run_with(Xs &&xs)
{
    for (const auto &[xref, opt] : xs) {
        std::cout << xref;

        if (opt)
            std::cout << " : " << opt.value();

        std::cout << std::endl;
    }
}

static void run_with(const options_t &opts)
{
    ctx_t ctx(opts["input"].as< std::string >());

    auto xs = ctx.xrefs() | views::filter(
        [](const auto &arg) { return !ast::is< ast::free_xref_t >(arg); });

    auto to_tuple = [&](const ast::xref_t &xref) {
        return std::make_tuple(xref.ref(), maybe_dict_from(xref, ctx));
    };

    if (opts.have("ref")) {
        auto xs0 = xs
            | views::filter(by_ref(opts["ref"].as< int >()))
            | views::transform(to_tuple);
        
        if (opts.have("type")) {
            do_run_with(xs0 | views::filter(by_type(opts["type"].as< std::string >())));
        } else {
            do_run_with(xs0);
        }
    } else if (opts.have("type")) {
        do_run_with(xs
            | views::transform(to_tuple)
            | views::filter(by_type(opts["type"].as< std::string >())));
    } else {
        do_run_with(xs | views::transform(to_tuple));
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
        run_with(global_options());
    } catch (const std::exception &x) {
        std::cerr << x.what() << std::endl;
        return 1;
    }

    return 0;
}
