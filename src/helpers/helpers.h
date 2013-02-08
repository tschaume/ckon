#ifndef helpers_h
#define helpers_h

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "src/cmdline/cmdline.h"

using namespace std;
namespace fs = boost::filesystem;

class helpers {
  private:
    cmdline* mCl;
    fs::path sd;
    string oprnd;
    string libname;
    int sd_cnt;
    string core_lib_string;

    string printFilesMk(const string&, vector<fs::path>, const string&);
    bool hasSuffix(const string& cl) {
      return ( mCl->bSuffix && mCl->ckon_exclSuffix.find(cl) == string::npos );
    }

  public:
    helpers(cmdline*);
    virtual ~helpers() {};

    void init_subdir(const fs::path& s) { // called for each subdir
      sd = s;
      libname = s.filename().string();
      oprnd = (sd_cnt > 0) ? " += " : " = ";
      sd_cnt++;
    }

    bool genDict() {
      string fn = sd.filename().string();
      return ( mCl->ckon_NoRootCint.find(fn) == string::npos );
    }
    string getSuffix(const string& cl) { return ( hasSuffix(cl) ) ? "+" : ""; }

    void push_subdirs(vector<fs::path>&);
    void push_src(vector<fs::path>&, vector<fs::path>&, vector<fs::path>&);
    void push_obj(const char*, vector<string>&);
    void genCoreLibStr(const fs::path& p);
    string writePkgDefs(const fs::path&, const vector<fs::path>&);
    string writeLibDefs(const vector<fs::path>&);
    string writeDict(const fs::path&, const vector<fs::path>&);
    string writeBinProg(const fs::path&);
    string writeMakefileAmHd();
    int getNrSubdirs() { return sd_cnt; }

};
#endif
