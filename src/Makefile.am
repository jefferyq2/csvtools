AM_CFLAGS = @CFLAGS@
AM_LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# # the previous manual Makefile
if USE_ODBC_H
bin_PROGRAMS = csv2odbc
csv2odbc_SOURCES = csv2odbc.c 
csv2odbc_CFLAGS = -I$(top_srcdir)/include
# csv2odbc_LDFLAGS = -all-static -lodbc
csv2odbc_LDFLAGS = -lodbc
csv2odbc_LDADD = ../lib/libcsvtools.la
endif
