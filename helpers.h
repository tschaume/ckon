#ifndef helpers_h
#define helpers_h

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

class cmdline;

class helpers {
  private:
    cmdline* mCl;

  public:
    helpers(cmdline*);
    virtual ~helpers() {};

    void push_subdirs(vector<fs::path>&);

};
#endif
