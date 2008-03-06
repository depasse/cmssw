#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <fstream>

#include <sys/time.h>

//#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include "CaloOnlineTools/HcalOnlineDb/interface/XMLProcessor.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/XMLLUTLoader.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/XMLHTRPatterns.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/XMLHTRPatternLoader.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/XMLHTRZeroSuppressionLoader.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/LMap.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/LMapLoader.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/XMLRBXPedestalsLoader.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/HCALConfigDB.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/DBlmapReader.h"

#include "CaloOnlineTools/HcalOnlineDb/interface/ConfigurationDatabase.hh"
#include "CaloOnlineTools/HcalOnlineDb/interface/ConfigurationDatabaseImplOracle.hh"
#include "CaloOnlineTools/HcalOnlineDb/interface/ConfigurationItemNotFoundException.hh"
#include "CaloOnlineTools/HcalOnlineDb/interface/HcalHardwareXml.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/HcalQIEManager.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/RooGKCounter.h"

#include "xgi/Utils.h"
#include "toolbox/string.h"
#include "occi.h"
#include "CaloOnlineTools/HcalOnlineDb/interface/ConfigurationItemNotFoundException.hh"

using namespace std;
using namespace oracle::occi;
using namespace hcal;

int createLUTLoader( string prefix_="", string tag_="" );
int createHTRPatternLoader( void );
int createLMap( void );
int createRBXLoader( string type_, string tag_, string list_file );
//int createRBXentries( string );
int createZSLoader( void );
int testocci( void );
int testDB( string _tag, string _filename );
int lmaptest( string _param );
int hardware( void );
int qie_adc( void );
int test_db_access( void );
std::vector <std::string> splitString (const std::string& fLine);
int createZSLoader2( string & tag, string & comment, string & zs2HB, string & zs2HE, string & zs2HO, string & zs2HF );

