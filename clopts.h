#include "StRoot/Options/Options.h"

const char * optv[] = {
  "v|verbose",
  "j:ncpu <ncpu>",
  "p|pythia",
  "r|roofit",
  "b|boost",
  "s|suffix",
  "d|doxygen",
  NULL
} ;

struct ClOpts {
  bool bHelp;
  bool bVerbose;
  bool bClean;
  bool bInstall;
  bool bPythia;
  bool bRooFit;
  bool bSuffix;
  bool bDoxygen;
  bool bSetup;
  bool bBoost;
  int nCpu;
  map <string, int> OptionsMap;
  map <string, string> DirsMap;
  void init() {
    bHelp = false; bVerbose = false;
    bClean = false; bInstall = false; bPythia = true;
    bBoost = true; bRooFit = true; bSuffix = true;
    bDoxygen = false; nCpu = 1; bSetup = false;
  }
  bool isCmd(string argstr) { 
    return ( argstr.find_first_of("-") == string::npos // didn't find '-' (option)
	&& argstr.compare("ckon") != 0 // 'ckon' is program name not command
	&& argstr.compare("./ckon") != 0 // './ckon' is program name not command
	&& argstr.find_first_of("0123456789") == string::npos // didn't find any number
	);
  }
  char askYesOrNo(string question) {
    char type;
    do {
      cout << question << "? [y/n] ";
      cin >> type;
      cin.ignore();
    }
    while( !cin.fail() && type!='y' && type!='n' );
    return type;
  }
  void askOption(string key) {
    OptionsMap[key] = (askYesOrNo(key)=='y') ? 1 : 0;
  }
  bool checkDirName(string key) {
    if (
	key.compare("ckon_exclSuffix") != 0 ||
	key.compare("ckon_NoRootCint") != 0 ||
	key.compare("ckon_DontScan") != 0
       ) {
      return boost::all(DirsMap[key],boost::is_alnum());
    }
    return boost::all(DirsMap[key],boost::is_alnum()||boost::is_space());
  }
  void askDir(string key) {
    do {
      cout << "set " << key << " : " << flush;
      getline(cin,DirsMap[key]);
    }
    while( !cin.fail() && !checkDirName(key) );
  }
  bool runSetup() {
    // check whether config file already exists
    if ( fs::exists(ckon_config_file) || fs::exists("configure.ac") ) {
      cout << ckon_config_file << " or configure.ac already exist(s)!" << endl;
      if ( askYesOrNo("remove and start over") == 'n' ) return false;
      fs::remove(ckon_config_file);
      fs::remove("configure.ac");
      fs::remove(".autom4te.cfg");
    }
    // init config file and yaml emitter
    fs::ofstream out;
    out.open(ckon_config_file);
    YAML::Emitter yaml_out;
    // set & print default DirsMap (directory structure)
    cout << "----------------------------" << endl;
    DirsMap["ckon_src_dir"] = ckon_src_dir;
    DirsMap["ckon_core_dir"] = ckon_core_dir;
    DirsMap["ckon_obsolete_dir"] = ckon_obsolete_dir;
    DirsMap["ckon_prog_subdir"] = ckon_prog_subdir;
    DirsMap["ckon_macro_subdir"] = ckon_macro_subdir;
    DirsMap["ckon_build_dir"] = ckon_build_dir;
    DirsMap["ckon_install_dir"] = ckon_install_dir;
    DirsMap["ckon_exclSuffix"] = ckon_exclSuffix;
    DirsMap["ckon_DontScan"] = ckon_DontScan;
    DirsMap["ckon_NoRootCint"] = ckon_NoRootCint;
    cout << "default directory structure:" << endl;
    cout << "----------------------------" << endl;
    BOOST_FOREACH( string key, DirsMap | ad::map_keys )
      cout << key << " : " << DirsMap[key] << endl;
    // set & print default OptionsMap (build options)
    cout << "----------------------" << endl;
    OptionsMap["pythia"] = bPythia;
    OptionsMap["roofit"] = bRooFit;
    OptionsMap["boost"] = bBoost;
    OptionsMap["suffix"] = bSuffix;
    OptionsMap["doxygen"] = bDoxygen;
    cout << "default build options:" << endl;
    cout << "----------------------" << endl;
    BOOST_FOREACH( string key, OptionsMap | ad::map_keys )
      cout << key << " : " << OptionsMap[key] << endl;
    // ask setup questions & stream to yaml emitter
    cout << "============================" << endl;
    if ( askYesOrNo("accept default directory structure") == 'n' ) {
      BOOST_FOREACH( string key, DirsMap | ad::map_keys ) askDir(key);
    }
    if ( askYesOrNo("accept default build options") == 'n' ) {
      BOOST_FOREACH( string key, OptionsMap | ad::map_keys ) askOption(key);
    }
    yaml_out << DirsMap << OptionsMap;
    // write to & close config file
    out << yaml_out.c_str() << endl;
    // write configure.ac
    out.close();
    fs::ofstream cfg_ac;
    cfg_ac.open("configure.ac");
    string ckon_ana_name, ckon_ana_version;
    cout << "set ckon_ana_name : ";
    getline(cin,ckon_ana_name);
    cout << "set ckon_ana_version : ";
    getline(cin,ckon_ana_version);
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
    if ( bDoxygen ) {
      cfg_ac << "AC_CHECK_PROGS([DOXYGEN], [doxygen])" << endl;
      cfg_ac << "if test -z \"$DOXYGEN\";" << endl;
      cfg_ac << "   then AC_MSG_WARN([Doxygen not found - continue w/o Doxygen])" << endl;
      cfg_ac << "fi" << endl;
      cfg_ac << "AM_CONDITIONAL([HAVE_DOXYGEN],[test -n \"$DOXYGEN\"])";
      cfg_ac << "AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])" << endl;
    }
    cfg_ac << "AC_CONFIG_FILES([Makefile])" << endl;
    cfg_ac << "AC_OUTPUT" << endl;
    cfg_ac.close();
    // write .autom4te.cfg
    fs::ofstream atmte;
    atmte.open(".autom4te.cfg");
    atmte << "begin-language: \"Autoconf-without-aclocal-m4\"" << endl;
    atmte << "args: --no-cache" << endl;
    atmte << "end-language: \"Autoconf-without-aclocal-m4\"" << endl;
    atmte.close();
    return true;
  }
  bool parse(int argc, char *argv[]) {
    // check number of commands (commands are mutually exclusive):
    int nCmds = 0;
    for( int n = 0 ; n < argc ; ++n ) {
      string argstr = argv[n];
      if( isCmd(argstr) ) nCmds++;
    }
    if ( nCmds > 1 ) return false;
    // parse commands
    if ( nCmds > 0 ) { // clean | install | help | setup
      if ( !strcmp("clean",argv[argc-1]) ) bClean = true;
      if ( !strcmp("install",argv[argc-1]) ) bInstall = true;
      if ( !strcmp("help",argv[argc-1]) ) bHelp = true;
      if ( !strcmp("setup",argv[argc-1]) ) bSetup = true;
    }
    // run setup if requested
    if ( bSetup && !runSetup() ) {
      cout << "setup failed!." << endl;
      return false;
    }
    // if yaml config file exists -> parse it
    if ( fs::exists(ckon_config_file) ) {
      vector<YAML::Node> config = YAML::LoadAllFromFile(ckon_config_file);
      // directory settings ckon_*
      YAML::Node nDirs(config.at(0));
      ckon_src_dir = nDirs["ckon_src_dir"].as<string>();
      ckon_core_dir = nDirs["ckon_core_dir"].as<string>();
      ckon_obsolete_dir = nDirs["ckon_obsolete_dir"].as<string>();
      ckon_prog_subdir = nDirs["ckon_prog_subdir"].as<string>();
      ckon_macro_subdir = nDirs["ckon_macro_subdir"].as<string>();
      ckon_build_dir = nDirs["ckon_build_dir"].as<string>();
      ckon_install_dir = nDirs["ckon_install_dir"].as<string>();
      ckon_exclSuffix = nDirs["ckon_exclSuffix"].as<string>();
      ckon_DontScan = nDirs["ckon_DontScan"].as<string>();
      ckon_NoRootCint = nDirs["ckon_NoRootCint"].as<string>();
      // more permanent options
      YAML::Node nOpts(config.at(1));
      bPythia = nOpts["pythia"].as<int>();
      bRooFit = nOpts["roofit"].as<int>();
      bBoost = nOpts["boost"].as<int>();
      bSuffix = nOpts["suffix"].as<int>();
      bDoxygen = nOpts["doxygen"].as<int>();
    }
    // parse all options (take precedence over yaml config!)
    Options  opts(*argv, optv);
    OptArgvIter  iter(--argc, ++argv);
    const char *optarg;
    while( char optchar = opts(iter, optarg) ) {
      switch (optchar) {
      case 'v' : // only as option
	bVerbose = true;
	printf("-v: run in verbose mode.\n");
	break;
      case 'j' : // only as option
	nCpu = atoi(optarg);
	printf("-j %d: compile w/ %d cores\n",nCpu,nCpu);
	break;
      case 'p' :
	bPythia = true;
	printf("-p: link with pythia library\n");
	break;
      case 'r' :
	bRooFit = true;
	printf("-r: link with RooFit library\n");
	break;
      case 'b' :
	bBoost = true;
	printf("-b: include BOOST_INC and BOOST_LIB\n");
	break;
      case 's' :
	bSuffix = true;
	printf("-s: compile with + suffix in LinkDef.h\n");
	break;
      case 'd' :
	bDoxygen = true;
	printf("-d: run doxygen\n");
	break;
      default : break;
      }
    }
    return true;
  }
} clopts;


