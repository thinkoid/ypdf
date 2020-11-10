// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_IOBJ_HH
#define YPDF_PARSER_AST_IOBJ_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast/obj.hh>
#include <ypdf/parser/ast/ref.hh>

namespace ypdf::parser::ast {

struct iobj_t
{
    ref_t ref;
    obj_t obj;
};

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_IOBJ_HH
