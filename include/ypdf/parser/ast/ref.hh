// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_PARSER_AST_REF_HH
#define YPDF_PARSER_AST_REF_HH

#include <ypdf/detail/defs.hh>

namespace ypdf::parser::ast {

struct ref_t
{
    int num, gen;
};

inline bool operator==(const ref_t &lhs, const ref_t &rhs)
{
    return lhs.num == rhs.num && lhs.gen == rhs.gen;
}

inline bool operator!=(const ref_t &lhs, const ref_t &rhs)
{
    return !(lhs == rhs);
}

} // namespace ypdf::parser::ast

#endif // YPDF_PARSER_AST_REF_HH
