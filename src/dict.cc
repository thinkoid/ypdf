// -*- mode: c++; -*-

#include <ypdf/parser/ast/dict.hh>
#include <ypdf/parser/ast/name.hh>
#include <ypdf/parser/ast/obj.hh>

#include <stdexcept>

namespace ypdf::parser::ast {

dict_t::iterator dict_t::find(const char *k)
{
    iterator it = begin();
    for (; it != end() && std::get< 0 >(*it).compare(k); ++it)
        ;
    return it;
}

dict_t::const_iterator dict_t::find(const char *k) const
{
    const_iterator it = begin();
    for (; it != end() && std::get< 0 >(*it).compare(k); ++it)
        ;
    return it;
}

bool dict_t::has(const char *s) const
{
    return end() != find(s);
}

bool dict_t::has(const name_t &k) const
{
    return end() != find(k);
}

obj_t &dict_t::operator[](name_t &&k)
{
    iterator it = find(k);

    if (it == end()) {
        emplace_back(std::move(k), obj_t());
        it = --end();
    }

    return std::get< 1 >(*it);
}

obj_t &dict_t::operator[](const name_t &k)
{
    iterator it = find(k);

    if (it == end()) {
        emplace_back(k, obj_t());
        it = --end();
    }

    return std::get< 1 >(*it);
}

obj_t &dict_t::at(const name_t &k)
{
    iterator it = find(k);

    if (it == end())
        throw std::out_of_range("dict_t::at");

    return std::get< 1 >(*it);
}

const obj_t &dict_t::at(const name_t &k) const
{
    const_iterator it = find(k);

    if (it == end())
        throw std::out_of_range("dict_t::at");

    return std::get< 1 >(*it);
}

} // namespace ypdf::parser::ast
