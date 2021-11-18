// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_DCT_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_DCT_INPUT_FILTER_HH

#include <ypdf/detail/defs.hh>

#include <vector>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/read.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
namespace bios = boost::iostreams;

#include <jpeglib.h>
#include <turbojpeg.h>

namespace ypdf::iostreams {

struct dct_input_filter_t  : public boost::iostreams::input_filter
{
private:
    using this_type = dct_input_filter_t;

    struct jpeg_custom_source_mgr
    {
        jpeg_source_mgr pub;

        this_type *this_;
        void *source_;

        void (*init_callback)(jpeg_decompress_struct *);
        int  (*fill_callback)(jpeg_decompress_struct *);
        void (*skip_callback)(jpeg_decompress_struct *, long);
        void (*term_callback)(jpeg_decompress_struct *);
    };

public:
    template< typename Source >
    int get(Source &source)
    {
        return eof_ ? EOF : do_get(source);
    }

    template< typename Source >
    void close(Source &)
    {
        eof_ = true;
    }

private:
    template< typename Source >
    int do_get(Source &source)
    {
        if (!init_)
            init(source);

        if (iter_ == decoded_buffer_.end()) {
            if (ctx_.output_scanline < ctx_.output_height) {
                {
                    uint8_t *line = decoded_buffer_.data();
                    jpeg_read_scanlines(&ctx_, &line, 1);
                }

                iter_ = decoded_buffer_.begin();
            }

            if (iter_ == decoded_buffer_.end()) {
                return eof_ = true, EOF;
            }
        }

        return *iter_++;
    }

    template< typename Source >
    void do_init(Source &, jpeg_decompress_struct *)
    { }

    template< typename Source >
    int do_fill(Source &source, jpeg_decompress_struct *ptr)
    {
        assert(&src_ == reinterpret_cast< jpeg_custom_source_mgr * >(ptr->src));

        auto iter = encoded_buffer_.begin();

        for (int c;
             EOF != (c = bios::get(source)) && iter != encoded_buffer_.end(); ) {
            *iter++ = c;
        }

        src_.pub.next_input_byte = encoded_buffer_.data();
        src_.pub.bytes_in_buffer = std::distance(encoded_buffer_.begin(), iter);

        return 1;
    }

    template< typename Source >
    void do_skip(Source &source, jpeg_decompress_struct *ptr, long n)
    {
        ASSERT(&src_ == reinterpret_cast< jpeg_custom_source_mgr * >(ptr->src));

        if (n > 0) {
            while (n > (long)src_.pub.bytes_in_buffer) {
                n -= (long)src_.pub.bytes_in_buffer;
                (*src_.pub.fill_input_buffer)(&ctx_);
            }

            src_.pub.next_input_byte += (size_t)n;
            src_.pub.bytes_in_buffer -= (size_t)n;
        }
    }

    template< typename Source >
    void do_term(Source &source, jpeg_decompress_struct *ptr)
    {
    }

private:
    static dct_input_filter_t *this_from(jpeg_decompress_struct *ptr)
    {
        return reinterpret_cast< jpeg_custom_source_mgr * >(ptr->src)->this_;
    }

    template< typename Source >
    static Source *source_from(jpeg_decompress_struct *ptr)
    {
        return reinterpret_cast< Source * >(
            reinterpret_cast< jpeg_custom_source_mgr * >(ptr->src)->source_);
    }

private:
    template< typename Source >
    static void init(jpeg_decompress_struct *ptr)
    {
        auto this_ = this_from(ptr);
        auto source_ = source_from< Source >(ptr);
        this_->do_init(source_, ptr);
    }

    template< typename Source >
    static int fill(jpeg_decompress_struct *ptr)
    {
        auto this_ = this_from(ptr);
        auto source_ = source_from< Source >(ptr);
        return this_->do_fill< Source >(*source_, ptr);
    }

    template< typename Source >
    static void skip(jpeg_decompress_struct *ptr, long n)
    {
        auto this_ = this_from(ptr);
        auto source_ = source_from< Source >(ptr);
        this_->do_skip< Source >(*source_, ptr, n);
    }

    template< typename Source >
    static void term(jpeg_decompress_struct *ptr)
    {
        auto this_ = this_from(ptr);
        auto source_ = source_from< Source >(ptr);
        this_->do_term< Source >(*source_, ptr);
    }

private:
    template< typename Source >
    void init(Source &source)
    {
        ctx_.err = jpeg_std_error(&err_);
        jpeg_create_decompress(&ctx_);

        ctx_.src = reinterpret_cast< jpeg_source_mgr * >(&src_);

        src_.pub.init_source       = &this_type::init< Source >;
        src_.pub.fill_input_buffer = &this_type::fill< Source >;
        src_.pub.skip_input_data   = &this_type::skip< Source >;
        src_.pub.term_source       = &this_type::term< Source >;

        src_.pub.resync_to_restart = jpeg_resync_to_restart;

        encoded_buffer_.resize(4096U);

        src_.pub.bytes_in_buffer = 0;
        src_.pub.next_input_byte = encoded_buffer_.data();

        src_.this_ = this;
        src_.source_ = &source;

        jpeg_read_header(&ctx_, 1);
        jpeg_start_decompress(&ctx_);

        decoded_buffer_.resize(ctx_.output_width * ctx_.output_components);
        iter_ = decoded_buffer_.end();

        init_ = true;
    }

private:
    jpeg_decompress_struct ctx_;
    jpeg_custom_source_mgr src_;
    jpeg_error_mgr err_;

    std::vector< uint8_t > encoded_buffer_, decoded_buffer_;
    std::vector< uint8_t >::const_iterator iter_;

    bool eof_ = false, init_ = false;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_DCT_INPUT_FILTER_HH
