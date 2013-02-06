#include "utils.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

utils::utils() {}

double utils::compareTimeStamps(const fs::path& f2, const fs::path& f1) {
  time_t t1 = fs::last_write_time(f1);
  time_t t2 = fs::last_write_time(f2);
  return difftime(t2,t1); // t2 - t1 in seconds
}

bool utils::checkTimeStamp(const fs::path& file, vector<fs::path> filelist) {
  BOOST_FOREACH( fs::path p, filelist ) {
    if ( compareTimeStamps(p,file) > 0 ) return false;
  }
  return true;
}

char utils::askYesOrNo(string question) {
  char type;
  do {
    cout << question << "? [y/n] ";
    cin >> type; cin.ignore();
  }
  while( !cin.fail() && type!='y' && type!='n' );
  return type;
}

void utils::writeConfigureAc() {
  fs::ofstream cfg_ac;
  cfg_ac.open("configure.ac");
  string ckon_ana_name, ckon_ana_version;
  cout << "set ckon_ana_name : "; getline(cin,ckon_ana_name);
  cout << "set ckon_ana_version : "; getline(cin,ckon_ana_version);
  cfg_ac << "AC_INIT([" << ckon_ana_name << "], [" << ckon_ana_version << "])" << endl;
  cfg_ac << "m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])" << endl;
  cfg_ac << "AC_CONFIG_AUX_DIR(config)" << endl;
  cfg_ac << "m4_pattern_allow([AM_PROG_AR])" << endl;
  cfg_ac << "m4_ifdef([AM_PROG_AR], [AM_PROG_AR])" << endl;
  cfg_ac << "AM_INIT_AUTOMAKE([-Wall no-define])" << endl;
  cfg_ac << "AC_PROG_CXX" << endl;
  cfg_ac << "AM_PROG_LIBTOOL" << endl;
  cfg_ac << "ROOTLIBS=`$ROOTSYS/bin/root-config --libs`" << endl;
  cfg_ac << "ROOTINCLUDES=`$ROOTSYS/bin/root-config --incdir`" << endl;
  cfg_ac << "ROOTLIBDIR=`$ROOTSYS/bin/root-config --libdir`" << endl;
  cfg_ac << "ROOTGLIBS=`$ROOTSYS/bin/root-config --glibs`" << endl;
  cfg_ac << "AC_SUBST(ROOTLIBS)" << endl;
  cfg_ac << "AC_SUBST(ROOTINCLUDES)" << endl;
  cfg_ac << "AC_SUBST(ROOTGLIBS)" << endl;
  cfg_ac << "AC_SUBST(ROOTLIBDIR)" << endl;
  cfg_ac << "AC_CONFIG_FILES([Makefile])" << endl;
  cfg_ac << "AC_OUTPUT" << endl;
  cfg_ac.close();
}

void utils::writeAutom4teCfg() {
  fs::ofstream atmte;
  atmte.open(".autom4te.cfg");
  atmte << "begin-language: \"Autoconf-without-aclocal-m4\"" << endl;
  atmte << "args: --no-cache" << endl;
  atmte << "end-language: \"Autoconf-without-aclocal-m4\"" << endl;
  atmte.close();
}
