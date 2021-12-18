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

template< typename T > inline auto to_array(const ast::obj_t &obj)
{
    using namespace ranges;

    std::vector< T > xs;

    if (is< ast::array_t >(obj)) {
        auto rng = as< ast::array_t >(obj)
            | views::transform([](auto &x) { return as< T >(x); });
        copy(rng, ranges::back_inserter(xs));
    } else if (is< T >(obj)) {
        xs.push_back(as< T >(obj));
    } else {
        ASSERT(0);
    }

    return xs;
};

} // namespace example

static auto
make_filtering_istream(const options_t &opts, const ast::dict_t &dict)
{
    namespace io_ = ypdf::iostreams;

    auto ptr = std::make_unique< io::filtering_istream >();

    if (opts.have("deflate") && dict.has("/Filter")) {
        for (auto &filter : example::to_array< ast::name_t >(dict.at("/Filter"))) {
            if (filter == "/FlateDecode") {
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

static auto
make_filtering_ostream(const options_t &opts)
{
    auto ptr = std::make_unique< io::filtering_ostream >();

    if (opts.have("hex"))
        ptr->push(example::asciihex_output_filter_t());

    ptr->push(boost::ref(std::cout));

    return ptr;
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

template< typename Xs >
inline void do_run_with(const options_t &opts, Xs &&xs)
{
    auto out = make_filtering_ostream(opts);

    for (const auto &x : xs) {
        std::cout << x.xref;

        const auto &arr = as< ast::array_t >(x.obj);
        ASSERT(arr.size());

        const auto &dict = as< ast::dict_t >(arr[0]);
        std::cout << dict << std::endl;

        if (1 < arr.size()) {
            auto in = make_filtering_istream(opts, dict);

            const auto &stream = as< ast::stream_t >(arr[1]);
            in->push(io::array_source(stream.data(), stream.size()));

            for (int c; EOF != (c = in->get()); ) {
                ASSERT(0 <= c && c <= 256);
                out->put(char(c));
            }

            out->flush();
        }

        std::cout << std::endl;
    }
}

ast::iobj_t
fetch(const ast::basic_xref_t &xref, ctx_t &ctx)
{
    using ypdf::parser::iobj;
    
    auto first = ctx.source()->data();
    std::advance(first, xref.off);

    auto iter = first, end = first;
    std::advance(end, ctx.source()->size());
    
    ast::iobj_t x;

    if (iobj(first, iter, end, x))
        return x;

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

static ast::iobj_t
fetch(const ast::stream_xref_t &xref, ctx_t &ctx)
{
    if (auto ptr = ::find(ctx.catalog(), xref.stream)) {
        return fetch(*ptr, ctx);
    }

    return { };
}

static ast::iobj_t
fetch(const ast::xref_t &xref, ctx_t &ctx)
{
    using result_type = ast::iobj_t;

    auto visitor = hana::overload(
        [&](const ast::free_xref_t &) { return result_type{ }; },
        [&](const auto &arg) { return fetch(arg, ctx); });

    return boost::apply_visitor(visitor, xref);
}

static const ast::dict_t *
dict_of(const ast::iobj_t &iobj)
{
    if (is< ast::array_t >(iobj.obj)) {
        auto &arr = as< ast::array_t >(iobj.obj);

        if (arr.size() > 1) {
            return &as< ast::dict_t >(arr[0]);
        }
    }

    return nullptr;
}

auto by_ref = [](int arg) {
    return [=](const ast::xref_t &xref) { return xref.ref().num == arg; };
};

auto by_type = [](std::string arg) {
    return [type = std::move(arg)](const ast::iobj_t &iobj) {
        const ast::dict_t *p = dict_of(iobj);            
        return p && p->has("/Type") && ast::as< ast::name_t >(p->at("/Type")) == type;
    };
};

static void run_with(const options_t &opts)
{
    ctx_t ctx(opts["input"].as< std::string >());

    auto xs = ctx.xrefs() | views::filter(
        [](const auto &arg) { return !ast::is< ast::free_xref_t >(arg); });

    auto resolve = [&](const ast::xref_t &xref) { return fetch(xref, ctx); };

    if (opts.have("ref")) {
        auto xs0 = xs
            | views::filter(by_ref(opts["ref"].as< int >()))
            | views::transform(resolve);
        
        if (opts.have("type")) {
            do_run_with(opts, xs0 | views::filter(by_type(opts["type"].as< std::string >())));
        } else {
            do_run_with(opts, xs0);
        }
    } else if (opts.have("type")) {
        do_run_with(opts, xs
            | views::transform(resolve)
            | views::filter(by_type(opts["type"].as< std::string >())));
    } else {
        do_run_with(opts, xs | views::transform(resolve));
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