int main( int argc, char **argv )
{
  cout << "Running xmlTools..." << endl;
  
  // parse command line options
  int c;
  int digit_optind = 0;
  
  // default parameter values
  bool luts = false;
  bool rbx = false;
  bool tag_b = false;
  bool comment_b = false;
  bool testdb_b = false;
  bool lmaptest_b = false;
  bool hardware_b = false;
  bool qie_b = false;
  bool test_db_access_b = false;
  bool zs2_b = false;
  bool zs2HB_b = false;
  bool zs2HE_b = false;
  bool zs2HO_b = false;
  bool zs2HF_b = false;

  string filename = "";
  string path = "";
  string tag = "";
  string comment = "";
  string prefix = "";
  string rbx_type = "";
  string aramsParameter = "";
  string zs2HB = "";
  string zs2HE = "";
  string zs2HO = "";
  string zs2HF = "";

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"filename", 1, 0, 1},
      {"path", 1, 0, 2},
      {"tag", 1, 0, 3},
      {"prefix", 1, 0, 4},
      {"comment", 1, 0, 5},
      {"luts", 0, 0, 10},
      {"patterns", 0, 0, 20},
      {"lmap", 0, 0, 30},
      {"rbxpedestals", 0, 0, 40},
      {"zs", 0, 0, 50},
      {"rbx", 1, 0, 60},
      {"luts2", 0, 0, 70},
      {"testocci", 0, 0, 1000},
      {"testdb", 1, 0, 1010},
      {"lmaptest", 1, 0, 2000},
      {"hardware", 0, 0, 1050},
      {"qie", 0, 0, 1060},
      {"test-db-access", 0, 0, 1070},
      {"zs2", 0, 0, 1080},
      {"zs2HB", 1, 0, 1090},
      {"zs2HE", 1, 0, 1091},
      {"zs2HO", 1, 0, 1092},
      {"zs2HF", 1, 0, 1093},
      {0, 0, 0, 0}
    };
        

    c = getopt_long (argc, argv, "",
		     long_options, &option_index);

    //cout << c << endl;

    if (c == -1)
      {
	break;
      }
    
    switch (c) {

    case 1:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "filename: " << _buf << endl;
	  filename .append( _buf );
	}
      else
	{
	  cout << "Missing file name!" << endl;
	}
      break;
      
    case 2:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "path: " << _buf << endl;
	  path .append( _buf );
	}
      else
	{
	  cout << "Empty path!" << endl;
	}
      break;
      
    case 3:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "path: " << _buf << endl;
	  tag .append( _buf );
	  tag_b = true;
	}
      else
	{
	  cout << "Empty tag!" << endl;
	}
      break;
      
    case 4:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "path: " << _buf << endl;
	  prefix .append( _buf );
	}
      else
	{
	  cout << "Empty prefix!" << endl;
	}
      break;
      
    case 5:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "path: " << _buf << endl;
	  comment . append( _buf );
	  comment_b = true;
	}
      else
	{
	  cout << "Empty comment!" << endl;
	}
      break;
      
    case 10:
      createLUTLoader();
      break;
      
    case 70:
      luts = true;
      break;
      
    case 20:
      createHTRPatternLoader();
      break;
      
    case 30:
      createLMap();
      break;
      
    case 40:
      //createRBXLoader();
      break;
      
    case 50:
      createZSLoader();
      break;
      
      // rbx
    case 60:
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  rbx_type .append( _buf );
	  rbx = true;
	}
      else
	{
	  cout << "RBX data type not defined!.." << endl;
	}
      break;

    case 1000: // testocci
      testocci();
      break;
      
    case 1010: // testdb
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  //cout << "filename: " << _buf << endl;
	  filename .append( _buf );
	  testdb_b = true;
	}
      else
	{
	  cout << "No XML file name specified! " << endl;
	}
      break;

    case 2000: // lmaptest
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  aramsParameter .append( _buf );
	  lmaptest_b = true;
	}
      else
	{
	  cout << "No parameter specified! " << endl;
	}
      break;

    case 1050: // HCAL hardware map
      hardware_b=true;
      break;
      
    case 1060: // qie
      qie_b=true;
      break;
      
    case 1070: // oracle access example to lmap and stuff for Dmitry
      test_db_access_b=true;
      break;
      
    case 1080: // ZS generator ver.2
      zs2_b=true;
      break;
      
    case 1090: // ZS for HB
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  zs2HB .append( _buf );
	  zs2HB_b = true;
	}
      else
	{
	  cout << "No zero suppression value for HB specified... " << endl;
	}
      break;

    case 1091: // ZS for HE
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  zs2HE .append( _buf );
	  zs2HE_b = true;
	}
      else
	{
	  cout << "No zero suppression value for HE specified... " << endl;
	}
      break;

    case 1092: // ZS for HO
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  zs2HO .append( _buf );
	  zs2HO_b = true;
	}
      else
	{
	  cout << "No zero suppression value for HO specified... " << endl;
	}
      break;

    case 1093: // ZS for HF
      if ( optarg )
	{
	  char _buf[1024];
	  sprintf( _buf, "%s", optarg );
	  zs2HF .append( _buf );
	  zs2HF_b = true;
	}
      else
	{
	  cout << "No zero suppression value for HF specified... " << endl;
	}
      break;

    default:
      printf ("?? getopt returned character code 0%o ??\n", c);
    }
  }
  
  if (optind < argc) {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    printf ("\n");
  }

  // decide what to depending on the params
  if ( luts )
    {
      cout << "path: " << path << endl;
      cout << "prefix: " << prefix << endl;
      cout << "TAG_NAME: " << tag << endl;
      createLUTLoader( prefix, tag );
    }
  else if ( rbx )
    {
      cout << "type: " << rbx_type << endl;
      cout << "TAG_NAME: " << tag << endl;
      cout << "list file: " << filename << endl;
      
      createRBXLoader( rbx_type, tag, filename );      

      if ( tag . size() < 1 ) cout << "===> WARNING: tag field is empty!" << endl;
    }
  else if ( testdb_b && tag_b )
    {
      testDB( tag, filename );      
    }

  else if ( lmaptest_b )
    {
      lmaptest( aramsParameter );      
    }
  else if ( hardware_b )
    {
      hardware();      
    }
  else if ( qie_b )
    {
      qie_adc();      
    }
  else if ( test_db_access_b )
    {
      test_db_access();      
    }
  else if ( zs2_b )
    {
      while(1){
	if ( !tag_b ){
	  cout << "No tag specified... exiting" << endl;
	  break;
	}
	if ( !zs2HB_b ){
	  cout << "No zero suppression value dor HB specified... exiting" << endl;
	  break;
	}
	if ( !zs2HE_b ){
	  cout << "No zero suppression value dor HE specified... exiting" << endl;
	  break;
	}
	if ( !zs2HO_b ){
	  cout << "No zero suppression value dor HO specified... exiting" << endl;
	  break;
	}
	if ( !zs2HF_b ){
	  cout << "No zero suppression value dor HF specified... exiting" << endl;
	  break;
	}
	createZSLoader2( tag, comment, zs2HB, zs2HE, zs2HO, zs2HF );
	break;
      }
    }
  else
    {
      //cout << "Nothing to do!" << endl;
    }
  
  cout << "xmlTools ...done" << endl;

  exit (0);
  
}








