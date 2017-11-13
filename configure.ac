#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(includize, 0.0.1, dcdillon@gmail.com, includize)
AM_INIT_AUTOMAKE
AC_OUTPUT(Makefile include/Makefile tests/Makefile)
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

CXXFLAGS="-std=c++11 $CXXFLAGS -Wall -Werror -pedantic"

AC_OUTPUT
