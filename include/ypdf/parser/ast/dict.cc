// -*- mode: c++; -*-

#include <ypdf/parser/ast/dict.hh>

namespace ypdf::parser::ast {

template< typename... Ts > void dict_t::emplace_back(Ts &&...args)
{
    base_type::emplace_back(std::forward< Ts >(args)...);
}

} // namespace ypdf::parser::ast
