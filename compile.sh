#!/bin/bash

if [ $# -ge 1 ]; then

  if [ $# -ge 3 ]; then
    echo "too many arguments."
    echo "usage: $0 prefix [nCpu]"
    exit
  fi

  nCpu=1
  if [ $# -eq 2 ]; then nCpu=$2; fi
  prefix=$1
  uname

  if [ "`uname`" == Darwin ]; then
    BOOST_SUF=-mt
  fi
  echo "BOOST_INC = $BOOST_INC"
  echo "BOOST_LIB = $BOOST_LIB"
  if [ -z "$BOOST_LIB" -o -z "$BOOST_INC" ]; then
    echo "set BOOST env!"
    exit
  fi

  # configure.ac
  if [ ! -e configure.ac ]; then
    echo 'AC_INIT([ckon], [0.0])' >> configure.ac
    echo 'm4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])' >> configure.ac
    echo 'AC_CONFIG_AUX_DIR(config)' >> configure.ac
    echo 'm4_pattern_allow([AM_PROG_AR])' >> configure.ac
    echo 'm4_ifdef([AM_PROG_AR], [AM_PROG_AR])' >> configure.ac
    echo 'AM_INIT_AUTOMAKE([-Wall no-define])' >> configure.ac
    echo 'AC_PROG_CXX' >> configure.ac
    echo 'AC_CONFIG_FILES([Makefile])' >> configure.ac
    echo 'AC_OUTPUT' >> configure.ac
  fi

  if [ ! -e Makefile.am ]; then
    echo 'AUTOMAKE_OPTIONS = foreign subdir-objects -Wall' >> Makefile.am
    echo 'ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}' >> Makefile.am
    echo 'AM_CPPFLAGS = -I'$BOOST_INC >> Makefile.am
    echo 'bin_PROGRAMS = ckon' >> Makefile.am
    echo 'ckon_LDFLAGS = -L'$BOOST_LIB' -lstdc++' >> Makefile.am
    echo 'ckon_LDFLAGS += -lboost_filesystem'$BOOST_SUF >> Makefile.am
    echo 'ckon_LDFLAGS += -lboost_system'$BOOST_SUF >> Makefile.am
    echo 'ckon_LDFLAGS += -lboost_regex'$BOOST_SUF >> Makefile.am
    echo 'ckon_LDFLAGS += -lboost_program_options'$BOOST_SUF >> Makefile.am
    echo 'ckon_SOURCES =' >> Makefile.am
    for file in `find . -type f -name "*.h" -o -name "*.cc"`; do
      echo "ckon_SOURCES += $file" >> Makefile.am
    done
  fi

  autoreconf --force --install
  mkdir -v build
  cd build
  ../configure --prefix=$prefix
  make -j $nCpu
  make install

  echo "===> ckon build done."

else
  echo "usage: $0 prefix [nCpu]"
  echo "prefix = install dir"
  echo "nCpu = #cpu's for parallel build (-j) [default=1]"
fi