// Zero suppression Loader
int createZSLoader( void )
{
  XMLHTRZeroSuppressionLoader::loaderBaseConfig baseConf;
  XMLHTRZeroSuppressionLoader::datasetDBConfig conf;

  string _prefix = "GREN_ZS_9adc_v2";
  string _comment = "ZS for GREN07, (tune=3.5)*2+2";
  string _version = "GREN07:1";
  string _subversion = "1";

  baseConf . tag_name = _prefix;
  baseConf . comment_description = _comment;
  baseConf . elements_comment_description = _comment;
  baseConf . run_number = 1;
  baseConf . iov_begin = 1 ;
  baseConf . iov_end = -1 ;

  XMLHTRZeroSuppressionLoader doc( &baseConf );
  //doc . createLoader( );

  LMap map_hbef;
  map_hbef . read( "HCALmapHBEF_11.9.2007.txt", "HBEF" ); // HBEF logical map

  for( vector<LMapRow>::const_iterator row = map_hbef . _table . begin(); row != map_hbef . _table . end(); row++ )
    {
      conf . comment_description = _comment;
      conf . version = _version;
      conf . subversion = _subversion;
      conf . eta = (row -> eta);
      conf . z = (row -> side);
      conf . phi = row -> phi;
      conf . depth = row -> depth;
      conf . detector_name = row -> det;

      HcalSubdetector _subdet;
      if ( row->det . c_str() == "HB" ) _subdet = HcalBarrel;
      if ( row->det . c_str() == "HE" ) _subdet = HcalEndcap;
      if ( row->det . c_str() == "HO" ) _subdet = HcalOuter;
      if ( row->det . c_str() == "HF" ) _subdet = HcalForward;
      HcalDetId _hcaldetid( _subdet, (row->side)*(row->eta), row->phi, row->depth );
      conf . hcal_channel_id = _hcaldetid . rawId();

      conf . zero_suppression = 9;

      doc . addZS( &conf );
    }
  
  LMap map_ho;
  map_ho . read( "HCALmapHO_11.9.2007.txt", "HO" ); // HO logical map

  for( vector<LMapRow>::const_iterator row = map_ho . _table . begin(); row != map_ho . _table . end(); row++ )
    {
      conf . comment_description = _comment;
      conf . version = _version;
      conf . subversion = _subversion;
      conf . eta = (row -> eta);
      conf . z = (row -> side);
      conf . phi = row -> phi;
      conf . depth = row -> depth;
      conf . detector_name = row -> det;

      HcalSubdetector _subdet;
      if ( row->det . c_str() == "HB" ) _subdet = HcalBarrel;
      if ( row->det . c_str() == "HE" ) _subdet = HcalEndcap;
      if ( row->det . c_str() == "HO" ) _subdet = HcalOuter;
      if ( row->det . c_str() == "HF" ) _subdet = HcalForward;
      HcalDetId _hcaldetid( _subdet, (row->side)*(row->eta), row->phi, row->depth );
      conf . hcal_channel_id = _hcaldetid . rawId();

      conf . zero_suppression = 9;

      doc . addZS( &conf );
    }
  
  doc . write( _prefix + "_ZeroSuppressionLoader.xml" );
  
  return 0;
}


// Zero suppression Loader version 2
int createZSLoader2( string & tag, string & comment, string & zs2HB, string & zs2HE, string & zs2HO, string & zs2HF )
{

  XMLHTRZeroSuppressionLoader::loaderBaseConfig baseConf;
  XMLHTRZeroSuppressionLoader::datasetDBConfig conf;

  string lmap_version = "30";

  string _prefix = tag;
  string _comment = comment;
  string _version = "GRuMM_test:1";
  string _subversion = "1";

  baseConf . tag_name = _prefix;
  baseConf . comment_description = _comment;
  baseConf . elements_comment_description = _comment;
  baseConf . run_number = 1;
  baseConf . iov_begin = 1 ;
  baseConf . iov_end = -1 ;

  XMLHTRZeroSuppressionLoader doc( &baseConf );

  // loop over LMAP
  HCALConfigDB * db = new HCALConfigDB();
  const std::string _accessor = "occi://CMS_HCL_PRTTYPE_HCAL_READER@anyhost/int2r?PASSWORD=HCAL_Reader_88,LHWM_VERSION=22";
  db -> connect( _accessor );

  oracle::occi::Connection * _connection = db -> getConnection();  

  int eta_abs, side, phi, depth;
  string subdet;

  try {
    Statement* stmt = _connection -> createStatement();
    std::string query = ("SELECT eta, side, phi, depth, subdetector, cds.version ");
    query += " FROM CMS_HCL_HCAL_CONDITION_OWNER.HCAL_HARDWARE_LOGICAL_MAPS_V3 lmap";
    query += " join cms_hcl_core_condition_owner.cond_data_sets cds ";
    query += " on cds.condition_data_set_id=lmap.condition_data_set_id ";
    query += toolbox::toString(" WHERE version='%s'", lmap_version . c_str() );
    
    //SELECT
    ResultSet *rs = stmt->executeQuery(query.c_str());

    while (rs->next()) {
      eta_abs  = rs -> getInt(1);
      side    = rs -> getInt(2);
      phi     = rs -> getInt(3);
      depth   = rs -> getInt(4);
      subdet  = rs -> getString(5);
      
      conf . comment_description = _comment;
      conf . version = _version;
      conf . subversion = _subversion;
      conf . eta = eta_abs;
      conf . z = side;
      conf . phi = phi;
      conf . depth = depth;
      conf . detector_name = subdet;

      int _zs;
      HcalSubdetector _subdet;
      if ( subdet == "HB" ){
	_subdet = HcalBarrel;
	sscanf(zs2HB.c_str(),"%d", &_zs);
      }
      if ( subdet == "HE" ){
	_subdet = HcalEndcap;
	sscanf(zs2HE.c_str(),"%d", &_zs);
      }
      if ( subdet == "HO" ){
	_subdet = HcalOuter;
	sscanf(zs2HO.c_str(),"%d", &_zs);
      }
      if ( subdet == "HF" ){
	_subdet = HcalForward;
	sscanf(zs2HF.c_str(),"%d", &_zs);
      }
      HcalDetId _hcaldetid( _subdet, side*eta_abs, phi, depth );
      conf . hcal_channel_id = _hcaldetid . rawId();

      //conf . zero_suppression = 9;
      conf . zero_suppression = _zs;

      doc . addZS( &conf );
    }
    //Always terminate statement
    _connection -> terminateStatement(stmt);
  } catch (SQLException& e) {
    XCEPT_RAISE(hcal::exception::ConfigurationDatabaseException,::toolbox::toString("Oracle  exception : %s",e.getMessage().c_str()));
  }

  db -> disconnect();
  
  doc . write( _prefix + "_ZeroSuppressionLoader.xml" );
  
  return 0;
}



