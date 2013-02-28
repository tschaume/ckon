*ckon* is a C++ program/tool which automatically takes care of
compilation, dictionary generation and linking of programs and libraries
developed for data analyses within the [CERN ROOT analysis
framework](http://root.cern.ch). This includes parsing include headers to
figure out which libraries the main programs need to be linked to. It uses
[automake/autoconf](http://www.gnu.org/software/autoconf/) to be platform
independent and GNU install compliant.

### Get the Code & Install

- clone *ckon* via ```git clone git@github.com:tschaume/ckon.git```
- install via ```cd ckon; ./installCkon <install-path>```  
  (replace ```<install-path>``` with an install path in your $PATH)  
  (see ```./installCkon -h``` for help)
- see ```./configure --help``` for configure options in case something goes
  wrong

### Usage

#### Generic Options

Shown below are the generic command line options which can be given to *ckon*.

```
Generic Options:
  -h [ --help ]         show this help
  -v [ --verbose ]      verbose output
  -j arg                call make w/ -j <#cores>
  --ckon_cmd arg        setup | clean | install
```

The long option ```--ckon_cmd``` is implemented as optional positional option
to run the setup, clean all compilation products (i.e. ```make clean```) and
globally install libraries and programs (i.e. ```make install```):

- ```ckon setup```: run the setup
- ```ckon```: compile
- ```ckon clean```: make clean
- ```ckon install```: make install

#### Setup
```ckon setup``` generates the files *configure.ac* and *.autom4te.cfg* (both
autoconf specific, no need for modifications) as well as *ckon.cfg*. Modify
the latter to resemble your directory structure and linker options. Simply
remove the lines/options you don't need, thus using the default options.

#### Configuration
The following options can be set on the command line or preferably in
*ckon.cfg*. Optionally, a file named ```ckonignore``` with a list of strings to
be ignored during the build process, can be created in the working directory.
Wildcards are not supported (yet). Instead each path currently processed by
*ckon* will be checked against the strings/lines in ```ckonignore```. If one of
the strings in ```ckonignore``` is contained in the path, the path is
ignored/skipped.

```
Configuration:
  -s [ --suffix ] arg    add suffix + in LinkDef.h (bool)
  -b [ --boost ] arg     include BOOST_INC/BOOST_LIB (bool)
  --ckon.src_dir arg     source dir
  --ckon.exclSuffix arg  no + suffix
  --ckon.NoRootCint arg  no dictionary
  --ckon.prog_subdir arg progs subdir
  --ckon.build_dir arg   build dir
  --ckon.install_dir arg install dir
  --ckon.cppflags arg    add CPPFLAGS

In addition, unregistered options of the form
ldadd.prog_name are allowed to use for adding
LDFLAGS to the linker of specific programs. The
given string/value is added verbatim in LDADD.
Unregistered options are only allowed in ckon.cfg
```

The unregistered option group ```ldadd``` is allowed. For instance, link the
programs *genCharmContrib* and *dedxCut* versus
[Pythia6](http://home.thep.lu.se/~torbjorn/Pythia.html) and
[RooFit](http://root.cern.ch/drupal/content/roofit), respectively, by adding the
following to *ckon.cfg*.

```
[ldadd]
genCharmContrib=-lPhysics -lEG -lEGPythia6  # link pythia
dedxCut=-lRooFit -lRooFitCore -lMinuit      # link roofit
```

IMPORTANT note for include directives: For the recursive header scan to work,
make sure that all C++ and ROOT headers are enclosed in ```<...>```! Only your
local/private headers should be enclosed in ```"..."```. Otherwise *ckon* will
fail reporting a ```basic_string::_S_create``` error.

#### Typical Directory Structure

Put header and source files for each library into a separate folder in
```ckon.src_dir```.  Running *ckon* should automagically take the right action
for the current status of your build directory (no need to run ```ckon clean```
before re-compilation). Makefiles and LinkDef’s are generated automatically
based on the contents and timestamps in the ckon.src_dir directory.

A typical directory structure could look as follows - using the current
defaults for illustration purposes:

```
StRoot
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
...
```

### Reference Talk about Automake/Autoconf Setup

Talk at Junior's Day of STAR Collaboration Meeting (LBNL, 2011/11/14)  
https://speakerdeck.com/tschaume/organize-root-analyses-with-autoconf

### Software Requirements

The following software and corresponding versions should be installed on your
system:

- m4/1.4.6
- autoconf/2.68
- automake/1.11.4
- libtool/2.4
- boost/1.50

### License & Project Homepage

*ckon* is published under MIT License (http://opensource.org/licenses/MIT).  
Find the project page at http://tschaume.github.com/ckon

### Authors and Contributors
Patrick Huck (@tschaume)  
*invaluable contributions*: Hiroshi Masui
