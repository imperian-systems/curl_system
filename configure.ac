AC_PREREQ([2.71])
AC_INIT([curl_system],[0.0.3],[tim.schwartz@imperiansystems.com],[curl_system],[imperian.systems])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT([disable-static])

AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES([LIBECS], ecs-cpp)
PKG_CHECK_MODULES([JSONCPP], jsoncpp)
PKG_CHECK_MODULES([LIBTHE_SEED], the-seed)
PKG_CHECK_MODULES([LIBCURL], libcurl)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
