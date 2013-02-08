// Copyright (c) 2013 Patrick Huck
#ifndef SRC_AUX_MYREGEX_H_
#define SRC_AUX_MYREGEX_H_
// extended version of
// http://www.boost.org/doc/libs/1_31_0/libs/
//     regex/example/snippets/regex_search_example.cpp
// purpose:
// takes the contents of a file in the form of a string
// and searches for all the C++ class definitions, storing
// their locations in a map of strings/int's

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include <map>
#include <string>
#include <functional>

using std::string;
namespace fs = boost::filesystem;

typedef std::map< string, string::difference_type, std::less<string> > map_type;

class myregex {
  private:
    static void IndexObjects(map_type&, const string&, const char* obj = NULL);
    static void load_file(string&, std::istream&);
    static string getLibString(const fs::path& p) {
      return " lib/lib" + p.stem().string() + ".la";
    }

  public:
    myregex() {}
    virtual ~myregex() {}

    static map_type getIndexMap(const fs::path&, const char* obj = NULL);
    static void parseIncs(const fs::path&, string&, const fs::path&);
};
#endif  // SRC_AUX_MYREGEX_H_
