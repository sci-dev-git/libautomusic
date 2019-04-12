#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libautomusic], [1.0], [sci-dev-git])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign])
LT_PREREQ([2.4.6])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/libautomusic.cc])
AC_CONFIG_HEADERS([config.h])

# Package Options
# ---------------

AC_ARG_ENABLE(image_composition, AS_HELP_STRING([--enable-image-composition], [Enable image composition.(default=no)]),
    [],
    [enable_image_composition="yes"])
AS_IF([test "x$enable_image_composition" = "xyes"],
    [AC_DEFINE([ENABLE_IMAGE_COMPOSITION], [], [Define if image composition was enabled])]
    [AC_MSG_RESULT([Image composition enabled])],
    [])

AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [enable DEBUG mode(default=no)]),
    [],
    [enable_debug="no"])
AS_IF([test "x$enable_debug" = "xyes"], [CXXFLAGS="-g -O0 -DDEBUG -Wall"; CFLAGS="-g -O0 -DNO_DEBUG -Wall"],
    [test "x$enable_debug" = "xno"], [CXXFLAGS="-O2"; CFLAGS="-O2"],
    [])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# yaml-cpp
PKG_CHECK_MODULES(yaml_cpp, yaml-cpp >= 0.6.2)
LIBAM_CPPFLAGS="$yaml_cpp_CFLAGS"
LIBAM_LDFLAGS="$yaml_cpp_LIBS"

# opencv
if test "x$enable_image_composition" = "xyes"; then
    PKG_CHECK_MODULES(opencv, opencv >= 3.4.1)
    LIBAM_CPPFLAGS="$LIBAM_CPPFLAGS $opencv_CFLAGS"
    LIBAM_LDFLAGS="$LIBAM_LDFLAGS $opencv_LIBS"
fi

AC_SUBST(LIBAM_CPPFLAGS)
AC_SUBST(LIBAM_LDFLAGS)

# Checks for header files.

AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.

AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_C_INLINE

# Checks for library functions.

AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([Makefile
                src/Makefile
                examples/Makefile
])
AC_OUTPUT
