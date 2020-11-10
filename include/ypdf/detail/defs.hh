// -*- mode: c++; -*-

#ifndef YPDF_DETAIL_DEFS_HH
#define YPDF_DETAIL_DEFS_HH

#include <ypdf/detail/config.hh>

#include <cassert>
#include <cstddef>

#include <boost/assert.hpp>
#define YPDF_ASSERT BOOST_ASSERT
#define ASSERT YPDF_ASSERT

#define YPDF_UNUSED(x) ((void)&x)
#define UNUSED YPDF_UNUSED

#define YPDF_DO_CAT(x, y) x##y
#define YPDF_CAT(x, y) YPDF_DO_CAT(x, y)

#define YPDF_DO_TO_S(x) #x
#define YPDF_TO_S(x) YPDF_DO_TO_S(x)

#endif // YPDF_DETAIL_DEFS_HH
