#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.58])
AC_INIT( swfdec-gnome, [0.5.4], [https://bugs.freedesktop.org/enter_bug.cgi?product=swfdec] )

AM_INIT_AUTOMAKE(1.6)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
IT_PROG_INTLTOOL([0.35.0])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strstr])

GNOME_COMMON_INIT
dnl GNOME_DOC_INIT
GNOME_MAINTAINER_MODE_DEFINES
GNOME_COMPILE_WARNINGS
GNOME_CXX_WARNINGS
GNOME_DEBUG_CHECK

dnl GTK_DOC_CHECK(1.0)

dnl Setup gettext
GETTEXT_PACKAGE=swfdec-gnome
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

dnl Required versions

SWFDEC_MAJORMINOR=0.5
AC_SUBST(SWFDEC_MAJORMINOR)
SWFDEC_REQUIRED=0.5.4

GTK_REQUIRED=2.12.0

dnl Check dependencies

PKG_CHECK_MODULES(THUMBNAILER, swfdec-$SWFDEC_MAJORMINOR = $SWFDEC_REQUIRED)
PKG_CHECK_MODULES(PLAYER, gmodule-export-2.0 swfdec-$SWFDEC_MAJORMINOR = $SWFDEC_REQUIRED swfdec-gtk-$SWFDEC_MAJORMINOR = $SWFDEC_REQUIRED gtk+-2.0 >= $GTK_REQUIRED)

AM_GCONF_SOURCE_2
AC_PATH_PROG([GCONFTOOL], [gconftool-2], [no])
if test "x$GCONFTOOL" = "xno"; then
	AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
fi


dnl Finish up

AC_SUBST(GLOBAL_CFLAGS)

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 player/Makefile
                 po/Makefile.in
                 thumbnailer/Makefile])
AC_OUTPUT
