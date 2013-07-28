#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([csvtools], [0.0.3], [two.oes@gmail.com])
AM_INIT_AUTOMAKE(csvtools, 2.6)
#AC_CONFIG_SRCDIR([src/csv2odbc.c])
#AC_CONFIG_SRCDIR([lib/libcsvtools.c])
AC_CONFIG_HEADERS([config.h])
LT_PREREQ([2.2])
LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

AC_ARG_ENABLE([odbc],AS_HELP_STRING([--enable-odbc],enables the csv2odbc tool) ,
	[have_odbc=yes] , [have_odbc=no])

AS_IF([test "x$have_odbc" = "xyes"],
      [AC_DEFINE([USE_ODBC_H], 1, [found odbc connection to csv2odbc])])

AM_CONDITIONAL([USE_ODBC_H], [test "x$have_odbc" = "xyes"])

#AC_CONFIG_FILES([Makefile src/Makefile
AC_OUTPUT([Makefile include/Makefile
		   lib/Makefile
		   src/Makefile])
