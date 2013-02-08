// Copyright (c) 2013 Patrick Huck
#ifndef SRC_CLEAN_CLEAN_H_
#define SRC_CLEAN_CLEAN_H_

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

namespace fs = boost::filesystem;

class clean {
  private:
    std::vector<std::string> cont;
    bool isFileNotToDelete(const fs::path&);

  public:
    explicit clean(const std::string&);
    virtual ~clean() {}
    int purge();
};
#endif  // SRC_CLEAN_CLEAN_H_
