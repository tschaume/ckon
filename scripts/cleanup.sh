#!/bin/bash
set -v
make distclean
rm Makefile.am
rm Makefile.in
rm configure
rm configure.ac
rm aclocal.m4
rm -rf autom4te.cache
rm -rf config
rm -rf src/.deps
rm -rf src/options/.deps
rm -rf src/yaml-cpp/.deps
