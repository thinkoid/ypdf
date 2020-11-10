// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_ASCII85_OUTPUT_FILTER_HH
#define YPDF_IOSTREAMS_ASCII85_OUTPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <cctype>

#include <boost/endian/conversion.hpp>
namespace endian = boost::endian;

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/write.hpp>

namespace ypdf::iostreams {

struct ascii85_output_filter_t : public boost::iostreams::output_filter
{
    template< typename Sink >
    bool put(Sink &dst, char c)
    {
        return !eof_ && do_put(dst, c);
    }

    template< typename Sink >
    void close(Sink &dst)
    {
        flush(dst);
        eof_ = true;
    }

private:
    template< typename Sink >
    bool endl(Sink &dst)
    {
        namespace bios = boost::iostreams;
        return n_ < 72 || (n_ = 0, 1 == bios::put(dst, '\n'));
    }

    template< typename Sink >
    std::streamsize write(Sink &dst, const char *s, std::streamsize n)
    {
        namespace bios = boost::iostreams;
        return n_ += n, bios::write(dst, s, n);
    }

    template< typename Sink >
    std::streamsize write(Sink &dst, char c)
    {
        namespace bios = boost::iostreams;
        return ++n_, bios::put(dst, c);
    }

    static void ascii85_encode(const char *src, char *dst)
    {
        unsigned x = *reinterpret_cast< const unsigned * >(src);
        endian::big_to_native_inplace(x);

        for (size_t i = 0; i < 5; ++i, x /= 85)
            dst[i] = (char)(x % 85 + 33);

        std::reverse(dst, dst + 5);
    }

    template< typename Sink >
    bool do_overflow(Sink &dst)
    {
        static int zero[] = { 0, 0, 0, 0 };

        switch (i_) {
        case 0:
            return true;

        case 1:
        case 2:
        case 3:
            return ascii85_encode(b_, c_), i_ + 1 == write(dst, c_, i_ + 1);

        case 4:
            if (0 == memcmp(b_, zero, sizeof zero)) {
                const auto b = 1 == write(dst, 'z');
                return i_ = 0, b;
            } else {
                ascii85_encode(b_, c_);

                const auto b = 5 == write(dst, c_, 5);
                i_ = 0;

                return b;
            }

        default:
            return false;
        }
    }

    template< typename Sink >
    bool overflow(Sink &dst)
    {
        return do_overflow(dst) && endl(dst);
    }

    template< typename Sink >
    bool do_put(Sink &dst, char c)
    {
        if (i_ < 4 || overflow(dst))
            return b_[i_++] = c, true;

        return false;
    }

    template< typename Sink >
    void flush(Sink &dst)
    {
        if (i_) {
            std::fill(b_ + i_, b_ + 4, 0);
            overflow(dst);
        }
    }

private:
    int i_ = 0, n_ = 0;
    char b_[4] = { 0 }, c_[5];
    bool eof_ = false;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_ASCII85_OUTPUT_FILTER_HH
