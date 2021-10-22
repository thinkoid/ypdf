// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_DICT_HH
#define YPDF_PARSER_AST_DICT_HH

#include <ypdf/detail/defs.hh>

#include <string>
#include <tuple>
#include <vector>

#include <ypdf/parser/ast/name.hh>
#include <ypdf/parser/ast/obj_fwd.hh>

namespace ypdf::parser::ast {

struct dict_t : std::vector< std::tuple< name_t, obj_t > >
{
    using base_type = std::vector< std::tuple< name_t, obj_t > >;

    using base_type::base_type;
    using base_type::operator=;

    using base_type::operator[];

    iterator       find(const char *);
    const_iterator find(const char *) const;

    iterator find(const name_t &k) { return find(k.c_str()); }

    const_iterator find(const name_t &k) const { return find(k.c_str()); }

    bool has(const char *) const;
    bool has(const name_t &) const;

    obj_t &operator[](const char *);

    obj_t &operator[](name_t &&);
    obj_t &operator[](const name_t &);

    obj_t &at(const char *);
    const obj_t &at(const char *) const;

    obj_t &      at(const name_t &);
    const obj_t &at(const name_t &) const;

    template< typename... Ts > void emplace_back(Ts &&...);

    template< typename... Ts > void emplace_back(const std::string &k, Ts &&...v)
    {
        emplace_back(name_t(k), v...);
    }

    template< typename... Ts > void emplace_back(std::string &&k, Ts &&...v)
    {
        emplace_back(name_t(std::move(k)), v...);
    }
};

} // namespace ypdf::parser::ast

#include <ypdf/parser/ast/dict.cc>

#endif // YPDF_PARSER_AST_DICT_HH