// LUT Loader
int createLUTLoader( string _prefix, string tag_name )
{
  cout << "Generating XML loader for LUTs..." << endl;

  XMLLUTLoader::loaderBaseConfig baseConf;
  XMLLUTLoader::lutDBConfig conf;
  XMLLUTLoader::checksumsDBConfig CSconf;

  //_prefix = "GREN_olddetid_fake_p3_thr9_all";
  //tag_name = "GREN_olddetid_fake_p3_thr9_all";

  baseConf . tag_name = tag_name;
  //baseConf . comment_description = _prefix + ": LUTs for GREN 26Nov2007";
  baseConf . comment_description = tag_name;
  baseConf . iov_begin = "1";
  baseConf . iov_end = "-1";

  //conf . version = _prefix + ":1";
  conf . version = "GREN2007:1";
  conf . subversion = "1";

  CSconf . version = conf . version;
  CSconf . subversion = conf . subversion;
  CSconf . trig_prim_lookuptbl_data_file = _prefix + "_checksums.xml.dat";
  //CSconf . comment_description = _prefix + ": GREN 26Nov2007 checksums LUTs";
  CSconf . comment_description = tag_name;

  XMLLUTLoader doc( &baseConf );
  vector<int> crate_number;
  crate_number . push_back(0);
  crate_number . push_back(1);
  crate_number . push_back(2);
  crate_number . push_back(4);
  crate_number . push_back(5);
  crate_number . push_back(9);
  crate_number . push_back(10);
  crate_number . push_back(11);
  crate_number . push_back(12);
  crate_number . push_back(14);
  crate_number . push_back(15);
  crate_number . push_back(17);
  vector<string> file_name;
  file_name . push_back( "./" + _prefix + "_0.xml.dat" );
  file_name . push_back( "./" + _prefix + "_1.xml.dat" );
  file_name . push_back( "./" + _prefix + "_2.xml.dat" );
  file_name . push_back( "./" + _prefix + "_4.xml.dat" );
  file_name . push_back( "./" + _prefix + "_5.xml.dat" );
  file_name . push_back( "./" + _prefix + "_9.xml.dat" );
  file_name . push_back( "./" + _prefix + "_10.xml.dat" );
  file_name . push_back( "./" + _prefix + "_11.xml.dat" );
  file_name . push_back( "./" + _prefix + "_12.xml.dat" );
  file_name . push_back( "./" + _prefix + "_14.xml.dat" );
  file_name . push_back( "./" + _prefix + "_15.xml.dat" );
  file_name . push_back( "./" + _prefix + "_17.xml.dat" );

  for ( vector<string>::const_iterator _file = file_name . begin(); _file != file_name . end(); _file++ )
    {
      conf . trig_prim_lookuptbl_data_file = *_file;
      //conf . trig_prim_lookuptbl_data_file += ".dat";
      conf . crate = crate_number[ _file - file_name . begin() ];

      char _buf[128];
      sprintf( _buf, "CRATE%.2d", conf . crate );
      string _namelabel;
      _namelabel . append( _buf );
      conf . name_label = _namelabel;
      doc . addLUT( &conf );
    }
  
  doc . addChecksums( &CSconf );
  //doc . write( _prefix + "_Loader.xml" );
  doc . write( tag_name + "_Loader.xml" );

  cout << "Generating XML loader for LUTs... done." << endl;

  return 0;
}

