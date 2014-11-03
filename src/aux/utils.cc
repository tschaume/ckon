// Copyright (c) 2013 Patrick Huck
#include "src/aux/utils.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include "src/cmdline/cmdline.h"

utils::utils() {}

double utils::compareTimeStamps(const fs::path& f2, const fs::path& f1) {
  time_t t1 = fs::last_write_time(f1);
  time_t t2 = fs::last_write_time(f2);
  return difftime(t2, t1);  // t2 - t1 in seconds
}

bool utils::checkTimeStamp(const fs::path& file,
    const std::vector<fs::path>& filelist) {
  BOOST_FOREACH(fs::path p, filelist) {
    if ( compareTimeStamps(p, file) > 0 ) return false;
  }
  return true;
}

char utils::askYesOrNo(const std::string& question) {
  char type;
  do {
    std::cout << question << "? [y/n] ";
    std::cin >> type; std::cin.ignore();
  } while (!std::cin.fail() && type != 'y' && type != 'n');
  return type;
}

bool utils::isEmptyDir(const fs::path& sd) {
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

std::vector<std::string> utils::split(const std::string& vi) {
  std::string v(vi);
  boost::erase_all(v, "\"");
  std::vector<std::string> vo;
  boost::split(vo, v, boost::is_any_of(" "));
  return vo;
}

std::string utils::getM4Url(const std::string& s) {
  std::string u("http://git.savannah.gnu.org/gitweb/");
  u += "?p=autoconf-archive.git;a=blob_plain;f=m4/ax_boost_";
  return u + s + ".m4";
}
