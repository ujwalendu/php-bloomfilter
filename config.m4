dnl $Id$
dnl config.m4 for extension bloomfilter

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(bloomfilter, for bloomfilter support,
dnl Make sure that the comment is aligned:
dnl [  --with-bloomfilter             Include bloomfilter support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(bloomfilter, whether to enable bloomfilter support,
dnl Make sure that the comment is aligned:
[  --enable-bloomfilter           Enable bloomfilter support])

if test "$PHP_BLOOMFILTER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-bloomfilter -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/bloomfilter.h"  # you most likely want to change this
  dnl if test -r $PHP_BLOOMFILTER/$SEARCH_FOR; then # path given as parameter
  dnl   BLOOMFILTER_DIR=$PHP_BLOOMFILTER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for bloomfilter files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BLOOMFILTER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BLOOMFILTER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the bloomfilter distribution])
  dnl fi

  dnl # --with-bloomfilter -> add include path
  dnl PHP_ADD_INCLUDE($BLOOMFILTER_DIR/include)

  dnl # --with-bloomfilter -> check for lib and symbol presence
  dnl LIBNAME=bloomfilter # you may want to change this
  dnl LIBSYMBOL=bloomfilter # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BLOOMFILTER_DIR/lib, BLOOMFILTER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BLOOMFILTERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong bloomfilter lib version or lib not found])
  dnl ],[
  dnl   -L$BLOOMFILTER_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(BLOOMFILTER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(bloomfilter, bloomfilter.c, $ext_shared)
fi
