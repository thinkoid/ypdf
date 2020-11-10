// -*- mode: c++; -*-

#ifndef YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/stream/lzw_filter_base.hh>

#include <map>
#include <string>

namespace ypdf::iostreams {

struct lzw_input_filter_t
    : ::boost::iostreams::input_filter, lzw_filter_base_t
{
    explicit lzw_input_filter_t() : table(make_table()) { }

    template< typename Source >
    int get(Source &src) {
        if (eof)
            return EOF;

        if (0 == cur) {
            const size_t code = do_get(src);

            switch (code) {
            case eod_code:
                return eof = true, EOF;

            case clear_code:
                table = make_table();

                next = first_code;
                bits = min_bits;

                return get(src);

            default: {
                auto iter = table.find(code);

                if (iter == table.end()) {
                    ASSERT(prev && !prev->empty());
                    iter = table.emplace(code, *prev + (*prev)[0]).first;
                }

                cur = &iter->second;
                pos = 0;
            }
                break;
            }

            if (bits < max_bits && (1UL << bits) - 1 <= next)
                ++bits;

            if (prev && !prev->empty() && (1UL << max_bits) > next)
                table[next++] = *prev + cur->at(0);

            prev = cur;
        }

        ASSERT(cur && pos < cur->size());
        const int c = static_cast< unsigned char >(cur->at(pos));

        if (++pos >= cur->size()) {
            cur = 0;
            pos = 0;
        }

        return c;
    }

    template< typename Source >
    void close(Source &) {
        eof = true;
    }

private:
    static std::map< size_t, std::string > make_table() {
        std::map< size_t, std::string > table;

        for (size_t i = 0; i < 256; ++i)
            table[i] = std::string(1UL, i);

        return table;
    }

    template< typename Source >
    size_t do_get(Source &src) {
        namespace bios = ::boost::iostreams;

        for (int c; pending < bits; pending += 8) {
            if (EOF == (c = bios::get(src)))
                return eod_code;

            buf |= size_t(c) << ((sizeof buf << 3) - pending - 8);
        }

        const size_t code = buf >> ((sizeof buf << 3) - bits);

        buf <<= bits;
        pending -= bits;

        return code;
    }

private:
    std::map< size_t, std::string > table;

    std::string *cur = 0, *prev = 0;
    std::size_t pos = 0;

    size_t buf = 0, bits = min_bits, pending = 0, next = first_code;
    bool eof = false;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_INPUT_FILTER_HH
