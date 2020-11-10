// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_STREAM_HH
#define YPDF_PARSER_AST_STREAM_HH

#include <ypdf/detail/defs.hh>

#include <vector>

namespace ypdf::parser::ast {

struct stream_t : std::vector< char >
{
    using base_type = std::vector< char >;

    using base_type::base_type;
    using base_type::operator=;

    using base_type::operator[];

    using base_type::begin;
    using base_type::end;
};

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_STREAM_HH