int createHTRPatternLoader( void )
{
  // HTR Patterns Loader

  cout << "Generating XML loader for HTR patterns..." << endl;

  XMLHTRPatternLoader::loaderBaseConfig baseConf;
  baseConf . tag_name = "Test tag 1";
  baseConf . comment_description = "Test loading to the DB";
  
  XMLHTRPatternLoader doc( &baseConf );
  baseConf . run_mode = "no-run";
  baseConf . data_set_id = "-1";
  baseConf . iov_id = "1";
  baseConf . iov_begin = "1"; // beginning of the interval of validity 
  baseConf . iov_end = "-1"; // end of IOV, "-1" stands for +infinity
  baseConf . tag_name = "test tag";
  baseConf . comment_description = "comment";
  
  XMLHTRPatternLoader::datasetDBConfig conf;
  
  // add a file with the crate 2 patterns
  // repeat for each crate
  //--------------------->
  conf . htr_data_patterns_data_file = "file_crate02.xml.dat";
  conf . crate = 2;
  conf . name_label = "CRATE02";
  conf . version = "ver:1";
  conf . subversion = "1";
  conf . create_timestamp = 1100000000; // UNIX timestamp
  conf . created_by_user = "user";
  doc . addPattern( &conf );
  //---------------------->

  // write the XML to a file
  doc . write( "HTRPatternLoader.xml" );

  cout << "Generating XML loader for HTR patterns... done." << endl;

  // end of HTR Patterns Loader

  return 0;
}


int createLMap( void ){

  cout << "XML Test..." << endl;
  
  //XMLProcessor * theProcessor = XMLProcessor::getInstance();

  //XMLDOMBlock * doc = theProcessor -> createLMapHBEFXMLBase( "FullLmapBase.xml" );
  LMapLoader doc;

  LMapLoader::LMapRowHBEF aRow;
  LMapLoader::LMapRowHO bRow;


  ifstream fcha("/afs/fnal.gov/files/home/room3/avetisya/public_html/HCAL/Maps/HCALmapHBEF_Feb.21.2008.txt");
  ifstream fcho("/afs/fnal.gov/files/home/room3/avetisya/public_html/HCAL/Maps/HCALmapHO_Feb.21.2008.txt");

  //List in order HB HE HF
  //side     eta     phi     dphi       depth      det
  //rbx      wedge   rm      pixel      qie   
  //adc      rm_fi   fi_ch   crate      htr          
  //fpga     htr_fi  dcc_sl  spigo      dcc 
  //slb      slbin   slbin2  slnam      rctcra     rctcar
  //rctcon   rctnam  fedid

  const int NCHA = 6912;
  const int NCHO = 2160;
  int ncho = 0;
  int i, j;
  string ndump;

  int sideC[NCHA], etaC[NCHA], phiC[NCHA], dphiC[NCHA], depthC[NCHA], wedgeC[NCHA], crateC[NCHA], rmC[NCHA], rm_fiC[NCHA], htrC[NCHA];
  int htr_fiC[NCHA], fi_chC[NCHA], spigoC[NCHA], dccC[NCHA], dcc_slC[NCHA], fedidC[NCHA], pixelC[NCHA], qieC[NCHA], adcC[NCHA];
  int slbC[NCHA], rctcraC[NCHA], rctcarC[NCHA], rctconC[NCHA];
  string detC[NCHA], rbxC[NCHA], fpgaC[NCHA], slbinC[NCHA], slbin2C[NCHA], slnamC[NCHA], rctnamC[NCHA];

  int sideO[NCHO], etaO[NCHO], phiO[NCHO], dphiO[NCHO], depthO[NCHO], sectorO[NCHO], crateO[NCHO], rmO[NCHO], rm_fiO[NCHO], htrO[NCHO];
  int htr_fiO[NCHO], fi_chO[NCHO], spigoO[NCHO], dccO[NCHO], dcc_slO[NCHO], fedidO[NCHO], pixelO[NCHO], qieO[NCHO], adcO[NCHO];
  int geoO[NCHO], blockO[NCHO], lcO[NCHO];
  string detO[NCHO], rbxO[NCHO], fpgaO[NCHO], let_codeO[NCHO];

  int counter = 0;
  for (i = 0; i < NCHA; i++){
    if(i == counter){
      for (j = 0; j < 31; j++){
	fcha>>ndump;
	ndump = "";
      }
      counter += 21;
    }
    fcha>>sideC[i];
    fcha>>etaC[i]>>phiC[i]>>dphiC[i]>>depthC[i]>>detC[i];
    fcha>>rbxC[i]>>wedgeC[i]>>rmC[i]>>pixelC[i]>>qieC[i];
    fcha>>adcC[i]>>rm_fiC[i]>>fi_chC[i]>>crateC[i]>>htrC[i];
    fcha>>fpgaC[i]>>htr_fiC[i]>>dcc_slC[i]>>spigoC[i]>>dccC[i];
    fcha>>slbC[i]>>slbinC[i]>>slbin2C[i]>>slnamC[i]>>rctcraC[i]>>rctcarC[i];
    fcha>>rctconC[i]>>rctnamC[i]>>fedidC[i];
  }
    
  for(i = 0; i < NCHA; i++){
    aRow . side   = sideC[i];
    aRow . eta    = etaC[i];
    aRow . phi    = phiC[i];
    aRow . dphi   = dphiC[i];
    aRow . depth  = depthC[i];
    aRow . det    = detC[i];
    aRow . rbx    = rbxC[i];
    aRow . wedge  = wedgeC[i];
    aRow . rm     = rmC[i];
    aRow . pixel  = pixelC[i];
    aRow . qie    = qieC[i];
    aRow . adc    = adcC[i];
    aRow . rm_fi  = rm_fiC[i];
    aRow . fi_ch  = fi_chC[i];
    aRow . crate  = crateC[i];
    aRow . htr    = htrC[i];
    aRow . fpga   = fpgaC[i];
    aRow . htr_fi = htr_fiC[i];
    aRow . dcc_sl = dcc_slC[i];
    aRow . spigo  = spigoC[i];
    aRow . dcc    = dccC[i];
    aRow . slb    = slbC[i];
    aRow . slbin  = slbinC[i];
    aRow . slbin2 = slbin2C[i];
    aRow . slnam  = slnamC[i];
    aRow . rctcra = rctcraC[i];
    aRow . rctcar = rctcarC[i];
    aRow . rctcon = rctconC[i];
    aRow . rctnam = rctnamC[i];
    aRow . fedid  = fedidC[i];
    
    doc . addLMapHBEFDataset( &aRow, "FullHCALDataset.xml" );
  }

  counter = 0;
  for (i = 0; i < NCHO; i++){
    if(i == counter){
      for (j = 0; j < 27; j++){
	fcho>>ndump;
	ndump = "";
      }
      counter += 21;
    }
    fcho>>sideO[i];
    if (sideO[i] != 1 && sideO[i] != -1){
      cerr<<ncho<<'\t'<<sideO[i]<<endl;
      break;
    }
    fcho>>etaO[i]>>phiO[i]>>dphiO[i]>>depthO[i]>>detO[i];
    fcho>>rbxO[i]>>sectorO[i]>>rmO[i]>>pixelO[i]>>qieO[i];
    fcho>>adcO[i]>>rm_fiO[i]>>fi_chO[i]>>let_codeO[i]>>crateO[i]>>htrO[i];
    fcho>>fpgaO[i]>>htr_fiO[i]>>dcc_slO[i]>>spigoO[i]>>dccO[i];
    fcho>>fedidO[i]>>geoO[i]>>blockO[i]>>lcO[i];

    ncho++;
  }
    
  for(i = 0; i < NCHO; i++){
    bRow . sideO     = sideO[i];
    bRow . etaO      = etaO[i];
    bRow . phiO      = phiO[i];
    bRow . dphiO     = dphiO[i];
    bRow . depthO    = depthO[i];

    bRow . detO      = detO[i];
    bRow . rbxO      = rbxO[i];
    bRow . sectorO   = sectorO[i];
    bRow . rmO       = rmO[i];
    bRow . pixelO    = pixelO[i];
  
    bRow . qieO      = qieO[i];
    bRow . adcO      = adcO[i];
    bRow . rm_fiO    = rm_fiO[i];
    bRow . fi_chO    = fi_chO[i];
    bRow . let_codeO = let_codeO[i];

    bRow . crateO    = crateO[i];
    bRow . htrO      = htrO[i];
    bRow . fpgaO     = fpgaO[i];
    bRow . htr_fiO   = htr_fiO[i];
    bRow . dcc_slO   = dcc_slO[i];

    bRow . spigoO    = spigoO[i]; 
    bRow . dccO      = dccO[i];
    bRow . fedidO    = fedidO[i];
    
    doc . addLMapHODataset( &bRow, "FullHCALDataset.xml" );

  }
  
  doc . write( "FullHCALmap.xml" );


  cout << "XML Test...done" << endl;

  return 0;
}


