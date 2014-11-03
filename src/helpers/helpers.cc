// Copyright (c) 2013 Patrick Huck
#include "src/helpers/helpers.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>
#include "src/cmdline/cmdline.h"
#include "src/aux/myregex.h"
#include "src/aux/utils.h"

helpers::helpers(const cmdline* cl) : mCl(cl), sd_cnt(0) { }

void helpers::push_subdirs(vpath* subdirs) {
  fs::recursive_directory_iterator d_end;
  for (fs::recursive_directory_iterator d(mCl->ckon_src_dir); d != d_end; ++d) {
    fs::path p((*d).path());
    if ( !fs::is_directory(p) ) continue;
    if ( check_ignore(p) ) continue;  // skip ignored dir's
    if ( utils::isEmptyDir(p) ) continue;  // skip dir w/ no header files
    subdirs->push_back(p);
  }
}

bool helpers::check_ignore(const fs::path& p) {
  BOOST_FOREACH(string i, mCl->ckon_vign) {
    if ( p.string().find(i) != string::npos ) {
      if ( mCl->bVerbose ) {
        std::cout << "found ignore string " << i;
        std::cout << " in " << p.string() << std::endl;
      }
      return true;
    }
  }
  return false;
}

void helpers::push_src(vpath* headers, vpath* sources, vpath* progs) {
  fs::recursive_directory_iterator dir_end;
  for (fs::recursive_directory_iterator d(sd); d != dir_end; ++d) {
    fs::path p((*d).path());
    if ( fs::is_directory(p) ) {
      if ( p.filename().compare(mCl->ckon_prog_subdir) == 0 ) {
        d.no_push();
        fs::directory_iterator pd_end;
        for (fs::directory_iterator pd(p); pd != pd_end; ++pd) {
          if ( (*pd).path().extension().compare(".cc") == 0 ) {
            progs->push_back(*pd);
          }
        }
      }
      if ( p.filename().compare(".git") == 0 ) d.no_push();
      if ( check_ignore(p) ) d.no_push();
    }
    if ( p.filename().compare("LinkDef.h") == 0 ) continue;
    if ( check_ignore(p) ) continue;
    if ( p.extension().compare(".h") == 0 ) headers->push_back(p);
    if ( p.extension().compare(".cxx") == 0 ) sources->push_back(p);
  }
}

void helpers::push_obj(const char* obj, vector<string>* objv) {
  for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
    if ( fs::is_directory(*dir) ) continue;
    fs::path p((*dir).path());
    if ( p.filename().compare("LinkDef.h") == 0 ) continue;
    if ( check_ignore(p) ) continue;
    if ( p.extension().compare(".h") == 0 ) {
      if ( mCl->bVerbose ) std::cout << "Processing file " << p << std::endl;
      map_type m = myregex::getIndexMap(p, obj);
      for ( map_type::iterator it = m.begin(); it != m.end(); ++it ) {
        if ( mCl->bVerbose ) {
          std::cout << "   " << obj << " \"" << (*it).first;
          std::cout << "\" found at: " << (*it).second << std::endl;
        }
        if ( find(objv->begin(), objv->end(), (*it).first) == objv->end() )
          objv->push_back((*it).first);
      }
    }
  }
}

string helpers::writePkgDefs(const fs::path& linkdef, const vpath& files) {
  string out = "pkglib_LTLIBRARIES" + oprnd + "lib/lib" + libname + ".la\n";
  out += "noinst_HEADERS" + oprnd + linkdef.string() + '\n';
  string mkstr = "pkginclude_HEADERS";
  return out + printFilesMk(mkstr, files, oprnd);
}

string helpers::writeLibDefs(const vpath& files) {
  string mkstr = "lib_lib" + libname + "_la_SOURCES";
  return printFilesMk(mkstr, files, " = ");
}

string helpers::printFilesMk(const string& mkstr, vpath fs, const string& op) {
  string out = mkstr + op + fs.front().string() + '\n';
  boost::sub_range< vpath > frange(fs.begin()+1, fs.end());
  BOOST_FOREACH(fs::path p, frange) {
    out += mkstr + " += " + p.string() + '\n';
  }
  return out;
}

string helpers::writeDict(const fs::path& linkdef, const vpath& headers) {
  fs::path dict(sd); dict /= libname; dict += "_Dict.C";
  string out = "nodist_lib_lib" + libname + "_la_SOURCES = ";
  out += dict.string() + '\n';
  out += "lib_lib" + libname + "_la_LIBADD = @ROOTLIBS@\n";
  out += dict.string() + ": ";
  BOOST_FOREACH(fs::path p, headers) { out += p.string() + " "; }
  out += linkdef.string() + '\n';
  out += "\t rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^\n";
  return out + '\n';
}

void helpers::genCoreLibStr(const fs::path& p) {
  // generate string of all libraries to be linked for bin_<prog_name>_LDADD
  myregex::parseIncs(p, core_lib_string, sd);
  if ( mCl->bVerbose ) {
    std::cout << "core_lib_string: " << core_lib_string << std::endl;
  }
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
  out += "bin_" + prog_name + "_LDADD += -ldl -lSpectrum\n";
  if ( mCl->ldadd.find(prog_name) != mCl->ldadd.end() )
    out += "bin_" + prog_name + "_LDADD += " + mCl->ldadd.at(prog_name) + "\n";
  out += "bin_" + prog_name + "_LDFLAGS = -R $(ROOTLIBDIR) -L$(ROOTLIBDIR)\n";
  if ( !(mCl->ckon_boost).empty() ) {
    out += "bin_" + prog_name + "_LDFLAGS += $(BOOST_LDFLAGS)\n";
    vector<string> boost_libs = utils::split(mCl->ckon_boost);
    BOOST_FOREACH(string s, boost_libs) {
      boost::to_upper(s);
      out += "bin_" + prog_name + "_LDFLAGS += $(BOOST_" + s + "_LIB)\n";
    }
  }
  if ( mCl->bYaml ) {
    out += "bin_" + prog_name + "_LDFLAGS += $(YAML_LDFLAGS)\n";
  }
  out += "bin_" + prog_name + " = @ROOTCFLAGS@\n";
  return out;
}

string helpers::writeMakefileAmHd() {
  string out = "AUTOMAKE_OPTIONS = foreign subdir-objects -Wall -Werror\n";
  out += "ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4\n";
  out += "ROOTINCLUDE = @ROOTINCLUDES@\n";
  out += "AM_CPPFLAGS = -I. -I$(srcdir) -I$(pkgincludedir) ";
  if ( !(mCl->ckon_boost).empty() ) out += "$(BOOST_CPPFLAGS) ";
  if ( mCl->bYaml ) out += "$(YAML_CPPFLAGS) ";
  out += "-I$(ROOTINCLUDE)\n";
  return out + "bin_PROGRAMS = \n";
}
