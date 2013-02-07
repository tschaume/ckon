#include "helpers.h"
#include "cmdline.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>

helpers::helpers(cmdline* cl) : mCl(cl) { }

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

void helpers::push_src(const fs::path& sd,
    vector<fs::path>& headers, vector<fs::path>& sources, vector<fs::path>& progs
    ) {
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
