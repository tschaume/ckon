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
#include <boost/range/adaptor/map.hpp>
#include <boost/range.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
namespace fs = boost::filesystem;
namespace ad = boost::adaptors;

const char ckon_config_file[] = "ckon_conf.yml";

// defaults only! changed or accepted during 'ckon setup'
string ckon_src_dir = "StRoot";
string ckon_core_dir = "MyCore";
string ckon_obsolete_dir = "Obsolete";
string ckon_exclSuffix = "Gnuplot Options";
string ckon_prog_subdir = "programs";
string ckon_build_dir = "build";
string ckon_install_dir = ckon_build_dir;

#include "StRoot/yaml-cpp/inc/yaml.h"
#include "clopts.h"
#include "clean.h"
#include "regex_search.h"

const char ckon_usage_string[] = 
"usage:\n"
"ckon [-v|--verbose] [-j|ncpu <#cores>]\n"
"[ help | install | clean | setup ]\n"
"\n"
"commands:\n"
"setup		run setup to generate ckon_conf.yml with compile configuration settings\n"
"<none>		runs automated compile using autoconf/automake ( = GNU's make)\n"
"install       	install program/libraries into target directory\n"
"         	[gSystem->Load(\"libMyLibrary\") & bash-completion]\n"
"clean		clean up everything\n"
"help         	show this help\n"
"\n"
"options:\n"
"-v		verbose output\n"
"-j <#cores>	call make with option -j <#cores> (parallel compile)\n"
"\n"
"the following options are implemented to temporarily overwrite yaml config:\n"
"(to include them permanently run 'ckon setup')\n"
"[-p|--pythia] [-r|--roofit] [-s|--suffix] [-d|--doxygen]\n"
"-p         	link with pythia library\n"
"-r         	link with roofit library\n"
"-s		Add suffix + at the end of classname in LinkDef file\n"
"-d		run doxygen (not implemented)\n";

double compareTimeStamps(fs::path f2, fs::path f1) {
  time_t t1 = fs::last_write_time(f1);
  time_t t2 = fs::last_write_time(f2);
  return difftime(t2,t1); // t2 - t1 in seconds
}

bool checkTimeStamp(fs::path file, vector<fs::path> filelist) {
  BOOST_FOREACH( fs::path p, filelist ) {
    if ( compareTimeStamps(p,file) > 0 ) return false;
  }
  return true;
}

