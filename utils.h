#ifndef utils_h
#define utils_h

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

class cmdline;

class utils {

  public:
    utils();
    virtual ~utils(void) {};

    static double compareTimeStamps(const fs::path&, const fs::path&);
    static bool checkTimeStamp(const fs::path&, vector<fs::path>);
    static char askYesOrNo(const string);

};
#endif

