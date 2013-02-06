#ifndef clean_h
#define clean_h

#include <vector>
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class clean {

  private:
    std::vector<std::string> cont;
    bool isFileNotToDelete (const fs::path&);

  public:
    clean(const std::string&);
    virtual ~clean() {};
    
    int purge();

};
#endif
