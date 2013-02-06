#ifndef utils_h
#define utils_h

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

class utils {

  public:
    utils();
    virtual ~utils(void) {};

    double compareTimeStamps(const fs::path&, const fs::path&);
    bool checkTimeStamp(const fs::path&, vector<fs::path>);
    char askYesOrNo(string);
    void writeConfigureAc();
    void writeAutom4teCfg();

};
#endif

