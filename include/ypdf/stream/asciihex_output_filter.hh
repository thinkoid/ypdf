// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_ASCIIHEX_OUTPUT_FILTER_HH
#define YPDF_IOSTREAMS_ASCIIHEX_OUTPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <cctype>
#include <boost/iostreams/concepts.hpp>

namespace ypdf {
namespace iostreams {

struct asciihex_output_filter_t : public boost::iostreams::output_filter
{
    template< typename Sink > bool put(Sink &dst, char c)
    {
        static const char *s = "0123456789ABCDEF";

        if (eof_)
            return false;

        const auto u = static_cast< unsigned char >(c);

        return
            ::boost::iostreams::put(dst, s[(u & 0xF0) >> 4]) &&
            ::boost::iostreams::put(dst, s[ u & 0x0F]);
    }

    template< typename Sink > void close(Sink &dst)
    {
        boost::iostreams::put(dst, '>');
        eof_ = true;
    }

private:
    bool eof_ = false;
};

} // namespace iostreams
} // namespace ypdf

#endif // YPDF_IOSTREAMS_ASCIIHEX_OUTPUT_FILTER_HH
