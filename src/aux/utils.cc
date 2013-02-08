#include "src/aux/utils.h"
#include "src/cmdline/cmdline.h"

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

char utils::askYesOrNo(const string question) {
  char type;
  do {
    cout << question << "? [y/n] ";
    cin >> type; cin.ignore();
  }
  while( !cin.fail() && type!='y' && type!='n' );
  return type;
}

bool utils::isEmptyDir(fs::path& sd) {
  for ( fs::directory_iterator dir_end, dir(sd); dir != dir_end; ++dir ) {
    if ( (*dir).path().extension().compare(".h") == 0 ) {
      return false;
    }
  }
  return true;
}

string utils::writeLinkDefHd() {
  string out = "#ifdef __CINT__\n";
  out += "#pragma link off all globals;\n";
  out += "#pragma link off all classes;\n";
  out += "#pragma link off all functions;\n";
  out += "#pragma link C++ nestedclasses;\n";
  return out + "#pragma link C++ nestedtypedefs;\n";
}
