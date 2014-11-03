Please find the docs at https://ckon.readthedocs.org
----------------------------------------------------

[![DOI](https://zenodo.org/badge/4568/tschaume/ckon.png)](http://dx.doi.org/10.5281/zenodo.12533)

*ckon* is a C++ program/tool which automatically takes care of compilation,
dictionary generation and linking of programs and libraries developed for data
analyses within the [CERN ROOT analysis framework](http://root.cern.ch). This
includes parsing include headers to figure out which libraries the main
programs need to be linked to. It uses
[automake/autoconf](http://www.gnu.org/software/autoconf/) to be platform
independent and GNU install compliant. In addition, [m4
macros](http://www.gnu.org/software/autoconf-archive/The-Macros.html#The-Macros)
are automatically downloaded and the according compiler flags included based on
a list of [boost](http://www.boost.org/) libraries provided in the config file.
For the purpose of YAML database usage, a m4 macro can be downloaded during
setup to link against the [yaml-cpp](https://code.google.com/p/yaml-cpp/)
library.

*ckon* is published under MIT License (http://opensource.org/licenses/MIT).  

Patrick Huck (@tschaume)  
*invaluable contributions*: Hiroshi Masui
