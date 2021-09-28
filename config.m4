dnl $Id$
dnl config.m4 for extension ps

PHP_ARG_WITH(ps, for ps support,
Option to configure ps extension:
[  --with-ps    Include PostScript support])

if test "$PHP_PS" != "no"; then
  dnl Write more examples of tests here...

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  AC_MSG_CHECKING(for libps)
  if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libps; then
    LIBPS_INC=`$PKG_CONFIG libps --cflags`
    LIBPS_LIB=`$PKG_CONFIG libps --libs`
    LIBPS_VER=`$PKG_CONFIG libps --modversion`
    AC_MSG_RESULT(version $LIBPS_VER)
  else
    AC_MSG_ERROR(cannot find libps using pkg-config)
  fi
  PHP_EVAL_LIBLINE($LIBPS_LIB, PS_SHARED_LIBADD)
  PHP_EVAL_INCLINE($LIBPS_INC)

  PHP_SUBST(PS_SHARED_LIBADD)

  AC_DEFINE(HAVE_PS,1,[Whether to include the ps extension])
  PHP_NEW_EXTENSION(ps, ps.c, $ext_shared)
fi

PHP_ARG_ENABLE([gd],
  [for GD support],
  [AS_HELP_STRING([--enable-gd],
    [Include GD support])])

if test "$PHP_GD" != "no"; then

  AC_MSG_CHECKING(for libgd)
  if test -x "$PKG_CONFIG" && $PKG_CONFIG gdlib --atleast-version 2.1.1; then
    LIBGD_INC=`$PKG_CONFIG gdlib --cflags`
    LIBGD_LIB=`$PKG_CONFIG gdlib --libs`
    LIBGD_VER=`$PKG_CONFIG gdlib --modversion`
    AC_MSG_RESULT(version $LIBGD_VER)
  else
    AC_MSG_ERROR(cannot find libgd >= 2.1.1 using pkg-config)
  fi
  PHP_EVAL_LIBLINE($LIBGD_LIB, PS_SHARED_LIBADD)
  PHP_EVAL_INCLINE($LIBGD_INC)
  PHP_ADD_EXTENSION_DEP(ps, gd, true)

  AC_DEFINE(HAVE_LIBGD, 1, [Whether libgd was found on the system])
fi

