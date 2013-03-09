// Copyright (c) 2013 Patrick Huck
#include "src/cmdline/cmdline.h"
#include <boost/foreach.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/algorithm/string.hpp>
#include "src/clean/clean.h"
#include "src/aux/utils.h"

using std::cout;
using std::endl;
using std::ostream;
using std::basic_string;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

typedef io::tee_device<ostream, fs::ofstream> Tee;
typedef io::stream<Tee> TeeStream;

cmdline::cmdline()
: ckon_cmd(""), nCpu("1"), ckon_config_file("ckon.cfg"), ckon_src_dir("StRoot"),
  ckon_exclSuffix("Gnuplot Options"), ckon_NoRootCint("YamlCpp"),
  ckon_prog_subdir("programs"), ckon_build_dir("build"),
  ckon_install_dir("build"), ckon_cppflags("-Wall"),
  ckon_ignore_file("ckonignore")
{ }

void cmdline::purge() {
  clean clcont(ckon_src_dir);
  printf("%d files removed.\n", clcont.purge());
}

void cmdline::runSetup() {
  if ( fs::exists(ckon_config_file) || fs::exists("configure.ac") ) {
    cout << ckon_config_file << " or configure.ac already exist(s)!" << endl;
    if ( utils::askYesOrNo("remove and start over") == 'n' ) return;
    fs::remove(ckon_config_file);
    fs::remove("configure.ac");
    fs::remove(".autom4te.cfg");
  }

  writeCfgFile();
  writeConfigureAc();
  writeAutom4teCfg();

  cout << endl << "setup done. check " << ckon_config_file << endl;
}

bool cmdline::parse(int argc, char *argv[]) {
  po::options_description generic("Generic Options");
  generic.add_options()
    ("help,h", po::bool_switch(&bHelp), "show this help")
    ("verbose,v", po::bool_switch(&bVerbose), "verbose output")
    (",j", po::value<string>(&nCpu), "call make w/ -j <#cores>")
    ("ckon_cmd", po::value<string>(&ckon_cmd), "setup | clean | install | dry");

  po::options_description config("Configuration");
  config.add_options()
    ("suffix,s", po::value<bool>(&bSuffix), "add suffix + in LinkDef.h (bool)")
    ("boost,b", po::value<bool>(&bBoost), "include BOOST_INC/BOOST_LIB (bool)")
    ("ckon.src_dir", po::value<string>(&ckon_src_dir), "source dir")
    ("ckon.exclSuffix", po::value<string>(&ckon_exclSuffix), "no + suffix")
    ("ckon.NoRootCint", po::value<string>(&ckon_NoRootCint), "no dictionary")
    ("ckon.prog_subdir", po::value<string>(&ckon_prog_subdir), "progs subdir")
    ("ckon.build_dir", po::value<string>(&ckon_build_dir), "build dir")
    ("ckon.install_dir", po::value<string>(&ckon_install_dir), "install dir")
    ("ckon.cppflags", po::value<string>(&ckon_cppflags), "add CPPFLAGS");

  po::options_description userOpts;
  userOpts.add(generic).add(config);

  po::positional_options_description posOpts;
  posOpts.add("ckon_cmd", 1);

  po::variables_map vm;

  try {
    // parse command line
    // don't allow unregistered on command line
    po::parsed_options cl_parsed =
      po::command_line_parser(argc, argv).options(userOpts)
      .positional(posOpts).run();
    po::store(cl_parsed, vm);
    // parse config file
    if ( fs::exists(ckon_config_file) ) {
      // parse & allow unregistered
      po::parsed_options file_parsed =
        po::parse_config_file<char>(ckon_config_file.c_str(), config, true);
      po::store(file_parsed, vm);
      // collect & parse unrecognized options
      BOOST_FOREACH(po::basic_option<char> opt, file_parsed.options) {
        if ( opt.unregistered ) {
          vector<string> v;
          boost::split(v, opt.string_key, boost::is_any_of("."));
          BOOST_FOREACH(basic_string<char> c, opt.value) ldadd[v.back()] += c;
        }
      }
    }

    // parse ckonignore file
    if ( fs::exists(ckon_ignore_file) ) {
      fs::ifstream ifs(ckon_ignore_file.c_str());
      string line;
      while ( std::getline(ifs, line) ) ckon_vign.push_back(line);
    }

    po::notify(vm);  // throws on error

    if ( bHelp ) {  // --help option
      cout << "ckon -- automatic ROOT analyses compiler & linker" << endl;
      cout << userOpts << endl;
      cout << "\nIn addition, unregistered options of the form";
      cout << "\nldadd.prog_name are allowed to use for adding";
      cout << "\nLDFLAGS to the linker of specific programs. The";
      cout << "\ngiven string/value is added verbatim in LDADD.";
      cout << "\nUnregistered options are only allowed in ckon.cfg\n";
      return false;
    }

    bSetup = !ckon_cmd.compare("setup");
    bClean = !ckon_cmd.compare("clean");
    bInstall = !ckon_cmd.compare("install");
    bDry = !ckon_cmd.compare("dry");

    if ( bSetup ) { cout << "run setup" << endl; runSetup(); return false; }
    if ( bClean ) { purge(); return false; }
    if ( !bInstall && !bDry && !ckon_cmd.empty() ) {
      cout << "unknown ckon command: " << ckon_cmd << endl;
      return false;
    }

    return true;
  }
  catch(const po::error& e) {
    std::cerr << "ERROR: " << e.what() << endl << endl << generic << endl;
    return false;
  }
}