int createRBXLoader( string type_, string tag_, string list_file )
{
  string _prefix = "oracle_"; 
  string _comment = "RBX pedestals for GREN 2007"; 
  //string _tag = "AllHCALGRENpartitionPeds4"; // may be overriden by the tag from the brickset
  string _tag = tag_;
  string _version = "GREN2007:1";
  string _subversion = "1";

  std::vector<string> brickFileList;
  char filename[1024];
  //string listFileName = "rbx_ped.list";
  string listFileName = list_file;
  ifstream inFile( listFileName . c_str(), ios::in );
  if (!inFile)
    {
      cout << " Unable to open list file" << endl;
    }
  else
    {
      cout << "List file opened successfully: " << listFileName << endl;
    }
  while (inFile >> filename)
    {
      string fullFileName = filename;
      brickFileList . push_back( fullFileName );
    }
  inFile.close();

  for ( std::vector<string>::const_iterator _file = brickFileList . begin(); _file != brickFileList . end(); _file++ )
    {
      XMLRBXPedestalsLoader::loaderBaseConfig _baseConf;
      XMLRBXPedestalsLoader::datasetDBConfig _conf;

      _baseConf . elements_comment_description = _comment;
      _baseConf . tag_name = _tag;

      _conf . version = _version;
      _conf . subversion = _subversion;
      _conf . comment_description = _comment;

      XMLRBXPedestalsLoader p( &_baseConf );

      p . addRBXSlot( &_conf, (*_file) );
      //p . write( _prefix + (*_file) );
      p . write( (*_file) + ".oracle.xml" );
    }

  return 0;

}


