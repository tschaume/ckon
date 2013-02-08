// Copyright 2013 Patrick Huck

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <string>
#include <vector>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

#include "src/aux/utils.h"
#include "src/cmdline/cmdline.h"
#include "src/helpers/helpers.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
namespace fs = boost::filesystem;

int main(int argc, char *argv[]) {

  try { 

    // init & parse options & arguments, fill container
    cmdline* clopts = new cmdline();
    if ( !clopts->parse(argc,argv) ) return 0;

    // init helpers w/ command line options
    helpers* hlp = new helpers(clopts);

    // get list of sub-directories in ckon_src_dir/ for which
    // to generate Makefile_insert and LinkDef.h
    vector<fs::path> subdirs; // absolute paths to subdirs
    hlp->push_subdirs(subdirs);

    // if Makefile.am doesn't exist, generate Makefile.am
    bool redoMakefileAm = !fs::exists("Makefile.am");
    fs::ofstream top_out;
    if ( redoMakefileAm ) {
      // create & init Makefile.am
      top_out.open("Makefile.am");
      top_out << hlp->writeMakefileAmHd();
    }

    // loop all subdirs
    BOOST_FOREACH( fs::path sd, subdirs ) {

      // check if subdir "empty" (no header files). If so, skip.
      if ( utils::isEmptyDir(sd) ) continue;
      if ( clopts->bVerbose ) cout << sd << endl;

      // set subdir for helpers, set libname
      // increase subdir counter, set oprnd
      hlp->init_subdir(sd);

      // get list of all header, source and prog files in current subdir
      vector<fs::path> headers, sources, progs;
      hlp->push_src(headers,sources,progs);

      // check time stamp for linkdef file
      fs::path linkdef(sd);
      linkdef /= "LinkDef.h";
      bool redoLinkDef = (
	  !fs::exists(linkdef) ||
	  !utils::checkTimeStamp(linkdef,headers) ||
	  !utils::checkTimeStamp(linkdef,sources) ||
	  !utils::checkTimeStamp(linkdef,progs)
	  );

      // write LinkDef.h for current subdir
      if ( redoLinkDef ) {

	// get lists of all classes & namespaces for current subdir
	vector<string> classes, namespaces;
	hlp->push_obj("class",classes);
	hlp->push_obj("namespace",namespaces);

	// write linkdef
	fs::ofstream out;
	out.open(linkdef);
	out << utils::writeLinkDefHd();
	BOOST_FOREACH( string ns, namespaces ) {
	  out << "#pragma link C++ namespace " << ns << ";" << endl;
	}
	BOOST_FOREACH( string cl, classes ) {
	  out << "#pragma link C++ class " << cl << hlp->getSuffix(cl) << ";" << endl;
	}
	out << "#endif" << endl;
	out.close();

      }

      // check time stamp for makefile_insert
      fs::path makefile(sd);
      makefile /= "Makefile_insert";
      bool redoMakefile = (
	  !fs::exists(makefile) ||
	  !utils::checkTimeStamp(makefile,headers) ||
	  !utils::checkTimeStamp(makefile,sources) ||
	  !utils::checkTimeStamp(makefile,progs)
	  );

      // write include statement into Makefile.am
      if ( redoMakefileAm ) {
	top_out << "include " << makefile.string() << endl;
      }

      // write Makefile_insert for current subdir
      if ( redoMakefile ) {

	// write makefile
	fs::ofstream out;
	out.open(makefile);
	out << hlp->writePkgDefs(linkdef,headers);
	out << hlp->writeLibDefs(sources);
	if ( hlp->genDict() ) { out << hlp->writeDict(linkdef,headers); }
	BOOST_FOREACH( fs::path p, progs ) { 
	  hlp->genCoreLibStr(p);
	  out << hlp->writeBinProg(p);
	}
	out.close();

      }

    } // end of subdir loop

    cout << hlp->getNrSubdirs() << " sub-directories processed." << endl;

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
