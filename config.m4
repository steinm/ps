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
  LIBSYMBOL=PS_show # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PS_DIR/lib, PS_SHARED_LIBADD)
    AC_DEFINE(HAVE_PS,1,[ ])
  ],[
    AC_MSG_ERROR([wrong pslib lib version or lib not found])
  ],[
    -L$PS_DIR/lib -lm -ldl
  ])

  PHP_SUBST(PS_SHARED_LIBADD)

  AC_DEFINE(HAVE_PS,1,[Whether to include the ps extension])
  PHP_NEW_EXTENSION(ps, ps.c, $ext_shared)
fi
