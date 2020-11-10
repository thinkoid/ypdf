# -*- mode: makefile; -*-

include $(top_srcdir)/Makefile.common

bin_PROGRAMS = ypdf

ypdf_SOURCES = ypdf.cc options.cc
ypdf_LDADD = $(top_srcdir)/src/libypdf.a $(BOOST_PROGRAM_OPTIONS_LIBS) $(LIBS)
