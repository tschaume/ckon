#include "src/helpers/helpers.h"
#include "src/cmdline/cmdline.h"
#include "src/aux/myregex.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/filesystem/operations.hpp>

helpers::helpers(cmdline* cl) : mCl(cl), sd_cnt(0) { }

void helpers::push_subdirs(vector<fs::path>& subdirs) {

  for( fs::recursive_directory_iterator dir_end, dir(mCl->ckon_src_dir); dir != dir_end; ++dir ) {
    fs::path p((*dir).path());
    if ( p.filename().compare(mCl->ckon_core_dir) != 0 ) { // == ckon_core_dir would return 0 !
      dir.no_push(); // don't descend into dir
      if ( !p.filename().compare(mCl->ckon_obsolete_dir) ) continue; // skip ckon_obsolete_dir
      subdirs.push_back(p);
    }
  }

}

void helpers::push_src(vector<fs::path>& headers, vector<fs::path>& sources, vector<fs::path>& progs) {

  for ( fs::recursive_directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
    fs::path p((*dir).path());
    if ( fs::is_directory(p) ) {
      if ( p.filename().compare(mCl->ckon_prog_subdir) == 0 ) {
	dir.no_push();
	for ( fs::directory_iterator pdir_end, pdir(p); pdir != pdir_end; ++pdir ) {
	  if ( (*pdir).path().extension().compare(".cc") == 0 ) progs.push_back(*pdir);
	}
      }
      if ( p.filename().compare(mCl->ckon_macro_subdir) == 0 ) dir.no_push();
      if ( p.filename().compare(".git") == 0 ) dir.no_push();
      if ( mCl->ckon_DontScan.find(p.filename().string()) != string::npos ) dir.no_push();
    }
    if ( p.filename().compare("LinkDef.h") == 0 ) continue;
    if ( p.extension().compare(".h") == 0 ) headers.push_back(p);
    if ( p.extension().compare(".cxx") == 0 ) sources.push_back(p);
  }

}

void helpers::push_obj(const char* obj, vector<string>& objv) {

  for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
    if ( fs::is_directory(*dir) ) continue;
    fs::path p((*dir).path());
    if ( p.filename().compare("LinkDef.h") == 0 ) continue;
    if ( p.extension().compare(".h") == 0 ) {
      if ( mCl->bVerbose ) cout << "Processing file " << p << endl;
      map_type m = myregex::getIndexMap(p,obj);
      for ( map_type::iterator it = m.begin(); it != m.end(); ++it ) {
	if ( mCl->bVerbose ) {
	  cout << "   " << obj << " \"" << (*it).first << "\" found at: " << (*it).second << endl;
	}
	if ( find(objv.begin(),objv.end(),(*it).first) == objv.end() )
	  objv.push_back((*it).first);
      }
    }
  }

}

string helpers::writePkgDefs(const fs::path& linkdef, const vector<fs::path>& files) {

  string out = "pkglib_LTLIBRARIES" + oprnd + "lib/lib" + libname + ".la\n";
  out += "noinst_HEADERS" + oprnd + linkdef.string() + '\n';
  string mkstr = "pkginclude_HEADERS";
  return out + printFilesMk(mkstr,files,oprnd);
}

string helpers::writeLibDefs(const vector<fs::path>& files) {

  string mkstr = "lib_lib" + libname + "_la_SOURCES";
  return printFilesMk(mkstr,files," = ");
}

string helpers::printFilesMk(const string& mkstr, vector<fs::path> files, const string& op) {

  string out = mkstr + op + files.front().string() + '\n';
  boost::sub_range< vector<fs::path> > files_range(files.begin()+1,files.end());
  BOOST_FOREACH( fs::path p, files_range ) { out += mkstr + " += " + p.string() + '\n'; }
  return out;
}

string helpers::writeDict(const fs::path& linkdef, const vector<fs::path>& headers) {

  fs::path dict(sd); dict /= libname; dict += "_Dict.C";
  string out = "nodist_lib_lib" + libname + "_la_SOURCES = " + dict.string() + '\n';
  out += "lib_lib" + libname + "_la_LIBADD = @ROOTLIBS@\n";
  out += dict.string() + ": ";
  BOOST_FOREACH( fs::path p, headers ) { out += p.string() + " "; }
  out += linkdef.string() + '\n';
  out += "\t rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^\n";
  return out + '\n';
}

void helpers::genCoreLibStr(const fs::path& p) {
  // generate string of all libraries to be linked for bin_<prog_name>_LDADD
  myregex::parseIncludes(p,core_lib_string,sd);
  if ( mCl->bVerbose ) { cout << "core_lib_string: " << core_lib_string << endl; }
}

string helpers::writeBinProg(const fs::path& p) {

  string prog_name = p.stem().string();
  string out = "bin_PROGRAMS += bin/" + prog_name + '\n';
  out += "bin_" + prog_name + "_SOURCES = " + p.string() + '\n';
  out += "bin_" + prog_name + "_LDADD = lib/lib" + libname + ".la\n";
  if ( !core_lib_string.empty() )
    out += "bin_" + prog_name + "_LDADD +=" + core_lib_string + '\n';
  out += "bin_" + prog_name;
  out += "_LDADD += -L@ROOTLIBDIR@ @ROOTGLIBS@ @ROOTLIBS@ @LIBS@\n";
  if ( mCl->bBoost ) out += "bin_" + prog_name + "_LDADD += -L$(BOOST_LIB)\n";
  out += "bin_" + prog_name + "_LDADD += -ldl -lSpectrum\n";
  if ( mCl->bRooFit )
    out += "bin_" + prog_name + "_LDADD += -lRooFit -lRooFitCore -lMinuit\n";
  if ( mCl->bPythia )
    out += "bin_" + prog_name + "_LDADD += -lPhysics -lEG -lEGPythia6\n";
  out += "bin_" + prog_name + "_LDFLAGS = -R $(ROOTLIBDIR) -L$(ROOTLIBDIR)\n";
  out += "bin_" + prog_name + " = @ROOTCFLAGS@\n";
  return out;
}

string helpers::writeMakefileAmHd() {
  string out = "AUTOMAKE_OPTIONS = foreign subdir-objects -Wall\n";
  out += "ROOTINCLUDE = @ROOTINCLUDES@\n";
  out += "AM_CPPFLAGS = -I. -I$(srcdir) -I$(pkgincludedir) ";
  if ( mCl->bBoost ) out += "-I$(BOOST_INC) ";
  out += "-I$(ROOTINCLUDE)\n";
  return out + "bin_PROGRAMS = \n";
}
