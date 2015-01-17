#!/bin/bash

srcdir="StRoot"
[ -e $srcdir ] && rm -rfv $srcdir
mkdir -p $srcdir
git clone https://github.com/tschaume/BesCocktail.git $srcdir/BesCocktail
