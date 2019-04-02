#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libautomusic], [1.0], [sci-dev-git])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign])
LT_PREREQ([2.4.6])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([main.cc])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES(yaml_cpp, yaml-cpp >= 0.6.2)
PKG_CHECK_MODULES(opencv, opencv >= 3.4.1)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [enable DEBUG mode(default=no)]),
              [],
              [enable_debug=no])
AS_IF([test "x$enable_debug" = "xyes"], [CXXFLAGS="-g -O0 -DDEBUG -Wall"; CFLAGS="-g -O0 -DNO_DEBUG -Wall"]
    [test "x$enable_debug" = "xno"], [CXXFLAGS="-O2"; CFLAGS="-O2"],
    [])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
