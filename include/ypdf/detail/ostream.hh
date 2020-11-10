// -*- mode: c++; -*-

#ifndef YPDF_DETAIL_OSTREAM_HH
#define YPDF_DETAIL_OSTREAM_HH

#include <boost/noncopyable.hpp>

#include <iostream>

namespace ypdf::detail {

struct ostream_guard_t : boost::noncopyable
{
    ostream_guard_t(std::ostream &stream)
        : stream(stream), flags(stream.flags())
    { }

    ~ostream_guard_t() { stream.flags(flags); }

private:
    std::ostream &stream;
    std::ios_base::fmtflags flags;
};

} // namespace ypdf::detail

#endif // YPDF_DETAIL_OSTREAM_HH
