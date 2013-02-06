#!/bin/bash
set -v
rm -rf build
rm Makefile.am
rm Makefile.in
rm configure
rm configure.ac
rm aclocal.m4
rm -rf autom4te.cache
rm -rf config
find . -type f -name ".deps" | xargs rm
