// Copyright (c) 2013 Patrick Huck
#ifndef SRC_HELPERS_HELPERS_H_
#define SRC_HELPERS_HELPERS_H_

#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include "src/cmdline/cmdline.h"

using std::string;
using std::vector;
namespace fs = boost::filesystem;

typedef vector<fs::path> vpath;

class helpers {
  private:
    const cmdline* mCl;
    fs::path sd;
    string oprnd;
    string libname;
    int sd_cnt;
    string core_lib_string;

    string printFilesMk(const string&, vpath, const string&);
    bool hasSuffix(const string& cl) {
      return ( mCl->bSuffix && mCl->ckon_exclSuffix.find(cl) == string::npos );
    }
    bool check_ignore(const fs::path&);

  public:
    explicit helpers(const cmdline* cl);
    virtual ~helpers() {}

    void init_subdir(const fs::path& s) {  // called for each subdir
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

    void push_subdirs(vpath*);
    void push_src(vpath*, vpath*, vpath*);
    void push_obj(const char*, vector<string>*);
    void genCoreLibStr(const fs::path& p);
    string writePkgDefs(const fs::path&, const vpath&);
    string writeLibDefs(const vpath&);
    string writeDict(const fs::path&, const vpath&);
    string writeBinProg(const fs::path&);
    string writeMakefileAmHd();
    int getNrSubdirs() { return sd_cnt; }
};
#endif  // SRC_HELPERS_HELPERS_H_
