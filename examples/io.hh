// -*- mode: c++; -*-
// Copyright 2020 Thinkoid, LLC

#ifndef YPDF_EXAMPLES_IO_HH
#define YPDF_EXAMPLES_IO_HH

#include <ypdf/detail/defs.hh>

#include <ypdf/parser/ast/array.hh>
#include <ypdf/parser/ast/dict.hh>
#include <ypdf/parser/ast/iobj.hh>
#include <ypdf/parser/ast/name.hh>
#include <ypdf/parser/ast/null.hh>
#include <ypdf/parser/ast/obj.hh>
#include <ypdf/parser/ast/ref.hh>
#include <ypdf/parser/ast/stream.hh>
#include <ypdf/parser/ast/string.hh>
#include <ypdf/parser/ast/xref.hh>

#include <iostream>

#include <boost/variant.hpp>

namespace ypdf::parser::ast {

inline std::ostream &operator<<(std::ostream &s, const null_t &null)
{
    return s << "(null)";
}

inline std::ostream &operator<<(std::ostream &s, const ref_t &x)
{
    return s << x.num << ' ' << x.gen << " R";
}

inline std::ostream &operator<<(std::ostream &s, const free_xref_t &arg)
{
    return s << arg.ref;
}

inline std::ostream &operator<<(std::ostream &s, const basic_xref_t &arg)
{
    return s << arg.ref << " : " << arg.off;
}

inline std::ostream &operator<<(std::ostream &s, const stream_xref_t &arg)
{
    return s << arg.ref << ", " << arg.stream << " : " << arg.pos;
}

inline std::ostream &operator<<(std::ostream &s, const xref_t &arg)
{
    boost::apply_visitor([&s](const auto &x) { s << x; }, arg);
    return s;
}

inline std::ostream &operator<<(std::ostream &s, const stream_t &xs)
{
    return s.write(xs.data(), xs.size());
}

std::ostream &operator<<(std::ostream &s, const array_t &);
std::ostream &operator<<(std::ostream &s, const dict_t &);

inline std::ostream &operator<<(std::ostream &s, const obj_t &obj)
{
    boost::apply_visitor([&s](const auto &x) { s << x; }, obj);
    return s;
}

inline std::ostream &operator<<(std::ostream &s, const array_t &arr)
{
    s << "[";
    for (const auto &obj : arr)
        s << " " << obj;
    return s << " ]";
}

inline std::ostream &operator<<(std::ostream &s, const dict_t &dict)
{
    s << "<<";
    for (auto &&[key, val] : dict)
        s << " " << key << " " << val;
    return s << " >>";
}

inline std::ostream &operator<<(std::ostream &s, const iobj_t &iobj)
{
    return s << iobj.xref.ref() << " 0\n" << iobj.obj;
}

} // namespace ypdf::parser::ast

#endif // YPDF_EXAMPLES_IO_HH
