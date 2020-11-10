# -*- Autoconf -*-

AC_INIT([ypdf],[0.0.1],)

AC_CONFIG_HEADERS([include/ypdf/detail/_config.hh:include/ypdf/detail/_config.hh.ac])
AC_CONFIG_MACRO_DIRS([etc/m4])

AM_INIT_AUTOMAKE([-Wno-portability subdir-objects dist-bzip2 nostdinc foreign])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_CONFIG_CXX_WARNINGS

AC_ENABLE_CXX_DIALECT([c++2a])

AC_PROG_LIBTOOL
AC_PROG_MAKE_SET(gmake)
AM_PROG_AS

AC_CONFIG_WITH_RANGE3

BOOST_REQUIRE([1.69.0])

BOOST_FILESYSTEM
BOOST_IOSTREAMS
BOOST_PROGRAM_OPTIONS
BOOST_SYSTEM
BOOST_TEST

PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])

AM_CONDITIONAL([DARWIN],[test `uname` == Darwin])
AM_CONDITIONAL([LINUX], [test `uname` == Linux])

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(include/Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_CONFIG_FILES(examples/Makefile)
AC_CONFIG_FILES(tests/Makefile)

AC_OUTPUT()

