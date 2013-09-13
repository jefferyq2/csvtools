AM_CFLAGS = @CFLAGS@
AM_LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# # the previous manual Makefile
#
bin_PROGRAMS = 
if USE_ODBC_H
bin_PROGRAMS += csv2odbc
csv2odbc_SOURCES = csv2odbc.c 
csv2odbc_CFLAGS = -I$(top_srcdir)/include
csv2odbc_LDFLAGS = -lodbc
csv2odbc_LDADD = ../lib/libcsvtools.la
endif
#
if USE_MYSQL_H
bin_PROGRAMS += csv2mysql
csv2mysql_SOURCES = csv2mysql.c 
csv2mysql_CFLAGS = -I$(top_srcdir)/include $(MYSQL_INCLUDE)
csv2mysql_LDFLAGS = $(MYSQL_LIBS)
csv2mysql_LDADD = ../lib/libcsvtools.la
endif
#
if USE_PGSQL_H
bin_PROGRAMS += csv2pgsql
csv2pgsql_SOURCES = csv2pgsql.c
csv2pgsql_CFLAGS = -I$(top_srcdir)/include -I$(PG_INCLUDE)
csv2pgsql_LDFLAGS = -lpq -L$(PG_LIBS)
csv2pgsql_LDADD = ../lib/libcsvtools.la
endif
