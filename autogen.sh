#!/bin/bash

VERSION=0.6.4
MIN_BOOST_VERSION=1.50
MIN_CURL_VERSION=7.34.0

[ -e configure.ac ] && rm -v configure.ac

echo 'AC_INIT([ckon], ['$VERSION'])' >> configure.ac
echo 'AC_CONFIG_MACRO_DIR([m4])' >> configure.ac
echo 'm4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])' >> configure.ac
echo 'AC_CONFIG_AUX_DIR(config)' >> configure.ac
echo 'm4_ifdef([AM_PROG_AR], [AM_PROG_AR])' >> configure.ac
echo 'AM_INIT_AUTOMAKE([-Wall no-define])' >> configure.ac
echo 'AC_PROG_CXX' >> configure.ac
echo 'AX_BOOST_BASE(['$MIN_BOOST_VERSION'])' >> configure.ac
echo 'AX_BOOST_SYSTEM' >> configure.ac
echo 'AX_BOOST_FILESYSTEM' >> configure.ac
echo 'AX_BOOST_REGEX' >> configure.ac
echo 'AX_BOOST_PROGRAM_OPTIONS' >> configure.ac
echo -n "AX_LIB_CURL([$MIN_CURL_VERSION],," >> configure.ac
echo "AC_MSG_ERROR([You need libcurl >= $MIN_CURL_VERSION]))" >> configure.ac
echo 'AC_CONFIG_FILES([Makefile])' >> configure.ac
echo 'AC_OUTPUT' >> configure.ac

[ -e Makefile.am ] && rm -v Makefile.am

echo 'AUTOMAKE_OPTIONS = foreign subdir-objects -Wall' >> Makefile.am
echo 'ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4' >> Makefile.am
echo 'AM_CPPFLAGS = $(BOOST_CPPFLAGS)' >> Makefile.am
echo 'AM_CPPFLAGS += $(CURL_CFLAGS)' >> Makefile.am
echo 'bin_PROGRAMS = ckon' >> Makefile.am
echo 'ckon_LDADD = $(BOOST_LDFLAGS) -lstdc++' >> Makefile.am
echo 'ckon_LDADD += $(BOOST_SYSTEM_LIB)' >> Makefile.am
echo 'ckon_LDADD += $(BOOST_FILESYSTEM_LIB)' >> Makefile.am
echo 'ckon_LDADD += $(BOOST_REGEX_LIB)' >> Makefile.am
echo 'ckon_LDADD += $(BOOST_PROGRAM_OPTIONS_LIB)' >> Makefile.am
echo 'ckon_LDADD += $(CURL_LIBS)' >> Makefile.am
echo 'ckon_SOURCES =' >> Makefile.am
for file in `find . -type f -name "*.h" -o -name "*.cc"`; do
  echo "ckon_SOURCES += $file" >> Makefile.am
done

