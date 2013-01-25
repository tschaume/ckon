
bool isFileNotToDelete (const fs::path& filename) {
  if ( filename.compare("LinkDef.h") == 0 ) return false;
  if ( filename.compare("Makefile_insert") == 0 ) return false;
  return true;
}

struct CleanUpContainer {
  vector<string> cont;
  vector<fs::directory_entry> decont;
  void init() {
    for( fs::recursive_directory_iterator dir_end, dir(ckon_src_dir); dir != dir_end; ++dir ) {
      fs::path p((*dir).path());
      fs::path filename(p.filename());
      if ( isFileNotToDelete(filename) ) continue;
      else cont.push_back(p.c_str());
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
  int purge() {
    int nfiles = 0;
    vector<string>::iterator it;
    for ( it = cont.begin(); it != cont.end(); ++it ) {
      if ( ! fs::exists((*it).c_str()) ) continue;
      cout << "remove " << (*it).c_str() << endl;
      nfiles += fs::remove_all((*it).c_str());
    }
    return nfiles;
  }
} clcont;

