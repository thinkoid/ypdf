// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_STRING_HH
#define YPDF_PARSER_AST_STRING_HH

#include <ypdf/detail/defs.hh>

#include <string>

namespace ypdf::parser::ast {

struct string_t : std::string
{
    using base_type = std::string;

    using base_type::base_type;
    using base_type::operator=;

    using base_type::operator[];

    string_t(const std::string &other) : base_type(other) { }
    string_t(std::string &&other) noexcept : base_type(std::move(other)) { }
};

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_STRING_HH
