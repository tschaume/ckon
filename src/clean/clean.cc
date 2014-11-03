// Copyright (c) 2013 Patrick Huck
#include "src/clean/clean.h"
#include <iostream>

clean::clean(const std::string& src_dir) {
  fs::recursive_directory_iterator dir_end;
  for (fs::recursive_directory_iterator d(src_dir); d != dir_end; ++d) {
    fs::path p((*d).path());
    fs::path filename(p.filename());
    if ( isFileNotToDelete(filename) ) {
      continue;
    } else { cont.push_back(p.c_str()); }
  }
  cont.push_back("build");
  cont.push_back("autom4te.cache");
  cont.push_back("config");
  cont.push_back(".lib");
  cont.push_back("aclocal.m4");
  cont.push_back("configure");
  cont.push_back("Makefile.in");
  cont.push_back("Makefile.am");
  cont.push_back(".autom4te.cfg");
}

bool clean::isFileNotToDelete(const fs::path& filename) {
  if ( filename.compare("LinkDef.h") == 0 ) return false;
  if ( filename.compare("Makefile_insert") == 0 ) return false;
  return true;
}

int clean::purge() {
  int nfiles = 0;
  std::vector<std::string>::iterator it;
  for ( it = cont.begin(); it != cont.end(); ++it ) {
    if ( !fs::exists((*it).c_str()) ) continue;
    std::cout << "remove " << (*it).c_str() << std::endl;
    nfiles += fs::remove_all((*it).c_str());
  }
  return nfiles;
}
