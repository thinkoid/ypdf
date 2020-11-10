// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_NAME_HH
#define YPDF_PARSER_AST_NAME_HH

#include <ypdf/detail/defs.hh>

#include <string>

namespace ypdf::parser::ast {

struct name_t : std::string
{
    using base_type = std::string;

    using base_type::base_type;
    using base_type::operator=;

    using base_type::operator[];

    name_t(const std::string &other) : base_type(other) { }
    name_t(std::string &&other) noexcept : base_type(std::move(other)) { }
};

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_NAME_HH
