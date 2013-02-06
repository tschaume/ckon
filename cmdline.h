#ifndef cmdline_h
#define cmdline_h

#include <string>
#include <boost/program_options.hpp>
#include "utils.h"

using namespace std;
namespace po = boost::program_options;

class cmdline {

  private:
    po::options_description desc; 
    po::positional_options_description posOpts; 
    po::variables_map vm; 

    utils* ut;

    void addopts();
    void purge();
    void runSetup();

  public:
    cmdline();
    virtual ~cmdline() {};

    bool bHelp;
    bool bVerbose;
    bool bInstall;
    bool bSetup;
    bool bClean;
    bool bPythia;
    bool bRooFit;
    bool bSuffix;
    bool bBoost;
    int nCpu;
    string ckon_config_file;
    string ckon_src_dir;
    string ckon_core_dir;
    string ckon_obsolete_dir;
    string ckon_exclSuffix;
    string ckon_DontScan;
    string ckon_NoRootCint;
    string ckon_prog_subdir;
    string ckon_macro_subdir;
    string ckon_build_dir;
    string ckon_install_dir;

    bool parse(int argc, char *argv[]);

};
#endif

