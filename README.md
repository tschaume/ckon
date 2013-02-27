*ckon* is a small C++ program/tool which automatically takes care of
compilation, dictionary generation as well linking of ROOT data analyses. This
includes building the required libraries and scanning/parsing include headers to
figure out which libraries the main programs need to be linked to. It uses
automake/autoconf to be platform independent and GNU install compliant.

### Get the Code & Install

The code is currently not published, yet. I plan to do it soon, though. If you'd like to contribute I'd need to grant you access to my private gitolite-managed repository server. Do the following:

- generate a ssh-key pair (no password), name the key-pair ```<username>(.pub)```
- send the .pub to @tschaume, put the following in ~/.ssh/config

        Host=gitolite
        Hostname=cgit.the-huck.com
        User=gitolite
        IdentityFile=~/.ssh/<username>
        IdentitiesOnly=yes
- as soon as I added your key, you should be able to check your accessible repos
via ```ssh gitolite info```
- clone *ckon* via ```git clone gitolite:ckon```
- to install: ```cd ckon; ./installCkon```  
(see ```./configure --help``` for configure options)

### Usage

#### Generic Options

Shown below are the generic command line options which can be given to *ckon*.

```
Generic Options:
  -h [ --help ]         show this help
  -v [ --verbose ]      verbose output
  -j arg                call make with option -j <#cores>
  --ckon_cmd arg        setup | clean | install
```

The long option ```--ckon_cmd``` is implemented as optional positional option
to run the setup, clean all compilation products (i.e. ```make clean```) and
globally install libraries and programs (i.e. ```make install```).

#### Setup
```ckon setup``` generates the files *configure.ac* and *.autom4te.cfg* (both
autoconf specific, no need for modifications) as well as *ckon.cfg*. Modify
the latter to resemble your directory structure and linker options.

#### Configuration
The following options can be set on the command line or preferably in
*ckon.cfg*. Optionally, a file named ```ckonignore``` with a list of strings to
be ignored during the build process, can be created in the working directory.
Wildcards not supported (yet).  

```
Configuration:
  -p [ --pythia ] arg    link with pythia library (bool)
  -r [ --roofit ] arg    link with roofit library (bool)
  -s [ --suffix ] arg    Add suffix + in LinkDef file (bool)
  -b [ --boost ] arg     include BOOST_INC and BOOST_LIB (bool)
  --ckon.src_dir arg     source dir
  --ckon.exclSuffix arg   no + suffix in LinkDef pragma
  --ckon.NoRootCint arg   no dictionary
        [don't generate CINT dictionary for subdirs in
        space-separated list ckon.NoRootCint]
  --ckon.prog_subdir arg  progs subdir
        [ckon.prog_subdir contains the programs
        for a specific subdir]
  --ckon.build_dir arg    build dir [$ ckon]
  --ckon.install_dir arg  install dir [$ ckon install]
  --ckon.cppflags arg    add CPPFLAGS to make call
```

#### Typical Directory Structure

Put header and source files for each library into a separate folder in
```ckon.src_dir```.  Running *ckon* should automagically take the right action
for the current status of your build directory. Makefiles and LinkDef’s are
generated automatically based on the contents in the ckon.src_dir directory.

[ldadd]
genCharmContrib=-lPhysics -lEG -lEGPythia6  # link pythia
dedxCut=-lRooFit -lRooFitCore -lMinuit      # link roofit

A typical directory structure could look as follows - using the current
defaults for illustration purposes:

```
StRoot
├── CocktailInputPt
│   ├── CocktailInputPt.cxx
│   ├── CocktailInputPt.h
│   ├── dat-files
│   └── programs
│       └── cipt.cc
├── ElectronPid
│   ├── BetaPanels.cxx
│   ├── BetaPanels.h
│   ├── PureSampleAnalysis.cxx
│   ├── PureSampleAnalysis.h
│   ├── SigmaElFitsMaker.cxx
│   ├── SigmaElFitsMaker.h
│   ├── SigmaElFitsPlotter.cxx
│   ├── SigmaElFitsPlotter.h
│   ├── SigmaElFitsUtils.cxx
│   ├── SigmaElFitsUtils.h
│   └── programs
│       ├── README
│       ├── beta3sig.cc
│       ├── dedxCut.cc
│       ├── nsigparamsGP.cc
│       └── pureSamp.cc
├── Gnuplot
│   ├── Gnuplot.cxx
│   └── Gnuplot.h
├── InvMassBgAnalysis
│   ├── InvMassBgAnalysis.cxx
│   ├── InvMassBgAnalysis.h
│   └── programs
│       ├── invmassBg.cc
│       └── runCocktail.cc
├── RunQA
│   ├── Grubbs.cxx
│   ├── Grubbs.h
│   ├── LinFit.cxx
│   ├── LinFit.h
│   ├── RunQaHistos.cxx
│   ├── RunQaHistos.h
│   └── programs
│       └── doRunQA.cc
├── StBadRdosDb
│   ├── StBadRdosDb.cxx
│   ├── StBadRdosDb.h
│   ├── database
│   │   ├── dbfiles
│   │   ├── genAll.sh
│   │   └── genBadRdosDb.pl
│   └── macros
│       └── testStBadRdosDb.C
├── YamlCfgReader
│   ├── YamlCfgReader.cxx
│   ├── YamlCfgReader.h
│   └── config.yml
```

### Appendix

#### Reference Talk about Automake/Autoconf Setup

Talk at Junior's Day of STAR Collaboration Meeting (LBNL, 2011/11/14)  
[link to be added]

#### Software Requirements

The following software and corresponding versions should be installed on your
system (might not be complete):

- m4/1.4.6
- autoconf/2.68
- automake/1.11.4
- libtool/2.4
- boost/1.50

#### License & Project Homepage

This software will be published under the MIT License
(http://opensource.org/licenses/MIT).  
Project page at http://tschaume.github.com/ckon

#### Authors and Contributors
Patrick Huck (@tschaume)  
*invaluable contributions*: Hiroshi Masui
