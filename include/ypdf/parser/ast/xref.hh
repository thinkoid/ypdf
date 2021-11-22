// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_XREF_HH
#define YPDF_PARSER_AST_XREF_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/detail/overloaded.hh>

#include <ypdf/parser/ast/ref.hh>

#include <boost/variant.hpp>

namespace ypdf::parser {
namespace ast {

struct free_xref_t
{
    ref_t ref;
};

inline bool operator==(const free_xref_t &lhs, const free_xref_t &rhs)
{
    return lhs.ref == rhs.ref;
}

inline bool operator!=(const free_xref_t &lhs, const free_xref_t &rhs)
{
    return !(lhs == rhs);
}

struct basic_xref_t
{
    ref_t ref;
    off_t off;
};

inline bool operator==(const basic_xref_t &lhs, const basic_xref_t &rhs)
{
    return lhs.ref == rhs.ref && lhs.off == rhs.off;
}

inline bool operator!=(const basic_xref_t &lhs, const basic_xref_t &rhs)
{
    return !(lhs == rhs);
}

struct stream_xref_t
{
    ref_t  ref, stream;
    size_t pos;
};

inline bool operator==(const stream_xref_t &lhs, const stream_xref_t &rhs)
{
    return lhs.ref == rhs.ref && lhs.stream == rhs.stream && lhs.pos == rhs.pos;
}

inline bool operator!=(const stream_xref_t &lhs, const stream_xref_t &rhs)
{
    return !(lhs == rhs);
}

struct xref_t : boost::variant< free_xref_t, basic_xref_t, stream_xref_t >
{
    using base_type = boost::variant<
        free_xref_t, basic_xref_t, stream_xref_t
        >;

    const ref_t &ref() const
    {
        return boost::apply_visitor(
            detail::overloaded_{
                [](const auto &arg) -> const ref_t & { return arg.ref; } },
            *this);
    }
};

inline bool operator==(const xref_t &lhs, const xref_t &rhs)
{
    return
        static_cast< const xref_t::base_type & >(lhs) ==
        static_cast< const xref_t::base_type & >(rhs);
}

inline bool operator!=(const xref_t &lhs, const xref_t &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const xref_t &lhs, const ref_t &rhs)
{
    return lhs.ref() == rhs;
}

inline bool operator!=(const xref_t &lhs, const ref_t &rhs)
{
    return !(lhs == rhs);
}

inline bool operator==(const ref_t &lhs, const xref_t &rhs)
{
    return lhs == rhs.ref();
}

inline bool operator!=(const ref_t &lhs, const xref_t &rhs)
{
    return !(lhs == rhs);
}

template< typename T > inline const T &as(const xref_t &arg)
{
    return boost::get< T >(arg);
}

template< typename T > inline bool is(const xref_t &arg)
{
    using value_type = std::remove_const_t< std::remove_pointer_t< T > >;
    return typeid(value_type) == arg.type();
}

} // namespace ast
} // namespace ypdf::parser

#endif // YPDF_PARSER_AST_XREF_HH
