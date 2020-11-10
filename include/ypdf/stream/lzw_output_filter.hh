// -*- mode: c++; -*-
// Copyright 2020- Thinkoid, LLC

#ifndef YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
#define YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH

#include <ypdf/detail/defs.hh>
#include <ypdf/stream/lzw_filter_base.hh>

#include <map>
#include <string>

namespace ypdf::iostreams {

struct lzw_output_filter_t : ::boost::iostreams::output_filter, lzw_filter_base_t
{
    explicit lzw_output_filter_t() : table(make_table()) { }

    template< typename Sink >
    bool put(Sink &dst, char c)
    {
        string += c;

        if (table.end() == table.find(string)) {
            if ((1UL << max_bits) > next)
                table[string] = next++;

            string.pop_back();

            do_put(table.at(string), bits);
            flush(dst);

            if ((1UL << bits) < next) {
                if (++bits > max_bits) {
                    do_put(clear_code, max_bits);
                    flush(dst);

                    table = make_table();

                    bits = min_bits;
                    next = first_code;
                }
            }

            string = c;
        }

        return true;
    }

    template< typename Sink >
    void close(Sink &dst) {
        if (!string.empty())
            do_put(table.at(string), bits);

        do_put(eod_code, bits);
        purge(dst);
    }

private:
    static std::map< std::string, size_t >
    make_table()
    {
        std::map< std::string, size_t > table;

        for (size_t i = 0; i < 256; ++i)
            table[std::string(1, i)] = i;

        return table;
    }

    void do_put(size_t value, size_t bits)
    {
        buf |= (value << ((sizeof buf << 3) - pending - bits));
        pending += bits;
    }

    template< typename Sink >
    void do_flush(Sink &dst, size_t threshold)
    {
        for (; pending > threshold; pending -= (std::min)(8UL, pending)) {
            namespace bios = ::boost::iostreams;

            const auto c = buf >> ((sizeof buf << 3) - 8);
            bios::put(dst, c);

            buf <<= 8;
        }
    }

    template< typename Sink >
    void flush(Sink &dst)
    {
        do_flush(dst, 7);
    }

    template< typename Sink >
    void purge(Sink &dst)
    {
        do_flush(dst, 0);
    }

private:
    std::map< std::string, size_t > table;
    std::string string;
    size_t buf = 0, bits = min_bits, pending = 0, next = first_code;
};

} // namespace ypdf::iostreams

#endif // YPDF_IOSTREAMS_LZW_OUTPUT_FILTER_HH