void cmdline::writeCfgFile() {
  fs::ofstream cfgfile(ckon_config_file);
  Tee tee(cout, cfgfile);
  TeeStream both(tee);
  both << "boost=" << bBoost << endl;
  both << "suffix=" << bSuffix << endl;
  both << "[ckon]" << endl;
  both << "src_dir=" << ckon_src_dir << endl;
  both << "prog_subdir=" << ckon_prog_subdir << endl;
  both << "build_dir=" << ckon_build_dir << endl;
  both << "install_dir=" << ckon_install_dir << endl;
  both << "exclSuffix=\"" << ckon_exclSuffix << "\"" << endl;
  both << "NoRootCint=" << ckon_NoRootCint << endl;
  both << "cppflags=\"" << ckon_cppflags << "\"" << endl;
  both.close();
}

void cmdline::writeConfigureAc() {
  string cfg_ac_str = "AC_INIT([ana], [0.0])\n";
  cfg_ac_str += "m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])\n";
  cfg_ac_str += "AC_CONFIG_AUX_DIR(config)\n";
  cfg_ac_str += "m4_pattern_allow([AM_PROG_AR])\n";
  cfg_ac_str += "m4_ifdef([AM_PROG_AR], [AM_PROG_AR])\n";
  cfg_ac_str += "AM_INIT_AUTOMAKE([-Wall no-define])\n";
  cfg_ac_str += "AC_PROG_CXX\n";
  cfg_ac_str += "AM_PROG_LIBTOOL\n";
  cfg_ac_str += "ROOTLIBS=`$ROOTSYS/bin/root-config --libs`\n";
  cfg_ac_str += "ROOTINCLUDES=`$ROOTSYS/bin/root-config --incdir`\n";
  cfg_ac_str += "ROOTLIBDIR=`$ROOTSYS/bin/root-config --libdir`\n";
  cfg_ac_str += "ROOTGLIBS=`$ROOTSYS/bin/root-config --glibs`\n";
  cfg_ac_str += "AC_SUBST(ROOTLIBS)\n";
  cfg_ac_str += "AC_SUBST(ROOTINCLUDES)\n";
  cfg_ac_str += "AC_SUBST(ROOTGLIBS)\n";
  cfg_ac_str += "AC_SUBST(ROOTLIBDIR)\n";
  cfg_ac_str += "AC_CONFIG_FILES([Makefile])\n";
  cfg_ac_str += "AC_OUTPUT\n";
  fs::ofstream cfg_ac;
  cfg_ac.open("configure.ac");
  cfg_ac << cfg_ac_str;
  cfg_ac.close();
}

void cmdline::writeAutom4teCfg() {
  string atmte_str = "begin-language: \"Autoconf-without-aclocal-m4\"\n";
  atmte_str += "args: --no-cache\n";
  atmte_str += "end-language: \"Autoconf-without-aclocal-m4\"\n";
  fs::ofstream atmte;
  atmte.open(".autom4te.cfg");
  atmte << atmte_str;
  atmte.close();
}
