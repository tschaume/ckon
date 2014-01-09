#!/bin/bash
#set -v
# files
[ -e Makefile.in ] && rm -v Makefile.in
[ -e configure ] && rm -v configure
[ -e aclocal.m4 ] && rm -v aclocal.m4
# dirs
[ -d build ] && rm -rvf build
[ -d autom4te.cache ] && rm -rvf autom4te.cache
[ -d config ] && rm -rvf config
[ -d bin ] && rm -rvf bin
find . -type f -name ".deps" -exec rm -v {} \;