int testocci( void )
{

  HCALConfigDB * db = new HCALConfigDB();
  const std::string _accessor = "occi://CMS_HCL_PRTTYPE_HCAL_READER@anyhost/int2r?PASSWORD=HCAL_Reader_88,LHWM_VERSION=22";
  db -> connect( _accessor );
  vector<unsigned int> _lut = db -> getOnlineLUT( "gren_P3Thr5", 17, 2, 1, 1, 0, 1 );

  cout << "LUT length = " << _lut . size() << endl;
  for ( vector<unsigned int>::const_iterator i = _lut . begin(); i != _lut . end(); i++ )
    {
      cout << (i-_lut.begin()) << "     " << _lut[(i-_lut.begin())] << endl;
    }

  db -> disconnect();
  

  return 0;
}

int test_db_access( void )
{
  // despite the name of the class, can be used with any Oracle DB
  HCALConfigDB * db = new HCALConfigDB();
  const std::string _accessor = "occi://CMS_HCL_PRTTYPE_HCAL_READER@anyhost/int2r?PASSWORD=HCAL_Reader_88,LHWM_VERSION=22";
  db -> connect( _accessor );

  oracle::occi::Connection * _connection = db -> getConnection();  

  unsigned int _version, _crate, _slot, _fiber, _channel;
  hcal::ConfigurationDatabase::FPGASelection _fpga;

  int side   = -1;
  int etaAbs =  1;
  int phi    =  1;
  int depth  =  1;
  string subdetector = "HB";

  cout << "version	" << "eta	" << "phi	" << "depth	" << "subdetector	";
  cout << "crate	" << "slot	" << "fiber	" << "channel	" << endl;

  try {
    Statement* stmt = _connection -> createStatement();
    std::string query = ("SELECT cds.version, CRATE, HTR_SLOT, HTR_FPGA, HTR_FIBER, FIBER_CHANNEL ");
    query += " FROM CMS_HCL_HCAL_CONDITION_OWNER.HCAL_HARDWARE_LOGICAL_MAPS_V3 lmap";
    query += " join cms_hcl_core_condition_owner.cond_data_sets cds ";
    query += " on cds.condition_data_set_id=lmap.condition_data_set_id ";
    query += toolbox::toString(" WHERE SIDE=%d AND ETA=%d AND PHI=%d AND DEPTH=%d AND SUBDETECTOR='%s'", side, etaAbs, phi, depth, subdetector . c_str() );
    
    //SELECT
    ResultSet *rs = stmt->executeQuery(query.c_str());

    while (rs->next()) {
      _version  = rs -> getInt(1);
      _crate    = rs -> getInt(2);
      _slot     = rs -> getInt(3);
      std::string fpga_ = rs -> getString(4);
      if ( fpga_ == "top" ) _fpga = hcal::ConfigurationDatabase::Top;
      else _fpga  = hcal::ConfigurationDatabase::Bottom;
      _fiber    = rs -> getInt(5);
      _channel  = rs -> getInt(6);
      
      cout << _version << "	" << side*etaAbs << "	" << phi << "	" << depth << "	" << subdetector << "		";
      cout << _crate << "	" << _slot << "	" << _fiber << "	" << _channel << endl;
    }
    //Always terminate statement
    _connection -> terminateStatement(stmt);
  } catch (SQLException& e) {
    XCEPT_RAISE(hcal::exception::ConfigurationDatabaseException,::toolbox::toString("Oracle  exception : %s",e.getMessage().c_str()));
  }

  db -> disconnect();
  
  return 0;
}

