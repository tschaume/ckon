#include "cmdline.h"
#include "clean.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

typedef io::tee_device<std::ostream, fs::ofstream> Tee;
typedef io::stream<Tee> TeeStream;

cmdline::cmdline() : 
    bHelp(0), bVerbose(0), bInstall(0), bSetup(0), bClean(0), bPythia(1), bBoost(1), bRooFit(1),
    bSuffix(1), nCpu(1), desc("Options"), ckon_config_file("ckon.cfg"),
    ckon_src_dir("StRoot"), ckon_core_dir("MyCore"), ckon_obsolete_dir("Obsolete"),
    ckon_exclSuffix("Gnuplot Options"), ckon_DontScan("dat-files database"),
    ckon_NoRootCint("YamlCpp"), ckon_prog_subdir("programs"), ckon_macro_subdir("macros"),
    ckon_build_dir("build"), ckon_install_dir("build") 
{
  ut = new utils();
}

void cmdline::purge() {
  clean clcont(ckon_src_dir);
  printf("%d files removed.\n",clcont.purge());
}

void cmdline::runSetup() {

  if ( fs::exists(ckon_config_file) || fs::exists("configure.ac") ) {
    cout << ckon_config_file << " or configure.ac already exist(s)!" << endl;
    if ( ut->askYesOrNo("remove and start over") == 'n' ) return;
    fs::remove(ckon_config_file);
    fs::remove("configure.ac");
    fs::remove(".autom4te.cfg");
  }

  cout << "write cfgfile" << endl;

  fs::ofstream cfgfile(ckon_config_file);
  Tee tee( std::cout, cfgfile );
  TeeStream both( tee );
  both << "----------------------------" << endl;
  both << "default directory structure:" << endl;
  both << "----------------------------" << endl;
  both << "ckon_src_dir : " << ckon_src_dir << endl;
  both << "ckon_core_dir : " << ckon_core_dir << endl;
  both << "ckon_obsolete_dir : " << ckon_obsolete_dir << endl;
  both << "ckon_prog_subdir : " << ckon_prog_subdir << endl;
  both << "ckon_macro_subdir : " << ckon_macro_subdir << endl;
  both << "ckon_build_dir : " << ckon_build_dir << endl;
  both << "ckon_install_dir : " << ckon_install_dir << endl;
  both << "ckon_exclSuffix : " << ckon_exclSuffix << endl;
  both << "ckon_DontScan : " << ckon_DontScan << endl;
  both << "ckon_NoRootCint : " << ckon_NoRootCint << endl;
  both << "----------------------" << endl;
  both << "default build options:" << endl;
  both << "----------------------" << endl;
  both << "pythia : " << bPythia << endl;
  both << "roofit : " << bRooFit << endl;
  both << "boost : " << bBoost << endl;
  both << "suffix : " << bSuffix << endl;
  both.close();

  ut->writeConfigureAc();
  ut->writeAutom4teCfg();

  cout << endl << "setup done. check " << ckon_config_file << endl;
}

void cmdline::addopts() {

  desc.add_options() 
    ("help,h"    , po::bool_switch(&bHelp)    , "show this help") 
    ("verbose,v" , po::bool_switch(&bVerbose) , "verbose output")
    ("pythia,p"  , po::bool_switch(&bPythia)  , "link with pythia library")
    ("roofit,r"  , po::bool_switch(&bRooFit)  , "link with roofit library")
    ("suffix,s"  , po::bool_switch(&bSuffix)  , "Add suffix + in LinkDef file")
    ("boost,b"   , po::bool_switch(&bBoost)   , "include BOOST_INC and BOOST_LIB")
    (",j" , po::value<int>(&nCpu) , "call make with option -j <#cores> (parallel compile)")
    ("ckon_config_file"  , po::value<string>(&ckon_config_file)  , "config file name")
    ("ckon_src_dir"      , po::value<string>(&ckon_src_dir)      , "source dir")
    ("ckon_core_dir"     , po::value<string>(&ckon_core_dir)     , "core dir")
    ("ckon_obsolete_dir" , po::value<string>(&ckon_obsolete_dir) , "obsolete dir")
    ("ckon_exclSuffix"   , po::value<string>(&ckon_exclSuffix)   , "exclude dirs from suffix")
    ("ckon_DontScan"     , po::value<string>(&ckon_DontScan)     , "omit dirs from source scan")
    ("ckon_NoRootCint"   , po::value<string>(&ckon_NoRootCint)   , "don't generate dictionary for libraries")
    ("ckon_prog_subdir"  , po::value<string>(&ckon_prog_subdir)  , "programs source dir")
    ("ckon_macro_subdir" , po::value<string>(&ckon_macro_subdir) , "macros subdir")
    ("ckon_build_dir"    , po::value<string>(&ckon_build_dir)    , "build dir")
    ("ckon_install_dir"  , po::value<string>(&ckon_install_dir)  , "install dir")
    ("install" , po::bool_switch(&bInstall) , "install program/libraries into target dir")
    ("setup"   , po::bool_switch(&bSetup) , "run setup to generate ckon.cfg")
    ("clean"   , po::bool_switch(&bClean) , "clean up everything");

  posOpts.add("ckon_config_file",1);
  posOpts.add("ckon_src_dir",1);
  posOpts.add("ckon_core_dir",1);
  posOpts.add("ckon_obsolete_dir",1);
  posOpts.add("ckon_exclSuffix",1);
  posOpts.add("ckon_DontScan",1);
  posOpts.add("ckon_NoRootCint",1);
  posOpts.add("ckon_prog_subdir",1);
  posOpts.add("ckon_macro_subdir",1);
  posOpts.add("ckon_build_dir",1);
  posOpts.add("ckon_install_dir",1);

  posOpts.add("install",1); 
  posOpts.add("setup",1); 
  posOpts.add("clean",1); 

}

bool cmdline::parse(int argc, char *argv[]) {

  addopts();

  try { 

    po::store(po::command_line_parser(argc,argv).options(desc).positional(posOpts).run(),vm);
    if ( fs::exists(ckon_config_file) ) {
      po::store(po::parse_config_file<char>(ckon_config_file.c_str(), desc), vm);
    }

    if ( bSetup ) { cout << "run setup" << endl; runSetup(); return false; }
    if ( bClean ) { purge(); return false; }
    if ( vm.count("help") ) { // --help option  
      cout << "ckon -- automatic ROOT analyses compiler & linker" << endl << desc << endl; 
      return false; 
    } 

    po::notify(vm); // throws on error, so do after help in case there are any problems 
    return true;

  } 
  catch(po::error& e) { 
    cerr << "ERROR: " << e.what() << endl << endl << desc << endl; 
    return false; 
  } 

}
