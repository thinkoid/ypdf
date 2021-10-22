// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#include <ypdf/parser/xrefstm.hh>
#include <ypdf/stream.hh>

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/transform.hpp>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace ypdf::parser {
namespace detail {

template< typename Rng, size_t... I >
auto to_tuple_fn(Rng &&rng, std::index_sequence< I... >)
{
    return std::make_tuple(rng[I]...);
}

template< typename T, size_t N > struct homogenous_tuple_t
{
    template< typename = std::make_index_sequence< N > > struct impl;

    template< size_t... Is > struct impl< std::index_sequence< Is... > >
    {
        template< size_t > using wrap = T;
        using type = std::tuple< wrap< Is >... >;
    };

public:
    using type = typename impl<>::type;
};

template< typename T, size_t N > inline auto to_tuple_array(const ast::obj_t &obj)
{
    using namespace ranges;

    using tuple_type = typename homogenous_tuple_t< T, N >::type;
    using result_type = std::vector< tuple_type >;

    auto to_type = [](const auto &x) { return ast::as< T >(x); };

    auto to_tuple = [](const auto &rng) -> tuple_type {
        return to_tuple_fn(rng, std::make_index_sequence< N >{});
    };

    const auto rng = as< ast::array_t >(obj) | views::transform(to_type) |
                     views::chunk(N) | views::transform(to_tuple);

    return rng | to< result_type >();
}

template< typename T > inline auto to_array(const ast::obj_t &obj)
{
    using namespace ranges;

    std::vector< T > xs;

    if (is< ast::array_t >(obj)) {
        copy(as< ast::array_t >(obj) |
                 views::transform([](const auto &x) { return ast::as< T >(x); }),
             ranges::back_inserter(xs));
    } else if (ast::is< T >(obj)) {
        xs.push_back(ast::as< T >(obj));
    } else {
        ASSERT(0);
    }

    return xs;
};

inline auto make_filtering_istream(const ast::dict_t &dict)
{
    using namespace ranges;
    namespace io = ::boost::iostreams;

    auto ptr = std::make_unique< io::filtering_istream >();

    if (dict.has("/Filter")) {
        for (const auto &filter : to_array< ast::name_t >(dict.at("/Filter"))) {
            if (filter == "/FlateDecode") {
                ptr->push(io::zlib_decompressor());
            } else if (filter == "/ASCII85Decode") {
                ptr->push(iostreams::ascii85_input_filter_t());
            } else if (filter == "/LZWDecode") {
                ptr->push(iostreams::lzw_input_filter_t());
            }
        }
    }

    return ptr;
}

inline auto xrefstm_W(const ast::dict_t &dict) -> std::tuple< int, int, int >
{
    return to_tuple_array< int, 3 >(dict.at("/W")).front();
}

inline auto xrefstm_Index(const ast::dict_t &dict, size_t default_size)
    -> std::vector< std::tuple< int, int > >
{
    return dict.has("/Index") ?
               to_tuple_array< int, 2 >(dict.at("/Index")) :
               std::vector< std::tuple< int, int > >{ { 0, default_size } };
}

inline int xrefstm_get(::boost::iostreams::filtering_istream &stream, int w)
{
    unsigned n = 0;

    for (int c; w && EOF != (c = stream.get()); --w)
        n = (n << 8) | ((unsigned)c & 0xff);

    if (w)
        throw std::runtime_error("unexpected xrefstm end");

    return n;
}

inline auto xrefstm_block(::boost::iostreams::filtering_istream &stream,
                          int start, int len, int a, int b, int c)
{
    ASSERT(b);

    std::vector< ast::xref_t > xrefs_;

    for (int i = start; len; ++i, --len) {
        int x = 1, y = 0, z = 0;

        if (a)
            x = xrefstm_get(stream, a);

        switch (x) {
        case 0:
            y = xrefstm_get(stream, b);

            if (c)
                z = xrefstm_get(stream, c);

            xrefs_.emplace_back(ast::free_xref_t{ i, z });
            break;

        case 1:
            y = xrefstm_get(stream, b);

            if (c)
                z = xrefstm_get(stream, c);

            xrefs_.emplace_back(ast::basic_xref_t{ { i, z }, y });
            break;

        case 2:
            ASSERT(b);
            y = xrefstm_get(stream, b);

            if (c)
                z = xrefstm_get(stream, c);

            xrefs_.emplace_back(
                ast::stream_xref_t{ { i, 0 }, { y, 0 }, size_t(z) });
            break;

        default:
            ASSERT(0);
        }
    }

    return xrefs_;
}

inline auto xrefstm_xrefs(const ast::dict_t &dict, const ast::stream_t &stream)
{
    std::vector< ast::xref_t > xrefs_;

    const auto &name = as< ast::name_t >(dict.at("/Type"));
    ASSERT(name == "/XRef");

    const int size = as< int >(dict.at("/Size"));
    ASSERT(0 < size);

    const auto [a, b, c] = xrefstm_W(dict);
    const auto index = xrefstm_Index(dict, size);

    {
        namespace io = boost::iostreams;

        auto ptr = make_filtering_istream(dict);
        ptr->push(io::array_source(stream.data(), stream.size()));

        for (const auto& [start, len] : index) {
            const auto block = xrefstm_block(*ptr, start, len, a, b, c);
            xrefs_.insert(xrefs_.end(), block.begin(), block.end());
        }
    }

    return xrefs_;
}

} // namespace detail

template< typename Iterator >
bool xrefstm(Iterator first, Iterator &iter, Iterator last,
             std::vector< ast::xref_t > &attr)
{
    ast::iobj_t iobj_;

    if (!iobj(first, iter, last, iobj_))
        return false;

    ASSERT(2 == ast::as< ast::array_t >(iobj_.obj).size());

    const auto &dict = as< ast::dict_t >(iobj_.obj[0]);
    const auto &stm = as< ast::stream_t >(iobj_.obj[1]);

    attr = detail::xrefstm_xrefs(dict, stm);

    if (dict.has("/Prev")) {
        const auto off = as< int >(dict.at("/Prev"));
        std::advance((iter = first), off);
    } else {
        iter = last;
    }

    return true;
}

} // ypdf::parser
