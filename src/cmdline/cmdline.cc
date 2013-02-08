#include "src/cmdline/cmdline.h"
#include "src/clean/clean.h"
#include "src/aux/utils.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

typedef io::tee_device<std::ostream, fs::ofstream> Tee;
typedef io::stream<Tee> TeeStream;

cmdline::cmdline() : 
  ckon_cmd(""), ckon_config_file("ckon.cfg"),
  ckon_src_dir("StRoot"), ckon_core_dir("MyCore"), ckon_obsolete_dir("Obsolete"),
  ckon_exclSuffix("Gnuplot Options"), ckon_DontScan("dat-files database"),
  ckon_NoRootCint("YamlCpp"), ckon_prog_subdir("programs"), ckon_macro_subdir("macros"),
  ckon_build_dir("build"), ckon_install_dir("build") { }

void cmdline::purge() {
  clean clcont(ckon_src_dir);
  printf("%d files removed.\n",clcont.purge());
}

void cmdline::runSetup() {

  if ( fs::exists(ckon_config_file) || fs::exists("configure.ac") ) {
    cout << ckon_config_file << " or configure.ac already exist(s)!" << endl;
    if ( utils::askYesOrNo("remove and start over") == 'n' ) return;
    fs::remove(ckon_config_file);
    fs::remove("configure.ac");
    fs::remove(".autom4te.cfg");
  }

  cout << "write cfgfile" << endl;

  fs::ofstream cfgfile(ckon_config_file);
  Tee tee( std::cout, cfgfile );
  TeeStream both( tee );
  both << "pythia=" << bPythia << endl;
  both << "roofit=" << bRooFit << endl;
  both << "boost=" << bBoost << endl;
  both << "suffix=" << bSuffix << endl;
  both << "[ckon]" << endl;
  both << "src_dir=" << ckon_src_dir << endl;
  both << "core_dir=" << ckon_core_dir << endl;
  both << "obsolete_dir=" << ckon_obsolete_dir << endl;
  both << "prog_subdir=" << ckon_prog_subdir << endl;
  both << "macro_subdir=" << ckon_macro_subdir << endl;
  both << "build_dir=" << ckon_build_dir << endl;
  both << "install_dir=" << ckon_install_dir << endl;
  both << "exclSuffix=\"" << ckon_exclSuffix << "\"" << endl;
  both << "DontScan=\"" << ckon_DontScan << "\"" << endl;
  both << "NoRootCint=" << ckon_NoRootCint << endl;
  both.close();

  writeConfigureAc();
  writeAutom4teCfg();

  cout << endl << "setup done. check " << ckon_config_file << endl;
}

