dnl $Id$
dnl config.m4 for extension ps

PHP_ARG_WITH(ps, for ps support,
Option to configure ps extension:
[  --with-ps[=DIR]    Include PostScript support])

if test "$PHP_PS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ps -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="include/libps/pslib.h"  # you most likely want to change this
  if test -r $PHP_PS/$SEARCH_FOR; then # path given as parameter
    PS_DIR=$PHP_PS
  else # search default path list
    AC_MSG_CHECKING([for ps files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        PS_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$PS_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the pslib distribution])
  fi

  PHP_ADD_INCLUDE($PS_DIR/include)

  LIBNAME=ps # you may want to change this
  LIBSYMBOL=PS_begin_pattern # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
  	AC_CHECK_LIB($LIBNAME, PS_begin_font, AC_DEFINE(HAVE_PSBEGINFONT,1,[pslib 0.4.0 or later]))
  	AC_CHECK_LIB($LIBNAME, PS_glyph_show, AC_DEFINE(HAVE_PSGLYPHSHOW,1,[pslib 0.4.0 or later]))
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PS_DIR/lib, PS_SHARED_LIBADD)
    AC_DEFINE(HAVE_PS,1,[ ])
  ],[
    AC_MSG_ERROR([wrong pslib lib version or lib not found])
  ],[
    -L$PS_DIR/lib $PS_SHARED_LIBADD -lm
  ])

  PHP_SUBST(PS_SHARED_LIBADD)

  AC_DEFINE(HAVE_PS,1,[Whether to include the ps extension])
  PHP_NEW_EXTENSION(ps, ps.c, $ext_shared)
fi

PHP_ARG_ENABLE([gd],
  [for GD support],
  [AS_HELP_STRING([--enable-gd],
    [Include GD support])])

if test "$PHP_GD" != "no"; then
  PKG_CHECK_MODULES([GDLIB], [gdlib >= 2.1.0])
  PHP_EVAL_LIBLINE($GDLIB_LIBS, GD_SHARED_LIBADD)
  PHP_EVAL_INCLINE($GDLIB_CFLAGS)
  AC_DEFINE(HAVE_LIBGD, 1, [Whether libgd was found on the system])
fi

