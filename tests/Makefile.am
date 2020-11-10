# -*- mode: makefile -*-

EXTRA_DIST =

include $(top_srcdir)/Makefile.common
LIBS += $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)

LDADD = $(top_srcdir)/src/libypdf.a $(LIBS)

check_PROGRAMS =								\
	ascii85_input_test							\
	ascii85_output_test							\
	asciihex_input_test							\
	asciihex_output_test						\
	eol											\
	comment										\
	lit											\
	lookahead									\
	name										\
	numeric_									\
	obj											\
	ref											\
	skip										\
	string_										\
	xref

TESTS = $(check_PROGRAMS)

ascii85_input_test_SOURCES = ascii85_input_test.cc
ascii85_output_test_SOURCES = ascii85_output_test.cc
asciihex_input_test_SOURCES = asciihex_input_test.cc
asciihex_output_test_SOURCES = asciihex_output_test.cc
comment_SOURCES = comment.cc
eol_SOURCES = eol.cc
lit_SOURCES = lit.cc
lookahead_SOURCES = lookahead.cc
name_SOURCES = name.cc
numeric__SOURCES = numeric.cc
obj_SOURCES = obj.cc
ref_SOURCES = ref.cc
skip_SOURCES = skip.cc
string__SOURCES = string.cc
xref_SOURCES = xref.cc
