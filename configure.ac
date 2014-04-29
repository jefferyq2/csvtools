#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([csvtools], [1.0.60], [two.oes@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
LT_PREREQ([2.2])
LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O


AC_ARG_WITH([odbc],AS_HELP_STRING([--with-odbc],enables the csv2odbc tool [[default=no]]),
	[have_odbc=yes] , [have_odbc=no])

if test "x$have_odbc" = "xyes"; then

	# Checks for libraries.
AC_SEARCH_LIBS([SQLDriverConnect], [odbc], [have_odbc=yes],
   [AC_MSG_ERROR(["unable to locate sqlite3 libraries"])])

	# Checks for header files.
AC_CHECK_HEADERS([sql.h sqlext.h], [have_odbc=yes],
   [AC_MSG_ERROR(["unable to locate sqlite3 libraries"])])
      	
AC_DEFINE([USE_ODBC_H], 1, [found odbc connection to csv2odbc])
fi




AC_ARG_WITH([pgsql],AS_HELP_STRING([--with-pgsql],enables the csv2pgsql tool [[default=no]]),
        [have_pgsql=yes], [have_pgsql=no])

AS_IF([test "x$have_pgsql" = "xyes"],
      [AC_CHECK_PROG([have_pg_config],pg_config,[yes],[AC_MSG_ERROR(["please make sure you have pg_config in your path"])])])

if test "x$have_pg_config" = "xyes"; then

AC_ARG_WITH([pgsql-include-path],
    [AS_HELP_STRING([--with-pgsql-include-path],
    [location of the Postgresql headers, defaults to /usr/include/])],
    [pgsql_include="$withval"],
    [pgsql_include="/usr/include"])

AC_CHECK_HEADERS(["$pgsql_include"/libpq-fe.h], [], AC_MSG_ERROR(["please make sure you have postgresql development kit"]))
	
PG_INCLUDE=`pg_config --includedir`
AC_SUBST(PG_INCLUDE)
	
PG_LIBS=`pg_config --libdir`
AC_SUBST(PG_LIBS)
     
AC_DEFINE([USE_PGSQL_H], 1, [found Postgresql connection for csv2pgsql])

fi


AC_ARG_WITH([mysql],AS_HELP_STRING([--with-mysql],enables the csv2mysql tool [[default=no]]),
        [have_mysql=yes], [have_mysql=no])

AS_IF([test "x$have_mysql" = "xyes"],
      [AC_CHECK_PROG([have_mysql_config],mysql_config,[yes],[AC_MSG_ERROR(["please make sure you have mysql_config in your path"])])])


if test "x$have_mysql_config" = "xyes"; then

AC_ARG_WITH([mysql-include-path],
    [AS_HELP_STRING([--with-mysql-include-path],
    [location of the MySQL headers, defaults to /usr/include/mysql])],
    [mysql_include="$withval"],
    [mysql_include="/usr/include/mysql"])

AC_CHECK_HEADERS(["$mysql_include"/mysql.h], [], AC_MSG_ERROR(["please make sure you have Mysql/MariaDB development kit"]))

MYSQL_INCLUDE=`mysql_config --cflags`
AC_SUBST(MYSQL_INCLUDE)

MYSQL_LIBS=`mysql_config --libs`
AC_SUBST(MYSQL_LIBS)

AC_DEFINE([USE_MYSQL_H], 1, [found mysql connection to csv2mysql])

fi

AC_ARG_WITH([sqlite],AS_HELP_STRING([--without-sqlite],disables insertion to sqlite with csv2sqlite tool [[default=yes]]),
	[have_sqlite=no], [have_sqlite=yes])

if test "x$have_sqlite" = "xyes"; then

dnl checking for sqlite biraries
AC_SEARCH_LIBS([sqlite3_open], [sqlite3], [have_sqlite=yes],
   [AC_MSG_ERROR(["unable to locate sqlite3 libraries"])])

dnl checking for header files for sqlite3
AC_CHECK_HEADERS([sqlite3.h], [have_sqlite=yes],
   [AC_MSG_ERROR(["unable to locate the sqlite3.h header file"])])
fi

AS_IF([test "x$have_sqlite" = "xyes"],
   [AC_DEFINE([USE_SQLITE_H], 1, [the tool csv2sqite will be compiled])])



AM_CONDITIONAL([USE_PGSQL_H], [test "x$have_pgsql" = "xyes"])
AM_CONDITIONAL([USE_MYSQL_H], [test "x$have_mysql" = "xyes"])
AM_CONDITIONAL([USE_ODBC_H], [test "x$have_odbc" = "xyes"])
AM_CONDITIONAL([USE_SQLITE_H], [test "x$have_sqlite" = "xyes"])

### Makefiles ###

#AC_CONFIG_FILES([Makefile src/Makefile
AC_OUTPUT([Makefile include/Makefile
		   lib/Makefile
		   man/Makefile
		   src/Makefile])
