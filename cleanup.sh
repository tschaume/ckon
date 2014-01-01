#!/bin/bash
set -v
rm -rf build
rm Makefile.in
rm configure
rm aclocal.m4
rm -rf autom4te.cache
rm -rf config
find . -type f -name ".deps" | xargs rm
