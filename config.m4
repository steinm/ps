dnl $Id$
dnl config.m4 for extension pslib

PHP_ARG_WITH(pslib, for pslib support,
Option to configure pslib extension:
[  --with-pslib[=DIR]    Include PostScript support])

if test "$PHP_PSLIB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pslib -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="include/libps/pslib.h"  # you most likely want to change this
  if test -r $PHP_PSLIB/$SEARCH_FOR; then # path given as parameter
    PSLIB_DIR=$PHP_PSLIB
  else # search default path list
    AC_MSG_CHECKING([for pslib files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        PSLIB_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$PSLIB_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the pslib distribution])
  fi

  PHP_ADD_INCLUDE($PSLIB_DIR/include)

  LIBNAME=ps # you may want to change this
  LIBSYMBOL=PS_show # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PSLIB_DIR/lib, PSLIB_SHARED_LIBADD)
    AC_DEFINE(HAVE_PSLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong pslib lib version or lib not found])
  ],[
    -L$PSLIB_DIR/lib -lm -ldl
  ])

  PHP_SUBST(PSLIB_SHARED_LIBADD)

  AC_DEFINE(HAVE_PSLIB,1,[Whether to include the pslib extension])
  PHP_NEW_EXTENSION(pslib, ps.c, $ext_shared)
fi
