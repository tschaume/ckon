# ckon - automatic compiler &amp; linker for ROOT analyses

*ckon* is a small C++ program/tool to automatically take care of compilation,
  dictionary generation and linking of ROOT 

## get the code

- for more see the project page at http://tschaume.github.com/ckon
- MIT License

## ckon
 
While developing code for our STAR analyses my colleague Hiroshi Masui and I
ended up using Autotools to compile, link and organize our classes and programs.
The setup needed to be easy to work with and had to support the use of CERN’s
data analysis software ROOT. This would combine the advantages of a standard GNU
build process with the requirements of a data analysis in Particle and Nuclear
Physics. Meanwhile, this approach has grown to an analysis tool which might be
interesting for the many ROOT-using physicists out there who don’t feel like
writing Makefiles but still want to keep their source directories well organized
or even want to use the power of a fully compiled program including an options
parser. On top, an Options class is included which makes compiling programs with
an option parser possible. The framework provides rules in the Makefile to
automatically translate doxygen commands and generate html- & pdf-documentation
(e.g. analyses homepages). The code includes simple example programs and more
advanced ones showing the usage of Bichsel functions and Gnuplot-C++ within this
framework. Makefiles and LinkDef’s are generated automatically based on the
contents in the src/ directory.

### Get the Code

There are a couple of options for you to get the code.

http://www.star-git.tschaume.com (usual STAR login)

Our main development we do through bitbucket.org which will always have the
newest versions. For those of you using bitbucket and/or interested in
contributing, we could grant read and/or write access to

https://bitbucket.org/potatoclub/templateanalysis

If you don’t have a bitbucket account but still want to have access via git, use

git clone http://USER:PWD@star-git.tschaume.com/templateanalysis (usual STAR
login)

This is a clone on my private host which should get updated as frequent as
bitbucket.

### References and Talk

Junior’s Talk at LBNL (2012/11/14)

### Build all Source Code & Usage (install)

Put header and source files for each library into a separate folder in src and
run ‘kon’. Put files you want to ignore into src/Obsolete. Running ‘kon’ should
automagically take the right action for the current status of your build
directory. run “./kon -h” for help about the usage.

This version should work for most of the daily-work use cases like using macros
and/or compiled programs. Makefiles and LinkDef’s are generated automatically
based on the contents in the src/ directory. If you want to call ‘make install’
a warning is issued asking you to put the according lines for LD_LIBRARY_PATH
into basrc/cshrc.

Example programs have been added to show how to use the STAR Bichsel class and
how to use Gnuplot from within C. The interface to Gnuplot has been written by
N. Devillard. see http://code.google.com/p/gnuplot-cpp/

The source code has been taken from the link given above and adapted to the
current framework. For the programs to run you need to download the files
uploaded in the Download section of bitbucket:

### Software Requirements

m4/1.4.6 autoconf/2.68 automake/1.11.4 libtool/2.4

### Related Stuff

#### Private ROOT Version from Source

``` setenv ROOTSYS ${HOME}/root setenv PATH ${HOME}/root/bin:${PATH} setenv
LD_LIBRARY_PATH ${HOME}/root/lib:${LD_LIBRARY_PATH} ./configure make -j8 make
install ```

#### Pythia Support for ROOT

wget ftp://root.cern.ch/root/pythia6.tar.gz changed gfortran to gfortran-mp-4.5
in makePythia6.macosx64 ./makePythia6.macosx64 # to get libPythia6.so/dylib
configure ROOT with –with-pythia6-libdir#$HOME/pythia6

