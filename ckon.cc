#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
namespace fs = boost::filesystem;

#include "cmdline.h"
#include "regex_search.h"

int main(int argc, char *argv[]) {

  try { 

    // init utils
    utils* ut = new utils();

    // init & parse options & arguments, fill container
    cmdline* clopts = new cmdline();
    if ( !clopts->parse(argc,argv) ) return 0;

    // get list of sub-directories in ckon_src_dir/ for which
    // to generate Makefile_insert and LinkDef.h
    vector<fs::path> subdirs; // absolute paths to subdirs
    for( fs::recursive_directory_iterator dir_end, dir(clopts->ckon_src_dir); dir != dir_end; ++dir ) {
      fs::path p((*dir).path());
      if ( p.filename().compare(clopts->ckon_core_dir) != 0 ) { // == ckon_core_dir would return 0 !
	dir.no_push(); // don't descend into dir
	if ( !p.filename().compare(clopts->ckon_obsolete_dir) ) continue; // skip ckon_obsolete_dir
	subdirs.push_back(p);
      }
    }

    // if Makefile.am doesn't exist, generate Makefile.am
    bool redoMakefileAm = !fs::exists("Makefile.am");
    fs::ofstream top_out;
    if ( redoMakefileAm ) {
      // create & init Makefile.am
      top_out.open("Makefile.am");
      top_out << "AUTOMAKE_OPTIONS = foreign subdir-objects -Wall" << endl;
      top_out << "ROOTINCLUDE = @ROOTINCLUDES@" << endl;
      top_out << "AM_CPPFLAGS = -I. -I$(srcdir) -I$(pkgincludedir) ";
      if ( clopts->bBoost ) top_out << "-I$(BOOST_INC) ";
      top_out << "-I$(ROOTINCLUDE)" << endl;
      top_out << "bin_PROGRAMS = " << endl;
    }

    // generate LinkDef.h and Makefile_insert in all subdirs
    // *****************************************************
    int subdir_cnt = 0;
    BOOST_FOREACH( fs::path sd, subdirs ) {

      // check if subdir "empty" (no header files). If so, skip.
      bool subdir_empty = true;
      for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
	if ( (*dir).path().extension().compare(".h") == 0 ) {
	  subdir_empty = false;
	  break;
	}
      }
      if ( subdir_empty ) continue;

      if ( clopts->bVerbose ) cout << sd << endl;

      // get list of all header, source and prog files in current subdir
      vector<fs::path> headers, sources, progs;
      for ( fs::recursive_directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
	fs::path p((*dir).path());
	if ( fs::is_directory(p) ) {
	  if ( p.filename().compare(clopts->ckon_prog_subdir) == 0 ) dir.no_push();
	  if ( p.filename().compare(clopts->ckon_macro_subdir) == 0 ) dir.no_push();
	  if ( p.filename().compare(".git") == 0 ) dir.no_push();
	  if ( clopts->ckon_DontScan.find(p.filename().string()) != string::npos ) dir.no_push();
	}
	if ( p.filename().compare("LinkDef.h") == 0 ) continue;
	if ( p.extension().compare(".h") == 0 ) headers.push_back(p);
	if ( p.extension().compare(".cxx") == 0 ) sources.push_back(p);
      }
      for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
	fs::path p((*dir).path());
	if ( fs::is_directory(p) && p.filename().compare(clopts->ckon_prog_subdir) == 0 ) {
	  for ( fs::directory_iterator pdir_end, pdir(p); pdir != pdir_end; ++pdir ) {
	    if ( (*pdir).path().extension().compare(".cc") == 0 ) progs.push_back(*pdir);
	  }
	}
      }

      // check time stamp for linkdef file
      fs::path linkdef(sd);
      linkdef /= "LinkDef.h";
      bool redoLinkDef = (
	  !fs::exists(linkdef) ||
	  !ut->checkTimeStamp(linkdef,headers) ||
	  !ut->checkTimeStamp(linkdef,sources) ||
	  !ut->checkTimeStamp(linkdef,progs)
	  );

      // write LinkDef.h for current subdir
      if ( redoLinkDef ) {

	// get lists of all classes & namespaces for current subdir
	vector<string> classes, namespaces;
	for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
	  if ( fs::is_directory(*dir) ) continue;
	  fs::path p((*dir).path());
	  if ( p.filename().compare("LinkDef.h") == 0 ) continue;
	  if ( p.extension().compare(".h") == 0 ) {
	    if ( clopts->bVerbose ) cout << "Processing file " << p << endl;
	    string text;
	    fs::ifstream in(p);
	    load_file(text,in);
	    in.close();
	    map_type mc, mn;
	    IndexObjects(mc,text,"class");
	    IndexObjects(mn,text,"namespace");
	    for ( map_type::iterator c = mc.begin(); c != mc.end(); ++c ) {
	      if ( clopts->bVerbose ) {
		cout << "   class \"" << (*c).first << "\" found at: " << (*c).second << endl;
	      }
	      if ( find(classes.begin(),classes.end(),(*c).first) == classes.end() )
		classes.push_back((*c).first);
	    }
	    for ( map_type::iterator c = mn.begin(); c != mn.end(); ++c ) {
	      if ( clopts->bVerbose ) {
		cout << "   namespace \"" << (*c).first << "\" found at: " << (*c).second << endl;
	      }
	      if ( find(namespaces.begin(),namespaces.end(),(*c).first) == namespaces.end() )
		namespaces.push_back((*c).first);
	    }
	  }
	}

	fs::ofstream out;
	out.open(linkdef);
	out << "#ifdef __CINT__" << endl;
	out << "#pragma link off all globals;" << endl;
	out << "#pragma link off all classes;" << endl;
	out << "#pragma link off all functions;" << endl;
	out << "#pragma link C++ nestedclasses;" << endl;
	out << "#pragma link C++ nestedtypedefs;" << endl;
	BOOST_FOREACH( string ns, namespaces ) {
	  out << "#pragma link C++ namespace " << ns << ";" << endl;
	}
	BOOST_FOREACH( string cl, classes ) {
	  out << "#pragma link C++ class " << cl;
	  if ( clopts->bSuffix && clopts->ckon_exclSuffix.find(cl) == string::npos ) out << "+";
	  out << ";" << endl;
	}
	out << "#endif" << endl;
	out.close();
      }

      // check time stamp for makefile_insert
      fs::path makefile(sd);
      makefile /= "Makefile_insert";
      bool redoMakefile = (
	  !fs::exists(makefile) ||
	  !ut->checkTimeStamp(makefile,headers) ||
	  !ut->checkTimeStamp(makefile,sources) ||
	  !ut->checkTimeStamp(makefile,progs)
	  );

      // write include statement into Makefile.am
      if ( redoMakefileAm ) {
	top_out << "include " << makefile.string() << endl;
      }

      // write Makefile_insert for current subdir
      string oprnd = " = ";
      if ( redoMakefile ) {

	// generate string of all libraries to be linked for bin_<prog_name>_LDADD
	map<fs::path,string> core_lib_string; // one string for each program
	BOOST_FOREACH( fs::path p, progs ) { parseIncludes(p,core_lib_string[p],sd); }

	if ( clopts->bVerbose ) {
	  BOOST_FOREACH( fs::path p, progs ) {
	    cout << "core_lib_string[" << p << "]:" << endl;
	    cout << "  " << core_lib_string[p] << endl;
	  }
	}

	fs::ofstream out;
	out.open(makefile);
	if ( subdir_cnt > 0 ) oprnd = " += ";
	string libname = sd.filename().string();
	out << "pkglib_LTLIBRARIES" << oprnd << "lib/lib" << libname << ".la" << endl;
	out << "pkginclude_HEADERS" << oprnd << headers.front().string() << endl;
	boost::sub_range< vector<fs::path> > headers_range(headers.begin()+1,headers.end());
	BOOST_FOREACH( fs::path p, headers_range ) {
	  out << "pkginclude_HEADERS += " << p.string() << endl;
	}
	out << "noinst_HEADERS" << oprnd << linkdef.string() << endl;
	out << "lib_lib" << libname << "_la_SOURCES = " << sources.front().string() << endl;
	boost::sub_range< vector<fs::path> > sources_range(sources.begin()+1,sources.end());
	BOOST_FOREACH( fs::path p, sources_range ) {
	  out << "lib_lib" << libname << "_la_SOURCES += " << p.string() << endl;
	}
	if ( clopts->ckon_NoRootCint.find(sd.filename().string()) == string::npos ) { // no dict if requested
	  fs::path dict(sd);
	  dict /= libname; dict += "_Dict.C";
	  out << "nodist_lib_lib" << libname << "_la_SOURCES = " << dict.string() << endl;
	  out << "lib_lib" << libname << "_la_LIBADD = @ROOTLIBS@" << endl;
	  out << dict.string() << ": ";
	  BOOST_FOREACH( fs::path p, headers ) { out << p.string() << " "; }
	  out << linkdef.string() << endl;
	  out << "\t rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^" << endl;
	  out << endl;
	}

	BOOST_FOREACH( fs::path p, progs ) { 
	  string prog_name = p.stem().string();
	  out << "bin_PROGRAMS += bin/" << prog_name << endl;
	  out << "bin_" << prog_name << "_SOURCES = " << p.string() << endl;
	  out << "bin_" << prog_name << "_LDADD = lib/lib" << libname << ".la" << endl;
	  if ( !core_lib_string[p].empty() ) {
	    out << "bin_" << prog_name << "_LDADD +=" << core_lib_string[p] << endl;
	  }
	  out << "bin_" << prog_name;
	  out << "_LDADD += -L@ROOTLIBDIR@ @ROOTGLIBS@ @ROOTLIBS@ @LIBS@" << endl;
	  if ( clopts->bBoost ) out << "bin_" << prog_name << "_LDADD += -L$(BOOST_LIB)" << endl;
	  out << "bin_" << prog_name << "_LDADD += -ldl -lSpectrum" << endl;
	  if ( clopts->bRooFit )
	    out << "bin_" << prog_name << "_LDADD += -lRooFit -lRooFitCore -lMinuit" << endl;
	  if ( clopts->bPythia )
	    out << "bin_" << prog_name << "_LDADD += -lPhysics -lEG -lEGPythia6" << endl;
	  out << "bin_" << prog_name << "_LDFLAGS = -R $(ROOTLIBDIR) -L$(ROOTLIBDIR)" << endl;
	  out << "bin_" << prog_name << " = @ROOTCFLAGS@" << endl;
	}

	out.close();

      }

      subdir_cnt++;
    }

    if ( redoMakefileAm ) top_out.close();

    // run autoconf if configure script doesn't exist
    if ( !fs::exists("configure") ) system("autoreconf -v --force --install");

    // if builddir doesn't exist, create it and run configure
    // else, just switch to builddir
    fs::path cwd(fs::current_path()); // pwd
    cwd /= clopts->ckon_build_dir; // append build dir
    if ( !fs::exists(cwd) ) {
      fs::create_directories(cwd); // mkdir
      fs::current_path(cwd); // chdir
      char export_build[200];
      sprintf(export_build,"export top_builddir=%s",cwd.c_str());
      putenv(export_build);
      fs::path prefix(fs::absolute(clopts->ckon_install_dir));
      char config_call[200];
      sprintf(config_call,"../configure --prefix=%s",prefix.c_str());
      system(config_call);
    }

    // always call make when invoking ckon
    char make_call[50];
    sprintf(make_call,"make -j %d %s",clopts->nCpu,(clopts->bInstall)?"install":"");
    fs::current_path(cwd); // chdir
    system(make_call);

    cout << "==> build process finished." << endl;

  } 
  catch(exception& e) { 
    cerr << "Unhandled Exception reached the top of main: " 
      << e.what() << ", application will now exit" << endl; 
    return 1; 
  } 

  return 0;
}