int main(int argc, char *argv[]) {

  // init & parse options & arguments, fill container
  clopts.init();
  if ( !clopts.parse(argc,argv) ) {
    cout << "option parsing failed!." << endl;
    return 0;
  }

  // print help and exit
  if ( clopts.bHelp ) {
    printf("%s\n\n",ckon_usage_string);
    return 0;
  }

  // exit if setup requested
  if ( clopts.bSetup ) return 0;

  // purge all files needed for compilation
  if ( clopts.bClean || clopts.bDoxygen ) {
    clcont.init();
    int nf = clcont.purge();
    if ( clopts.bVerbose ) { printf("%d files removed.\n",nf); }
  }

  // exit if clean-up requested. (like make clean)
  if ( clopts.bClean ) return 0; 

  // get list of sub-directories in ckon_src_dir/ for which
  // to generate Makefile_insert and LinkDef.h
  vector<fs::path> subdirs; // absolute paths to subdirs
  for( fs::recursive_directory_iterator dir_end, dir(ckon_src_dir); dir != dir_end; ++dir ) {
    fs::path p((*dir).path());
    if ( p.filename().compare(ckon_core_dir) != 0 ) { // == ckon_core_dir would return 0 !
      dir.no_push(); // don't descend into dir
      if ( !p.filename().compare(ckon_obsolete_dir) ) continue; // skip ckon_obsolete_dir
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
    top_out << "-I$(ROOTINCLUDE) -I/opt/local/include" << endl;
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

    if ( clopts.bVerbose ) cout << sd << endl;

    // get list of all header, source and prog files in current subdir
    vector<fs::path> headers, sources, progs;
    for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
      fs::path p((*dir).path());
      if ( fs::is_directory(p) ) {
	// get list of program names in current subdir
	if ( p.filename().compare(ckon_prog_subdir) == 0 ) {
	  for ( fs::directory_iterator pdir_end, pdir(p); pdir != pdir_end; ++pdir ) {
	    if ( (*pdir).path().extension().compare(".cc") == 0 ) progs.push_back(*pdir);
	  }
	}
	continue;
      }
      if ( p.filename().compare("LinkDef.h") == 0 ) continue;
      if ( p.extension().compare(".h") == 0 ) headers.push_back(p);
      if ( p.extension().compare(".cxx") == 0 ) sources.push_back(p);
    }

    // check time stamp for linkdef file
    fs::path linkdef(sd);
    linkdef /= "LinkDef.h";
    bool redoLinkDef = (
	!fs::exists(linkdef) ||
	!checkTimeStamp(linkdef,headers) ||
	!checkTimeStamp(linkdef,sources) ||
	!checkTimeStamp(linkdef,progs)
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
	  if ( clopts.bVerbose ) cout << "Processing file " << p << endl;
	  string text;
	  fs::ifstream in(p);
	  load_file(text,in);
	  in.close();
	  map_type mc, mn;
	  IndexObjects(mc,text,"class");
	  IndexObjects(mn,text,"namespace");
	  for ( map_type::iterator c = mc.begin(); c != mc.end(); ++c ) {
	    if ( clopts.bVerbose ) {
	      cout << "   class \"" << (*c).first << "\" found at: " << (*c).second << endl;
	    }
	    classes.push_back((*c).first);
	  }
	  for ( map_type::iterator c = mn.begin(); c != mn.end(); ++c ) {
	    if ( clopts.bVerbose ) {
	      cout << "   namespace \"" << (*c).first << "\" found at: " << (*c).second << endl;
	    }
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
	if ( clopts.bSuffix && ckon_exclSuffix.find(cl) == string::npos ) out << "+";
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
	!checkTimeStamp(makefile,headers) ||
	!checkTimeStamp(makefile,sources) ||
	!checkTimeStamp(makefile,progs)
	);

    // write include statement into Makefile.am
    if ( redoMakefileAm ) {
      top_out << "include " << makefile.string() << endl;
    }

    // write Makefile_insert for current subdir
    string oprnd = " = ";
    if ( redoMakefile ) {

      // generate string of all core libraries for bin_<prog_name>_LDADD
      map<fs::path,string> core_lib_string; // one string for each program
      BOOST_FOREACH( fs::path p, progs ) {
	if ( clopts.bVerbose ) cout << "Processing file " << p << endl;
	string text;
	fs::ifstream in(p);
	load_file(text,in);
	in.close();
	map_type mi;
	IndexObjects(mi,text); // default searches for includes
	for ( map_type::iterator i = mi.begin(); i != mi.end(); ++i ) {
	  if ( clopts.bVerbose ) {
	    cout << " < " << (*i).first << " > found at: " << (*i).second << endl;
	  }
	  // compare to subdir, fill tmp string
	  fs::path fndHdr((*i).first); // found header
	  if ( fndHdr.parent_path().compare(sd) == 0 ) continue;
	  if ( clopts.bVerbose ) cout << "  link hdr: " << fndHdr << endl;
	  // build core_lib_string string for prog
	  core_lib_string[p] += " lib/lib";
	  core_lib_string[p] += fndHdr.parent_path().filename().string();
	  core_lib_string[p] += ".la";
	}
      }

      if ( clopts.bVerbose ) {
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
      fs::path dict(sd);
      dict /= libname; dict += "_Dict.C";
      out << "nodist_lib_lib" << libname << "_la_SOURCES = " << dict.string() << endl;
      out << "lib_lib" << libname << "_la_LIBADD = @ROOTLIBS@" << endl;
      out << dict.string() << ": ";
      BOOST_FOREACH( fs::path p, headers ) { out << p.string() << " "; }
      out << linkdef.string() << endl;
      out << "\t rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^" << endl;
      out << endl;

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
	out << "bin_" << prog_name << "_LDADD += -ldl -lSpectrum" << endl;
	if ( clopts.bRooFit )
	  out << "bin_" << prog_name << "_LDADD += -lRooFit -lRooFitCore -lMinuit" << endl;
	if ( clopts.bPythia )
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
  cwd /= ckon_build_dir; // append build dir
  if ( !fs::exists(cwd) ) {
    fs::create_directories(cwd); // mkdir
    fs::current_path(cwd); // chdir
    char export_build[200];
    sprintf(export_build,"export top_builddir=%s",cwd.c_str());
    putenv(export_build);
    fs::path prefix(fs::absolute(ckon_install_dir));
    char config_call[200];
    sprintf(config_call,"../configure --prefix=%s",prefix.c_str());
    system(config_call);
  }

  // always call make when invoking ckon
  char make_call[50];
  sprintf(make_call,"make -j %d %s",clopts.nCpu,(clopts.bInstall)?"install":"");
  fs::current_path(cwd); // chdir
  system(make_call);

  cout << "==> build process finished." << endl;
  return 0;
}
