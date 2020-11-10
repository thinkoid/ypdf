// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_OBJ_HH
#define YPDF_PARSER_AST_OBJ_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast/array.hh>
#include <ypdf/parser/ast/dict.hh>
#include <ypdf/parser/ast/name.hh>
#include <ypdf/parser/ast/null.hh>
#include <ypdf/parser/ast/ref.hh>
#include <ypdf/parser/ast/stream.hh>
#include <ypdf/parser/ast/string.hh>
#include <ypdf/parser/ast/xref.hh>

#include <boost/variant.hpp>

namespace ypdf::parser::ast {

using obj_base_t = boost::variant<
    null_t, bool, int, double, ref_t, string_t, name_t, array_t, dict_t,
    stream_t >;

struct obj_t : obj_base_t
{
    using base_type = obj_base_t;

    using base_type::base_type;
    using base_type::operator=;

    const obj_t &operator[](size_t) const;

    const obj_t &at(const char *) const;
    const obj_t &at(const name_t &) const;
};

template< typename T >
inline const T &as(const obj_t &arg)
{
    return boost::get< T >(arg);
}

template< typename T >
inline bool is(const obj_t &arg)
{
    return typeid(T) == arg.type();
}

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_OBJ_HH
