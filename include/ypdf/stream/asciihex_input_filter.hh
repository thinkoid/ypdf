// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_ASCIIHEX_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_ASCIIHEX_INPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <cctype>
#include <boost/iostreams/concepts.hpp>

namespace ypdf {
namespace iostreams {

struct asciihex_input_filter_t : public boost::iostreams::input_filter
{
    template< typename Source > int get(Source &src)
    {
        if (eof_)
            return EOF;

        namespace bios = boost::iostreams;

        int c;
        while (EOF != (c = bios::get(src)) && std::isspace(c))
            ;

        int result = 0;

        switch (c) {
        case '>':
            return eof_ = true, EOF;
            break;

        case '0':
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            result = ((unsigned)(c - '0') & 0x0F) << 4;
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            result = ((unsigned)(c - 'a' + 10) & 0x0F) << 4;
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            result = ((unsigned)(c - 'A' + 10) & 0x0F) << 4;
            break;

        case EOF:
            throw std::ios_base::failure("ASCIIHex : unexpected EOF");

        default:
            throw std::ios_base::failure("ASCIIHex : invalid character");
        }

        while (EOF != (c = bios::get(src)) && std::isspace(c))
            ;

        switch (c) {
        case '>':
        case '0':
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            result |= (unsigned)(c - '0') & 0x0F;
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            result |= (unsigned)(c - 'a' + 10) & 0x0F;
            break;

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            result |= (unsigned)(c - 'A' + 10) & 0x0F;
            break;

        case EOF:
            throw std::ios_base::failure("ASCIIHex : unexpected EOF");

        default:
            throw std::ios_base::failure("ASCIIHex : invalid character");
        }

        return result;
    }

    template< typename Source > void close(Source &) { eof_ = false; }

private:
    bool eof_ = false;
};

} // namespace iostreams
} // namespace ypdf

#endif // YPDF_IOSTREAMS_ASCIIHEX_INPUT_FILTER_HH
