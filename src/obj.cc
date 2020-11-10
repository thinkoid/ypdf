// -*- mode: c++; -*-

#include <ypdf/parser/ast/obj.hh>

#include <stdexcept>

namespace ypdf::parser::ast {

const obj_t &obj_t::operator[](size_t n) const
{
    return as< array_t >(*this)[n];
}

const obj_t &obj_t::at(const char *s) const
{
    return as< dict_t >(*this).at(s);
}

const obj_t &obj_t::at(const name_t &k) const
{
    return as< dict_t >(*this).at(k);
}

} // namespace ypdf::parser::ast