int testDB( string _tag, string _filename )
{

  HCALConfigDB * db = new HCALConfigDB();
  db -> connect( _filename, "occi://CMS_HCL_PRTTYPE_HCAL_READER@anyhost/int2r?PASSWORD=HCAL_Reader_88,LHWM_VERSION=22" );

  //vector<unsigned int> _lut = db -> getOnlineLUTFromXML( "emap_hcal_emulator_test_luts", 17, 2, 1, 1, 0, 1 );
  //vector<unsigned int> _lut = db -> getOnlineLUT( _tag, 17, 2, 1, 1, 0, 1 );

  HcalDetId _hcaldetid( HcalBarrel, -11, 12, 1 );

  struct timeval _t;
  gettimeofday( &_t, NULL );
  cout << "before getting a LUT: " << _t . tv_sec << "." << _t . tv_usec << endl;

  vector<unsigned int> _lut = db -> getOnlineLUTFromXML( _tag, _hcaldetid . rawId() );

  gettimeofday( &_t, NULL );
  cout << "after getting a LUT: " << _t . tv_sec << "." << _t . tv_usec << endl;

  HcalDetId _hcaldetid2( HcalBarrel, -11, 13, 1 );
  _lut = db -> getOnlineLUTFromXML( _tag, _hcaldetid2 . rawId() );

  gettimeofday( &_t, NULL );
  cout << "after getting a LUT: " << _t . tv_sec << "." << _t . tv_usec << endl;

  _lut = db -> getOnlineLUTFromXML( _tag, _hcaldetid . rawId() );

  gettimeofday( &_t, NULL );
  cout << "after getting a LUT: " << _t . tv_sec << "." << _t . tv_usec << endl;

  cout << "LUT length = " << _lut . size() << endl;
  for ( vector<unsigned int>::const_iterator i = _lut . end() - 1; i != _lut . begin()-1; i-- )
    {
      cout << (i-_lut.begin()) << "     " << _lut[(i-_lut.begin())] << endl;
      break;
    }

  db -> disconnect();
  

  return 0;
}

int lmaptest( string _param ){
  cout << "lmaptest() is running, param = " << _param << endl;

  DBlmapReader * dbr = new DBlmapReader();
  dbr->lrTestFunction();
 
  VectorLMAP* curLMAP = dbr->GetLMAP(30);

  FILE* HBEFfile = fopen ("HCALmapHBEF.txt","w");
  FILE* HOfile   = fopen ("HCALmapHO.txt","w");
  FILE* EMAPfile = fopen ("HCALemap.txt", "w");

  dbr->PrintLMAP(HBEFfile, HOfile, curLMAP);
  
  dbr->PrintEMAPfromLMAP(EMAPfile, curLMAP);
  return 0;
}
int hardware( void )
{
  HcalHardwareXml _hw;

  std::map<string,map<string,map<string,map<int,string> > > > hw_map;

  ifstream infile("HBHOHE.ascii");
  string buf;

  if ( infile . is_open() ){
  cout << "File is open" << endl;
    while ( getline( infile, buf ) > 0 )
      {
	vector<string> _line = splitString( buf );
	//cout << _line . size() << endl;
	if ( _line[0] != "XXXXXX" && _line[1] != "XXXXXX" && _line[2] != "XXXXXX" ){
	  if (_line[3] != "XXXXXX") hw_map[_line[0]][_line[1]]["3040000000000" + _line[2]][1] = "3040000000000" + _line[3];
	  if (_line[4] != "XXXXXX") hw_map[_line[0]][_line[1]]["3040000000000" + _line[2]][2] = "3040000000000" + _line[4];
	  if (_line[5] != "XXXXXX") hw_map[_line[0]][_line[1]]["3040000000000" + _line[2]][3] = "3040000000000" + _line[5];
	}
      }
  }
  
  cout << hw_map . size() << endl;

  _hw . addHardware( hw_map );
  _hw . write("HCAL_hardware.xml");

  return 0;
}

int qie_adc( void )
{
  HcalQIEManager _manager;

  map<HcalChannelId,HcalQIECaps> & _old = _manager . getQIETableFromFile( "qie_normalmode_v3.txt" );
  map<HcalChannelId,HcalQIECaps> & _new = _manager . getQIETableFromFile( "qie_adc_table_after.txt" );

  int goodChannels = 0;
  int badChannels = 0;
  cout << "old size: " << _old.size() << endl;
  cout << "new size: " << _new.size() << endl;
  for (map<HcalChannelId,HcalQIECaps>::const_iterator line=_old.begin(); line!=_old.end(); line++ ){
    HcalQIECaps * the_caps;
    HcalChannelId theId = line -> first;
    if (_new.find(theId)==_new.end()){
      badChannels++;
      the_caps = &_old[theId];
    }
    else{
      goodChannels++;
      the_caps = &_new[theId];
    }
    char buffer[1024];
    int eta = theId.eta;
    int phi = theId.phi;
    int depth = theId.depth;
    sprintf(buffer, "%15d %15d %15d %15s", eta, phi, depth, theId.subdetector.c_str());
    cout << buffer;

    for (int j = 0; j != 32; j++){
      double _x = the_caps->caps[j];
      sprintf(buffer, " %8.5f", _x);
      cout << buffer;      
    }
    cout << endl;
  }

  cout<< goodChannels<< "   " << badChannels << "   " << goodChannels+badChannels << endl;
  return 0;
}

// courtesy of Fedor Ratnikov
std::vector <std::string> splitString (const std::string& fLine) {
  std::vector <std::string> result;
  int start = 0;
  bool empty = true;
  for (unsigned i = 0; i <= fLine.size (); i++) {
    if (fLine [i] == ' ' || fLine [i] == '	' || i == fLine.size ()) {
      if (!empty) {
        std::string item (fLine, start, i-start);
        result.push_back (item);
        empty = true;
      }
      start = i+1;
    }
    else {
      if (empty) empty = false;
    }
  }
  return result;
}
