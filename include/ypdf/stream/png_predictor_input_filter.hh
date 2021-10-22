// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_PNG_PREDICTOR_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_PNG_PREDICTOR_INPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <cctype>
#include <boost/iostreams/concepts.hpp>

namespace ypdf {
namespace iostreams {

struct png_predictor_input_filter_t : public boost::iostreams::input_filter
{
    png_predictor_input_filter_t(size_t ppl, size_t cpp, size_t bpc)
        : ppl_{ppl}, cpp_{cpp}, bpc_{bpc}, cpl_{ppl * cpp},
          Bpp_{ (cpp  * bpc + 7) / 8},
          Bpl_{((cpl_ * bpc + 7) / 8) + Bpp_}, // a pixel more than line size
          line_(size_t(Bpl_)) {
        assert(ppl_ > 0);
        assert(cpp_ > 0);
        assert(bpc_ > 0);

        assert(0 < cpp_ && cpp_ < 32);
        assert(0 < bpc_ && bpc_ < 16);

        pos_ = Bpl_;
    }

    template< typename Source > int get(Source &src)
    {
        return eof_ ? EOF : do_get(src);
    }

    template< typename Source > void close(Source &)
    {
        eof_ = true;
    }

private:
    template< typename Source > void getline(Source &src) {
        namespace bios = ::boost::iostreams;

        int pred = 0;

        if (EOF == (pred = bios::get(src))) {
            eof_ = true;
            return;
        }

        assert(0 <= pred && pred <= 5);
        pred += 10;

        uint8_t buf[16] = { 0 };

        for (size_t i = Bpp_; i < Bpl_; ++i) {
            std::rotate(buf, buf + 1, buf + sizeof buf);
            buf[0] = line_[i];

            union {
                int i;
                uint8_t c;
            } x;

            if (EOF == (x.i = bios::get(src))) {
                if (i > Bpp_) {
                    // Allow incomplete lines
                    break;
                } else {
                    eof_ = true;
                    return;
                }
            }

            int a, b, c, pa, pb, pc, p;

            switch (pred) {
            case 11: // PNG sub
                line_[i] = line_[i - Bpp_] + x.c;
                break;

            case 12: // PNG up
                line_[i] = line_[i] + x.c;
                break;

            case 13: // PNG average
                line_[i] = ((line_[i - Bpp_] + line_[i]) / 2) + x.c;
                break;

            case 14: // PNG Paeth algorithm
                a = line_[i - Bpp_];
                b = line_[i];
                c = buf[Bpp_];

                p = a + b - c;

                if ((pa = p - a) < 0) pa = -pa;
                if ((pb = p - b) < 0) pb = -pb;
                if ((pc = p - c) < 0) pc = -pc;

                if      (pa <= pb && pa <= pc) line_[i] = a + x.c;
                else if (pb <= pc)             line_[i] = b + x.c;
                else                           line_[i] = c + x.c;

                break;

            case 10:
            default:
                line_[i] = x.c;
                break;
            }
        }

        pos_ = Bpp_;
    }

    template< typename Source > int do_get(Source &src)
    {
        if (pos_ >= Bpl_)
            getline(src);

        return eof_ ? EOF : line_[pos_++];
    }

private:
    size_t ppl_, cpp_, bpc_, cpl_, Bpp_, Bpl_;

    bool eof_ = false;

    std::vector< uint8_t > line_;
    std::size_t pos_ = 0;
};

} // namespace iostreams
} // namespace ypdf

#endif // YPDF_IOSTREAMS_PNG_PREDICTOR_INPUT_FILTER_HH
