// Copyright (c) 2013 Patrick Huck
#ifndef SRC_CMDLINE_CMDLINE_H_
#define SRC_CMDLINE_CMDLINE_H_

#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <map>

using std::string;
using std::vector;
using std::map;
namespace po = boost::program_options;

class cmdline {
  private:
    string ckon_cmd;
    void purge();
    void runSetup();
    void writeCfgFile();
    void writeConfigureAc();
    void writeAutom4teCfg();

  public:
    cmdline();
    virtual ~cmdline() {}

    bool bHelp;
    bool bVerbose;
    bool bInstall;
    bool bSetup;
    bool bClean;
    bool bSuffix;
    bool bBoost;
    string nCpu;
    string ckon_config_file;
    string ckon_src_dir;
    string ckon_exclSuffix;
    string ckon_NoRootCint;
    string ckon_prog_subdir;
    string ckon_build_dir;
    string ckon_install_dir;
    string ckon_cppflags;
    string ckon_ignore_file;

    map<string, string> ldadd;

    vector<string> ckon_vign;

    bool parse(int argc, char *argv[]);
};
#endif  // SRC_CMDLINE_CMDLINE_H_
