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
