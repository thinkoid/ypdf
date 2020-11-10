// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_ARRAY_HH
#define YPDF_PARSER_AST_ARRAY_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/parser/ast/obj_fwd.hh>

#include <vector>

namespace ypdf::parser::ast {

using array_t = std::vector< obj_t >;

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_ARRAY_HH
