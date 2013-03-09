// Copyright (c) 2013 Patrick Huck
#ifndef SRC_AUX_UTILS_H_
#define SRC_AUX_UTILS_H_

#include <boost/filesystem.hpp>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

class cmdline;

class utils {
  public:
    utils();
    virtual ~utils(void) {}

    static double compareTimeStamps(const fs::path&, const fs::path&);
    static bool checkTimeStamp(const fs::path&, const std::vector<fs::path>&);
    static char askYesOrNo(const std::string&);
    static bool isEmptyDir(const fs::path&);
    static std::string writeLinkDefHd();
    static std::vector<std::string> split(const std::string&);
    static std::string getM4Url(const std::string&);
};
#endif  // SRC_AUX_UTILS_H_
