// -*- mode: c++; -*-

#ifndef YPDF_DETAIL_OVERLOADED_HH
#define YPDF_DETAIL_OVERLOADED_HH

namespace ypdf::detail {

template< typename... Ts > struct overloaded_ : Ts...
{
    using Ts::operator()...;
};
template< typename... Ts > overloaded_(Ts...) -> overloaded_< Ts... >;

} // namespace ypdf::detail

#endif // YPDF_DETAIL_OVERLOADED_HH
