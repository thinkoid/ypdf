// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_ASCII85_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_ASCII85_INPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <boost/endian/conversion.hpp>
namespace endian = boost::endian;

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/read.hpp>

namespace ypdf::iostreams {

struct ascii85_input_filter_t : public boost::iostreams::input_filter
{
    template< typename Source > int get(Source &src)
    {
        if (eof_)
            return EOF;

        if (i_ > 3) {
            i_ = 0;
            underflow(src);
        }

        return (eof_ = EOF == b_[i_]) ? EOF : b_[i_++];
    }

    template< typename Source > void close(Source &) { eof_ = false; }

private:
    template< typename Source > int do_get(Source &src)
    {
        namespace bios = boost::iostreams;

        static const char arr[256] = {
            //  1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, // 0
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
            1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 2
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 3
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 4
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 5
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 6
            2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, // 7
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // F
        };

        for (int c; EOF != (c = bios::get(src));) {
            ASSERT(0 <= c && c < 256);

            if (0 == arr[c])
                throw std::runtime_error("invalid character");
            else if (2 == arr[c])
                return c;
        }

        return EOF;
    }

    template< typename Source > void fill(Source &src, int *p, int *const pend)
    {
        switch (*p = do_get(src)) {
        case '~':
        case EOF:
            std::fill(p, pend, EOF);
            return;

        case 'z':
            std::fill(p, pend, 33);
            return;

        default:
            break;
        }

        for (++p; p != pend; ++p) {
            switch (*p = do_get(src)) {
            case '~':
            case EOF:
                std::fill(p, pend, EOF);
                return;

            case 'z':
                throw std::runtime_error("invalid `z' in mid-block");

            default:
                break;
            }
        }
    }

    static void ascii85_decode(const int *src, const int *esrc, int *dst,
                               const int *edst)
    {
        unsigned x = 0;

        for (auto p = src; p != esrc; ++p)
            x = x * 85 + (*p - 33);

        endian::big_to_native_inplace(x);

        for (auto p = (const char *)(&x); dst != edst; ++p, ++dst)
            *dst = (unsigned char)*p;
    }

    template< typename Source > void underflow(Source &src)
    {
        int c[5];
        fill(src, c, c + 5);

        size_t i = 0;
        for (; i < 5 && EOF != c[i]; ++i)
            ;

        ASSERT(1 != i);
        size_t n = i > 1 ? i - 1 : 0;

        for (; i < 5; ++i) {
            ASSERT(EOF == c[i]);
            c[i] = 'u';
        }

        ascii85_decode(c, c + sizeof c / sizeof *c, b_,
                       b_ + sizeof b_ / sizeof *b_);

        for (i = n; i < 4; ++i)
            b_[i] = EOF;

        i_ = 0;
    }

private:
    size_t i_ = 4;
    int    b_[4] = { 0 };
    bool   eof_ = false;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_ASCII85_INPUT_FILTER_HH
