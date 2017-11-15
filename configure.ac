#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(includize, 0.0.1, dcdillon@gmail.com)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AX_CXX_COMPILE_STDCXX_11

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_CHECK_FUNCS([getcwd gettimeofday memset])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 tests/Makefile])

CXXFLAGS="$CXXFLAGS -Wall -Werror -pedantic"

AC_ARG_WITH([codecvt], AS_HELP_STRING([--without-codecvt],
            [Use disable codecvt]),
            CPPFLAGS="$CPPFLAGS -DINCLUDIZE_NO_CODECVT")

AC_OUTPUT