bool cmdline::parse(int argc, char *argv[]) {

  po::options_description generic("Generic Options"); 
  generic.add_options() 
    ("help,h" , po::bool_switch(&bHelp)    , "show this help") 
    ("verbose,v" , po::bool_switch(&bVerbose) , "verbose output")
    (",j" , po::value<string>(&nCpu) , "call make with option -j <#cores> (parallel compile)")
    ("ckon_cmd" , po::value<string>(&ckon_cmd)->default_value("")  , "<none> | setup | clean | install");

  po::options_description config("Configuration"); 
  config.add_options() 
    ("pythia,p"  , po::value<bool>(&bPythia)->default_value(0)  , "link with pythia library")
    ("roofit,r"  , po::value<bool>(&bRooFit)->default_value(0)  , "link with roofit library")
    ("suffix,s"  , po::value<bool>(&bSuffix)->default_value(0)  , "Add suffix + in LinkDef file")
    ("boost,b"   , po::value<bool>(&bBoost)->default_value(0)   , "include BOOST_INC and BOOST_LIB")
    ("ckon.config_file"  , po::value<string>(&ckon_config_file)->default_value(ckon_config_file), "config file name")
    ("ckon.src_dir"      , po::value<string>(&ckon_src_dir)->default_value(ckon_src_dir), "source dir")
    ("ckon.core_dir"     , po::value<string>(&ckon_core_dir)->default_value(ckon_core_dir), "core dir")
    ("ckon.obsolete_dir" , po::value<string>(&ckon_obsolete_dir)->default_value(ckon_obsolete_dir), "obsolete dir")
    ("ckon.exclSuffix"   , po::value<string>(&ckon_exclSuffix)->default_value(ckon_exclSuffix), "exclude dirs from suffix")
    ("ckon.DontScan"     , po::value<string>(&ckon_DontScan)->default_value(ckon_DontScan), "omit dirs from source scan")
    ("ckon.NoRootCint"   , po::value<string>(&ckon_NoRootCint)->default_value(ckon_NoRootCint), "don't generate dictionary for libraries")
    ("ckon.prog_subdir"  , po::value<string>(&ckon_prog_subdir)->default_value(ckon_prog_subdir), "programs source dir")
    ("ckon.macro_subdir" , po::value<string>(&ckon_macro_subdir)->default_value(ckon_macro_subdir), "macros subdir")
    ("ckon.build_dir"    , po::value<string>(&ckon_build_dir)->default_value(ckon_build_dir), "build dir")
    ("ckon.install_dir"  , po::value<string>(&ckon_install_dir)->default_value(ckon_install_dir), "install dir");

  po::options_description allopts; 
  allopts.add(generic).add(config);

  po::positional_options_description posOpts; 
  posOpts.add("ckon_cmd",1);

  po::variables_map vm; 

  try { 

    po::store(po::command_line_parser(argc,argv).options(allopts).positional(posOpts).run(),vm);
    if ( fs::exists(ckon_config_file) ) {
      po::store(po::parse_config_file<char>(ckon_config_file.c_str(), config), vm);
    }

    po::notify(vm); // throws on error, so do after help in case there are any problems 

    if ( bHelp ) { // --help option  
      cout << "ckon -- automatic ROOT analyses compiler & linker" << endl << allopts << endl; 
      return false; 
    } 

    bSetup = !ckon_cmd.compare("setup");
    bClean = !ckon_cmd.compare("clean");
    bInstall = !ckon_cmd.compare("install");

    if ( bSetup ) { cout << "run setup" << endl; runSetup(); return false; }
    if ( bClean ) { purge(); return false; }

    return true;

  } 
  catch(po::error& e) { 
    cerr << "ERROR: " << e.what() << endl << endl << generic << endl; 
    return false; 
  } 

}

void cmdline::writeConfigureAc() {
  fs::ofstream cfg_ac;
  cfg_ac.open("configure.ac");
  string ckon_ana_name("ana"), ckon_ana_version("0.0");
  //cout << "set ckon_ana_name : "; getline(cin,ckon_ana_name);
  //cout << "set ckon_ana_version : "; getline(cin,ckon_ana_version);
  cfg_ac << "AC_INIT([" << ckon_ana_name << "], [" << ckon_ana_version << "])" << endl;
  cfg_ac << "m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])" << endl;
  cfg_ac << "AC_CONFIG_AUX_DIR(config)" << endl;
  cfg_ac << "m4_pattern_allow([AM_PROG_AR])" << endl;
  cfg_ac << "m4_ifdef([AM_PROG_AR], [AM_PROG_AR])" << endl;
  cfg_ac << "AM_INIT_AUTOMAKE([-Wall no-define])" << endl;
  cfg_ac << "AC_PROG_CXX" << endl;
  cfg_ac << "AM_PROG_LIBTOOL" << endl;
  cfg_ac << "ROOTLIBS=`$ROOTSYS/bin/root-config --libs`" << endl;
  cfg_ac << "ROOTINCLUDES=`$ROOTSYS/bin/root-config --incdir`" << endl;
  cfg_ac << "ROOTLIBDIR=`$ROOTSYS/bin/root-config --libdir`" << endl;
  cfg_ac << "ROOTGLIBS=`$ROOTSYS/bin/root-config --glibs`" << endl;
  cfg_ac << "AC_SUBST(ROOTLIBS)" << endl;
  cfg_ac << "AC_SUBST(ROOTINCLUDES)" << endl;
  cfg_ac << "AC_SUBST(ROOTGLIBS)" << endl;
  cfg_ac << "AC_SUBST(ROOTLIBDIR)" << endl;
  cfg_ac << "AC_CONFIG_FILES([Makefile])" << endl;
  cfg_ac << "AC_OUTPUT" << endl;
  cfg_ac.close();
}

void cmdline::writeAutom4teCfg() {
  fs::ofstream atmte;
  atmte.open(".autom4te.cfg");
  atmte << "begin-language: \"Autoconf-without-aclocal-m4\"" << endl;
  atmte << "args: --no-cache" << endl;
  atmte << "end-language: \"Autoconf-without-aclocal-m4\"" << endl;
  atmte.close();
}

