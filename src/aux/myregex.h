#ifndef myregex_h
#define myregex_h
// extended version of
// http://www.boost.org/doc/libs/1_31_0/libs/regex/example/snippets/regex_search_example.cpp
// purpose:
// takes the contents of a file in the form of a string
// and searches for all the C++ class definitions, storing
// their locations in a map of strings/int's

#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std;
namespace fs = boost::filesystem;

typedef map < string, string::difference_type, less<string> > map_type;

class myregex {
  private:
    static void IndexObjects(map_type&, const string&, const char* obj = NULL);
    static void load_file(string&, istream&);
    static string getLibString(const fs::path& p) {
      return " lib/lib" + p.stem().string() + ".la";
    }

  public:
    myregex() {};
    virtual ~myregex() {};

    static map_type getIndexMap(const fs::path&, const char* obj = NULL);
    static void parseIncludes(const fs::path&, string&, const fs::path&);

};
#endif

