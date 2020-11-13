// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_LZW_FILTER_BASE_HH
#define YPDF_IOSTREAMS_LZW_FILTER_BASE_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/stream/lzw_filter_base.hh>

namespace ypdf::iostreams {

struct lzw_filter_base_t
{
    using parameter_type = size_t;

    constexpr static parameter_type min_bits = 9;
    constexpr static parameter_type max_bits = 12;

    using code_type = size_t;

    constexpr static code_type clear_code = 256;
    constexpr static code_type eod_code = 257;
    constexpr static code_type first_code = 258;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_FILTER_BASE_HH
