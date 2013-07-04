Please find the docs at https://ckon.readthedocs.org
----------------------------------------------------

*ckon* is a C++ program/tool which automatically takes care of compilation,
dictionary generation and linking of programs and libraries developed for data
analyses within the [CERN ROOT analysis framework](http://root.cern.ch). This
includes parsing include headers to figure out which libraries the main programs
need to be linked to. It uses
[automake/autoconf](http://www.gnu.org/software/autoconf/) to be platform
independent and GNU install compliant. In addition, [m4
macros](http://www.gnu.org/software/autoconf-archive/The-Macros.html#The-Macros)
are automatically downloaded and the according compiler flags included based on
a list of [boost](http://www.boost.org/) libraries provided in the config file.

#### Authors and Contributors
Patrick Huck (@tschaume)  
*invaluable contributions*: Hiroshi Masui

#### License & Project Homepage

*ckon* is published under MIT License (http://opensource.org/licenses/MIT).  
Find the project page at http://tschaume.github.com/ckon
