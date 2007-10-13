AC_PREREQ([2.58])
AC_INIT(swfdec-thumbnailer, 0.5.3.1)

[nano=$(echo $PACKAGE_VERSION | sed 's/[0-9]\.[0-9]\.[0-9][0-9]*\.*//')]
if test x"$nano" = x1 ; then
  SWFDEC_CVS="yes"
else
  SWFDEC_CVS="no"
fi

AM_INIT_AUTOMAKE(1.6)
dnl AC_CANONICAL_TARGET([])
AM_MAINTAINER_MODE

SWFDEC_MAJORMINOR=0.5
AC_SUBST(SWFDEC_MAJORMINOR)

AM_CONFIG_HEADER(config.h)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")

dnl decide on error flags
dnl if we support them, we set them unconditionally
AS_COMPILER_FLAG(-Wall, GLOBAL_CFLAGS="-Wall", GLOBAL_CFLAGS="")
dnl I want this but stupid headers don't let me
dnl AS_COMPILER_FLAG(-Wshadow, GLOBAL_CFLAGS="$GLOBAL_CFLAGS -Wshadow")
AS_COMPILER_FLAG(-Wextra -Wno-missing-field-initializers -Wno-unused-parameter, GLOBAL_CFLAGS="$GLOBAL_CFLAGS -Wextra -Wno-missing-field-initializers -Wno-unused-parameter")
dnl if we're in nano >= 1, add -Werror if supported
if test x$SWFDEC_CVS = xyes ; then
  AS_COMPILER_FLAG(-Werror, GLOBAL_CFLAGS="$GLOBAL_CFLAGS -Werror")
fi

AC_PROG_CC


dnl modify pkg-config path
AC_ARG_WITH(pkg-config-path, 
   AC_HELP_STRING([--with-pkg-config-path],[colon-separated list of pkg-config(1) dirs]),
   [export PKG_CONFIG_PATH=${withval}])

SWFDEC_REQUIRES=0.5.3.1
PKG_CHECK_MODULES(SWFDEC, swfdec-$SWFDEC_MAJORMINOR = $SWFDEC_REQUIRES, HAVE_SWFDEC=yes, HAVE_SWFDEC=no)
if test "$HAVE_SWFDEC" = "no"; then
  AC_MSG_ERROR([cannot find swfdec-$SWFDEC_MAJORMINOR $SWFDEC_REQUIRES, which is required for build])
fi
AC_SUBST(SWFDEC_LIBS)
AC_SUBST(SWFDEC_CFLAGS)

AC_SUBST(GLOBAL_CFLAGS)

AC_CONFIG_FILES(Makefile src/Makefile)

AC_OUTPUT
