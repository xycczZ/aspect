dnl config.m4 for extension aspect

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([aspect],
dnl   [for aspect support],
dnl   [AS_HELP_STRING([--with-aspect],
dnl     [Include aspect support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([aspect],
  [whether to enable aspect support],
  [AS_HELP_STRING([--enable-aspect],
    [Enable aspect support])],
  [no])

if test "$PHP_ASPECT" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, ASPECT_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-aspect -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/aspect.h"  # you most likely want to change this
  dnl if test -r $PHP_ASPECT/$SEARCH_FOR; then # path given as parameter
  dnl   ASPECT_DIR=$PHP_ASPECT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for aspect files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ASPECT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ASPECT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the aspect distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-aspect -> add include path
  dnl PHP_ADD_INCLUDE($ASPECT_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-aspect -> check for lib and symbol presence
  dnl LIBNAME=ASPECT # you may want to change this
  dnl LIBSYMBOL=ASPECT # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_ASPECT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your aspect library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ASPECT_DIR/$PHP_LIBDIR, ASPECT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ASPECT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your aspect library.])
  dnl ],[
  dnl   -L$ASPECT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ASPECT_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_ASPECT, 1, [ Have aspect support ])

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, $EXTRA_LDFLAGS)

  PHP_NEW_EXTENSION(aspect, aspect.cpp expr.cpp, $ext_shared)
fi
