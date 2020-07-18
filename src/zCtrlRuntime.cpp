/**************************************************************************
 *
 * zCtrlRuntime.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 **************************************************************************/
/*! \file zCtrlRuntime.cpp
*/
#include <iostream>
#include <time.h>

//#pragma disable warning C4786

#ifdef WIN32
	#include <windows.h>
	#include <process.h>
	#include <direct.h>
#else	// Unix
	#include <stdarg.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/unistd.h>
	#include <sys/utsname.h>
	#include <sys/wait.h>
#endif

#include "zDwConst.h"
#include "zDwVersion.h"
#include "zCtrlOsInfo.h"
#include "zCtrlDwRuntime.h"

std::ofstream out("zdwruntime.log");

//debug begin
int ZRuntime::objectCount = 0;
void ZRuntime::print(const std::string& msg = "") {
		if(msg.size() !=0) std::cout << msg << ": ";
		std::cout << "objectCount = " << objectCount << std::endl;
};
//debug end

//*******************************************************************
// Class		: ZRuntime
// Method		: ZRuntime (Constructor)
// Purpose	: Parses the command line parameters
// Inputs	  : 
//			n		- Number of arguments
//			argv	- Array of parameters	
// Outputs	: 
//			None
//*******************************************************************

//globals
ZRuntime* gl_pContext;

/***************************************************************************
 *
 * default constructor
 *
 ***************************************************************************/
ZRuntime::ZRuntime() {
	//debug begin
	objectCount++;
	std::cout << "ZRuntime::ZRuntime()" << std::endl;
	//debug end

	// switches
	analysisLevel_  = DEFAULT_LEXANALYSIS_LEVEL;// detail level of analysis
	checkEncoded_ = false;                // tbd
	checkFileType_ = false;               // tbd
	//checkParamTypes_ = false;           // check command line
	convertFileToWin_ = false;            //
	debugMem_ = false;                    //
	decode_ = false;                      //
	dwListOnly_ = false;                  //
	encode_ = false;                      //
	encodeDecodeOptionSet_ = false;       //
	errors_ = false;                      //
	genOutput_ = false;                   //
	iterateCount_ = false;                //
	iterateDataWarehouse_ = false;        //
	noEncode_ = false;                    //
	dwOutputLogOpen_ = false;              //
	parseErrPlusMinusOffset_ = DEFAULT_LEX_PLUSMINUS_OFFSET;	
	printResults_ = false;                //
	printTokens_ = false;                 //
	quiet_ = false;                       //
	quietLevel_ = MAX_QUIET;              //
	//revealCode_ = false;                  //
	showHiddenOptions_ = false;           //
	showProgress_ = false;                //
	spawnedMode_ = false;                 //

	//data members
	dbConfigName_ = "";         // database: configuration/preference settings	
	dbRootPath_ = "";           // database: full path to the root directory
	dbServerName_ = "";         // database: server name	
	dbServerPassword_ = "";     // database: user password to database server	
	dbServerPort_ = "";         // database: server port as string	
	dbServerUsername_ = "";     // database: user id to database server	
	dtConfigName_ = "";         // datatable: configuration/preference settings	
	dtRootPath_ = "";           // datatable: full path to the root directory
	dtDataInPath_ = "";         // datatable: full path to data input directory	
	dtDataOutPath_ = "";        // datatable: full path to data output directory	
	dtFileAttrSpec_ = "";       // datatable: filespec for datatable attributes	
	dtFileDataSpec_ = "";       // datatable: filespec for datatable data load	
	dtFileInputSpec_ = "";      // datatable: filespec for the input file name = dwroot + dir + db + name	
	dtFileSchemaName_ = "";     // datatable: schema file name to be generated	
	dwConfigName_ = "";         // datawarehouse: configuration/preference settings	
	dwDataBasePath_ = "";       // datawarehouse: default data location	
	dwDriveName_ = "";          // datawarehouse: installation drive letter	
	dwExecPath_ = "";           // datawarehouse: full path to system executable directory	
	dwLogName_ = "";            // datawarehouse: file spec for the system log	
	dwLogPath_ = "";            // datawarehouse: full path to system log directory	
	dwRootPath_ = "";           // datawarehouse: full path to the root directory	
	dwSchemaPath_ = "";         // datawarehouse: full path to the schema directory	
	dwStatsFileSpec_ = "";      // datawarehouse: file spec for statisics file	
	dwTmpPath_ = "";            // datawarehouse: path for temporary files	
	zdwServerId_ = "";          // datawarehouse: the host name of the zdwRemoteServer	
	zdwClientId_ = "";          // datawarehouse: computer id for logging		
}

/***************************************************************************
 *
 * copy constructor
 *
 ***************************************************************************/
ZRuntime::ZRuntime(const ZRuntime& rhs) {
	//debug begin
	objectCount++;
	std::cout << "ZRuntime::ZRuntime(const ZRuntime& rhs)" << std::endl;
	//debug end

	// switches
	analysisLevel_ = rhs.analysisLevel_;       // detail level of analysis
	checkEncoded_  = rhs.checkEncoded_;        // tbd
	checkFileType_ = rhs. checkFileType_;      // tbd
	//checkParamTypes_ = rhs.checkParamTypes_; // check command line
	convertFileToWin_ = rhs.convertFileToWin_; //
	debugMem_      = rhs.debugMem_;                    //
	decode_        = rhs.decode_;                      //
	dwListOnly_    = rhs.dwListOnly_;                  //
	encode_        = rhs.encode_;                      //
	encodeDecodeOptionSet_ = rhs.encodeDecodeOptionSet_;//
	errors_        = rhs.errors_;                      //
	genOutput_     = rhs.genOutput_;                   //
	iterateCount_  = rhs.iterateCount_;                //
	iterateDataWarehouse_ = rhs.iterateDataWarehouse_; //
	noEncode_      = rhs.noEncode_;                    //
	dwOutputLogOpen_ = rhs.dwOutputLogOpen_;           //
	parseErrPlusMinusOffset_ = rhs.parseErrPlusMinusOffset_;	
	printResults_  = rhs.printResults_;                //
	printTokens_   = rhs.printTokens_;                 //
	quiet_         = rhs.quiet_;                       //
	quietLevel_    = rhs.quietLevel_;                  //
	//revealCode_  = rhs.revealCode_;                  //
	showHiddenOptions_ = rhs.showHiddenOptions_;       //
	showProgress_  = rhs.showProgress_;                //
	spawnedMode_   = rhs.spawnedMode_;                 //

	//data members
	dbConfigName_ = rhs.dbConfigName_;         // database: configuration/preference settings	
	dbRootPath_ = rhs.dbRootPath_;             // database: full path to the root directory
	dbServerName_ = rhs. dbServerName_;        // database: server name	
	dbServerPassword_ = rhs.dbServerPassword_; // database: user password to database server	
	dbServerPort_ = rhs.dbServerPort_;         // database: server port as string	
	dbServerUsername_ = rhs.dbServerUsername_; // database: user id to database server	
	dtConfigName_ = rhs.dtConfigName_;         // datatable: configuration/preference settings	
	dtRootPath_ = rhs.dtRootPath_;             // datatable: full path to the root directory
	dtDataInPath_ = rhs.dtDataInPath_;         // datawarehouse: full path to data input directory	
	dtDataOutPath_ = rhs.dtDataOutPath_;       // datawarehouse: full path to data output directory	
	dtFileAttrSpec_ = rhs.dtFileAttrSpec_;     // datatable: filespec for datatable attributes	
	dtFileDataSpec_ = rhs.dtFileDataSpec_;     // datatable: filespec for datatable data load	
	dtFileInputSpec_ = rhs.dtFileInputSpec_;   // datatable: filespec for the input file name = dwroot + dir + db + name	
	dtFileSchemaName_ = rhs.dtFileSchemaName_; // datatable: schema file name to be generated	
	dwConfigName_ = rhs.dwConfigName_;         // datawarehouse: configuration/preference settings	
	dwDataBasePath_ = rhs.dwDataBasePath_;     // datawarehouse: default data location	
	dwDriveName_ = rhs.dwDriveName_;           // datawarehouse: installation drive letter	
	dwExecPath_ = rhs.dwExecPath_;             // datawarehouse: full path to system executable directory	
	dwLogName_ = rhs.dwLogName_;               // datawarehouse: file spec for the system log	
	dwLogPath_ = rhs.dwLogPath_;               // datawarehouse: full path to system log directory	
	dwRootPath_ = rhs.dwRootPath_;             // datawarehouse: full path to the root directory	
	dwSchemaPath_ = rhs.dwSchemaPath_;         // datawarehouse: full path to the schema directory	
	dwStatsFileSpec_ = rhs.dwStatsFileSpec_;   // datawarehouse: file spec for statisics file	
	dwTmpPath_ = rhs.dwTmpPath_;               // datawarehouse: path for temporary files	
	zdwServerId_ = rhs.zdwServerId_;           // datawarehouse: the host name of the zdwRemoteServer	
	zdwClientId_ = rhs.zdwClientId_;           // datawarehouse: computer id for logging		
}

/***************************************************************************
 *
 * assignment operator
 *
 ***************************************************************************/
ZRuntime& ZRuntime::operator= (const ZRuntime& rhs) {
	//debug begin
	objectCount++;
	std::cout << "ZRuntime::ZRuntime(const ZRuntime& rhs)" << std::endl;
	//debug end

	//check for self-assignment
	if(this == &rhs)
		return *this;
	// switches
	analysisLevel_ = rhs.analysisLevel_;       // detail level of analysis
	checkEncoded_  = rhs.checkEncoded_;        // tbd
	checkFileType_ = rhs. checkFileType_;      // tbd
	//checkParamTypes_ = rhs.checkParamTypes_; // check command line
	convertFileToWin_ = rhs.convertFileToWin_; //
	debugMem_      = rhs.debugMem_;                    //
	decode_        = rhs.decode_;                      //
	dwListOnly_    = rhs.dwListOnly_;                  //
	encode_        = rhs.encode_;                      //
	encodeDecodeOptionSet_ = rhs.encodeDecodeOptionSet_;//
	errors_        = rhs.errors_;                      //
	genOutput_     = rhs.genOutput_;                   //
	iterateCount_  = rhs.iterateCount_;                //
	iterateDataWarehouse_ = rhs.iterateDataWarehouse_; //
	noEncode_      = rhs.noEncode_;                    //
	dwOutputLogOpen_ = rhs.dwOutputLogOpen_;           //
	parseErrPlusMinusOffset_ = rhs.parseErrPlusMinusOffset_;	
	printResults_  = rhs.printResults_;                //
	printTokens_   = rhs.printTokens_;                 //
	quiet_         = rhs.quiet_;                       //
	quietLevel_    = rhs.quietLevel_;                  //
	//revealCode_  = rhs.revealCode_;                  //
	showHiddenOptions_ = rhs.showHiddenOptions_;       //
	showProgress_  = rhs.showProgress_;                //
	spawnedMode_   = rhs.spawnedMode_;                 //

	//data members
	dbConfigName_ = rhs.dbConfigName_;         // database: configuration/preference settings	
	dbRootPath_ = rhs.dbRootPath_;             // database: full path to the root directory
	dbServerName_ = rhs.dbServerName_;         // database: server name	
	dbServerPassword_ = rhs.dbServerPassword_; // database: user password to database server	
	dbServerPort_ = rhs.dbServerPort_;         // database: server port as string	
	dbServerUsername_ = rhs.dbServerUsername_; // database: user id to database server	
	dtConfigName_ = rhs.dtConfigName_;         // datatable: configuration/preference settings	
	dtRootPath_ = rhs.dtRootPath_;             // datatable: full path to the root directory
	dtDataInPath_ = rhs.dtDataInPath_;         // datawarehouse: full path to data input directory	
	dtDataOutPath_ = rhs.dtDataOutPath_;       // datawarehouse: full path to data output directory	
	dtFileAttrSpec_ = rhs.dtFileAttrSpec_;     // datatable: filespec for datatable attributes	
	dtFileDataSpec_ = rhs.dtFileDataSpec_;     // datatable: filespec for datatable data load	
	dtFileInputSpec_ = rhs.dtFileInputSpec_;   // datatable: filespec for the input file name = dwroot + dir + db + name	
	dtFileSchemaName_ = rhs.dtFileSchemaName_; // datatable: schema file name to be generated	
	dwConfigName_ = rhs.dwConfigName_;         // datawarehouse: configuration/preference settings	
	dwDataBasePath_ = rhs.dwDataBasePath_;     // datawarehouse: default data location	
	dwDriveName_ = rhs.dwDriveName_;           // datawarehouse: installation drive letter	
	dwExecPath_ = rhs.dwExecPath_;             // datawarehouse: full path to system executable directory	
	dwLogName_ = rhs.dwLogName_;               // datawarehouse: file spec for the system log	
	dwLogPath_ = rhs.dwLogPath_;               // datawarehouse: full path to system log directory	
	dwRootPath_ = rhs.dwRootPath_;             // datawarehouse: full path to the root directory	
	dwSchemaPath_ = rhs.dwSchemaPath_;         // datawarehouse: full path to the schema directory	
	dwStatsFileSpec_ = rhs.dwStatsFileSpec_;   // datawarehouse: file spec for statisics file	
	dwTmpPath_ = rhs.dwTmpPath_;               // datawarehouse: path for temporary files	
	zdwServerId_ = rhs.zdwServerId_;           // datawarehouse: the host name of the zdwRemoteServer	
	zdwClientId_ = rhs.zdwClientId_;           // datawarehouse: computer id for logging		
	return *this;
}

/***************************************************************************
 *
 * constructor
 *
 ***************************************************************************/
// construct using passed args
ZRuntime::ZRuntime(int argc, char * argv[])            
{
  objectCount++;
	std::cout << "ZRuntime::ZRuntime(int n, char * argv[])" << std::endl;

	showHelp(true);

	init();

	// parse out the command line options for this instance
	//if ( ! parseCommandLineOptions(argc, argv) || getError())
	if ( ! parseCommandLineOptions(argc, argv))
	{
			// help switch was specified
			// outputConsoleMsg(L_STATUS,"%s version %s \n%s\n",PROG_NAME, PROG_VERSION, GEN_DATE);
			// showHelp(getShowHiddenOptions());
			showHelp(true);
			complete(RET_NO_ERR);
	}

	//set defaults that are not specified on the command line
	setDefaultOptions();	
}

/***************************************************************************
 *
 * destructor
 *
 ***************************************************************************/
//virtual ~ZRuntime();				               // destructor
ZRuntime::~ZRuntime()
{
  objectCount--;
	print("ZRuntime::~ZRuntime()");

	std::cout << "ZRuntime::~ZRuntime()" << std::endl;

	//delete [] parmList_;
}

/***************************************************************************
 *
 * init
 *
 ***************************************************************************/
void ZRuntime::init()
{
	// switches
	analysisLevel_  = DEFAULT_LEXANALYSIS_LEVEL;
	// detail level of analysis
	checkEncoded_ = false;                // tbd
	checkFileType_ = false;               // tbd
	//checkParamTypes_ = false;             // check command line
	convertFileToWin_ = false;            //
	debugMem_ = false;                    //
	decode_ = false;                      //
	dwListOnly_ = false;                  //
	encode_ = false;                      //
	encodeDecodeOptionSet_ = false;       //
	errors_ = false;                      //
	genOutput_ = false;                   //
	iterateCount_ = false;                //
	iterateDataWarehouse_ = false;        //
	noEncode_ = false;                    //
	dwOutputLogOpen_ = false;              //
	parseErrPlusMinusOffset_ = DEFAULT_LEX_PLUSMINUS_OFFSET;	
	printResults_ = false;                //
	printTokens_ = false;                 //
	quiet_ = false;                       //
	quietLevel_ = MAX_QUIET;              //
	//revealCode_ = false;                  //
	showHiddenOptions_ = false;           //
	showProgress_ = false;                //
	spawnedMode_ = false;                 //

	//data members
	dbConfigName_ = "";         // database: configuration/preference settings	
	dbRootPath_ = "";           // database: full path to the root directory
	dbServerName_ = "";         // database: server name	
	dbServerPassword_ = "";     // database: user password to database server	
	dbServerPort_ = "";         // database: server port as string	
	dbServerUsername_ = "";     // database: user id to database server	
	dtConfigName_ = "";         // datatable: configuration/preference settings	
	dtRootPath_ = "";           // datatable: full path to the root directory
	dtDataInPath_ = "";         // datatable: full path to data input directory	
	dtDataOutPath_ = "";        // datatable: full path to data output directory	
	dtFileAttrSpec_ = "";       // datatable: filespec for datatable attributes	file
	dtFileDataSpec_ = "";       // datatable: filespec for datatable data load file	
	dtFileInputSpec_ = "";      // datatable: filespec for the input file name = dwroot + dir + db + name	
	dtFileSchemaName_ = "";     // datatable: schema file name to be generated	
	dwConfigName_ = "";         // datawarehouse: configuration/preference settings	
	dwDataBasePath_ = "";       // datawarehouse: default data location	
	dwDriveName_ = "";          // datawarehouse: installation drive letter	
	dwExecPath_ = "";           // datawarehouse: full path to system executable directory	
	dwLogName_ = "";            // datawarehouse: file spec for the system log	
	dwLogPath_ = "";            // datawarehouse: full path to system log directory	
	dwRootPath_ = "";           // datawarehouse: full path to the root directory	
	dwSchemaPath_ = "";         // datawarehouse: full path to the schema directory	
	dwStatsFileSpec_ = "";      // datawarehouse: file spec for statisics file	
	dwTmpPath_ = "";            // datawarehouse: path for temporary files	
	zdwServerId_ = "";          // datawarehouse: the host name of the zdwRemoteServer	
	zdwClientId_ = "";          // datawarehouse: computer id for logging				
}

// runtime switches
int  ZRuntime::getAnalysisLevel()							{ return analysisLevel_ ;};									//
bool ZRuntime::getCheckEncoded()							{ return checkEncoded_ ;};                	//
bool ZRuntime::getCheckFileType()           	{ return checkFileType_ ;};               	//
//bool ZRuntime::getCheckLexParamTypes()      	{ return checkParamTypes_ ;};             	//
bool ZRuntime::getConvertFileToDosWindows() 	{ return convertFileToWin_ ;};     	//
bool ZRuntime::getDebugMem()                	{ return debugMem_ ;};                    	//
bool ZRuntime::getDecode()                  	{ return decode_ ;};                      	//
bool ZRuntime::getDwListOnly()              	{ return dwListOnly_ ;};                  	//
bool ZRuntime::getEncode()                  	{ return encode_ ;};                      	//
bool ZRuntime::getEncodeDecodeOptionSet()   	{ return encodeDecodeOptionSet_ ;};       	//
bool ZRuntime::getError()                   	{ return errors_ ;};                      	//
bool ZRuntime::getGenOutput()               	{ return genOutput_ ;};                   	//
int  ZRuntime::getIterateCount()            	{ return iterateCount_ ;};                	//
bool ZRuntime::getIterateDataWarehouse()    	{ return iterateDataWarehouse_ ;};        	//
bool ZRuntime::getNoEncode()                	{ return noEncode_ ;};                    	//
bool ZRuntime::getOutputLogOpen()             { return dwOutputLogOpen_ ;};              	//
int  ZRuntime::getParseErrPlusMinusOffset() 	{ return parseErrPlusMinusOffset_ ;};     	//
bool ZRuntime::getPrintResults()            	{ return printResults_ ;};                	//
bool ZRuntime::getPrintTokens()             	{ return printTokens_ ;};                 	//
bool ZRuntime::getQuiet()                   	{ return quiet_ ;};                       	//
int  ZRuntime::getQuietLevel()              	{ return quietLevel_ ;};                  	//
//bool ZRuntime::revealCode()										{ return revealCode_ ;};                 	  //
bool ZRuntime::getShowHiddenOptions()       	{ return showHiddenOptions_ ;};           	//
bool ZRuntime::getShowProgress()            	{ return showProgress_ ;};                	//
bool ZRuntime::getSpawnedMode()             	{ return spawnedMode_ ;};                 	//

// getters
//database specific
std::string ZRuntime::getDbConfigName()				{ return dbConfigName_; };  		// database: configuration/preference settings
std::string ZRuntime::getDbRootPath()         { return dbRootPath_; };        // database: full path to the root directory
std::string ZRuntime::getDbServerName()				{ return dbServerName_; };      // database: server name
std::string ZRuntime::getDbServerPassword()		{ return dbServerPassword_; };  // database: user password to database server
std::string ZRuntime::getDbServerPort()				{ return dbServerPort_; };      // database: server port as string
std::string ZRuntime::getDbServerUsername()		{ return dbServerUsername_; };  // database: user id to database server
//datatable specific		
std::string ZRuntime::getDtConfigName()				{ return dtConfigName_; };     	// datatable: configuration/preference settings
std::string ZRuntime::getDtDataInPath()				{ return dtDataInPath_; };     	// datatable: full path to data input directory
std::string ZRuntime::getDtDataOutPath()			{ return dtDataOutPath_; };     // datatable: full path to data output directory
std::string ZRuntime::getDtFileAttribSpec()		{ return dtFileAttrSpec_; };    // datatable: filespec for datatable attributes
std::string ZRuntime::getDtFileDataSpec()			{ return dtFileDataSpec_; };    // datatable: filespec for datatable data load
std::string ZRuntime::getDtFileInputSpec()		{ return dtFileInputSpec_; };  	// datatable: filespec for the input file name = dwroot + dir + db + name
std::string ZRuntime::getDtFileSchemaName()		{ return dtFileSchemaName_; };  // datatable: schema file name to be generated
//datawarehouse specific		
std::string ZRuntime::getDwConfigName()				{ return dwConfigName_; };      // datawarehouse: configuration/preference settings
std::string ZRuntime::getDwDataBasePath()			{ return dwDataBasePath_; };    // datawarehouse: default data location
std::string ZRuntime::getDwDriveName()				{ return dwDriveName_; };     	// datawarehouse: installation drive letter
std::string ZRuntime::getDwExecPath()					{ return dwExecPath_; };     		// datawarehouse: full path to system executable directory
std::string ZRuntime::getDwLogName()					{ return dwLogName_; };     		// datawarehouse: file spec for the system log
std::string ZRuntime::getDwLogPath()					{ return dwLogPath_; };     		// datawarehouse: full path to system log directory
std::string ZRuntime::getDwRootPath()					{ return dwRootPath_; };      	// datawarehouse: full path to the root directory
std::string ZRuntime::getDwSchemaPath()				{ return dwSchemaPath_; };    	// datawarehouse: full path to the schema directory
std::string ZRuntime::getDwStatsFileSpec()		{ return dwStatsFileSpec_; };  	// datawarehouse: file spec for statisics file
std::string ZRuntime::getDwTmpPath()					{ return dwTmpPath_; };     		// datawarehouse: path for temporary files
std::string ZRuntime::getZdwServerId()				{ return zdwServerId_; };  			// datawarehouse: the host name of the zdwRemoteServer
std::string ZRuntime::getZdwClientId()				{ return zdwClientId_; };  			// datawarehouse: computer id for logging
			

//void    ZRuntime::setListOnly(bool bFlag) {dwListOnly_ = bFlag;}
//void    ZRuntime::setOutputLogOpen(bool bFlag) {outputLogOpen_ = bFlag;}
//void		ZRuntime::setOutputLogName(char *szFileName) {dwOutputLogSpec_ = szFileName;}
//void    ZRuntime::setDbRootPath(std::string sDbSpec) {dbRootPath_ = sDbSpec;}

/****************************************************************
*
* ParseCommandLineOptions
*
* Manages the deconstruction of parameters passed to this app. 
* The return value will be false if not enough parameters 
* were supplied, OR the help switch was specified ("?").
* See method ShowHelp() for argument usage.
*
****************************************************************/
bool ZRuntime::parseCommandLineOptions(int n, char *argv[])
{
	bool bReturn = true;
	errors_ = false;

	std::vector<std::string> parmList_;

	int i = 0;

	for(i=0; i < n; i++) 
		//parmList_[i] = argv[i];
		parmList_.push_back(argv[i]);

	//test param dump
/*
	std::string eachParam = "";
	for(i=0; i < n; i++) {
		eachParam = parmList_[i];
		printf("argv[%d]=%s\n",i,eachParam.c_str());
	}
*/
/*
		printf("\nUsage:\n");
		printf("-?      [option]     Display this usage list.\n");
		printf("-a      [n]          Analysis level.\n");
		printf("-db     [database]   Database directory within warehouse.\n");
		printf("-dt     [table]      Database table within database directory.\n");
		printf("-dw     [root]       Path to the data warehouse.\n");
		printf("-dbtype [mysql]      Database server type.\n");
		printf("-f      [filename]   Specific file name to parse.\n");
		printf("-g      [filename]   Generate database table schema.\n");
		printf("-l      [option]     List contents of warehouse only.\n");
		printf("-o      [filename]   Output file to use for reporting.\n");
		printf("-port   [nnnn]       Server Port-Number.\n");
		printf("-pwd    [password]   Database password.\n");
		printf("-s      [option]     Output to stream [stderr | stdout].\n");
		printf("-server [id]         Database Server ID.\n");
		printf("-t      [directory]  Temporary file location.\n");
		printf("-user   [username]   Username to connect to database server.\n");
		
		printf("-c      [n]          Display character code.\n");
		printf("-e      [n]          Number of offset lines to display upon parse error.\n");
		printf("-i      [n]          Iteration count (for testing iteration only!.\n");
		printf("-x      [option]     Encryption options. Set option = off|encode|decode|check.\n");
		printf("-z      [option]     Spawn from another process.\n");
*/

	//skipping over the first twp params, i=2
	for(i=2; i < n && !errors_; )
	{
		//printf("-?      [option]     Display this usage list.\n");
		//if(parmList_[i] == "-?" ) {
		if(parmList_[i] == "-?" ) {
			bReturn = false;
			// check if they asked for the hidden options to be shown...
			if(i < n-1)
			{
				//if(parmList_[i+1] == "z2it") {
				if(parmList_[i+1] == "z2it") {
					showHiddenOptions_ = true;
					i+=2;
				}
			}
			i++;
		} else 

		//printf("-a      [n]          Analysis level.\n");
		if(parmList_[i] == "-a" ) {
			// specify analysis level
			//if ( sscanf(parmList_[i+1].c_str(), "%d", &analysisLevel_) )
			if ( sscanf(parmList_[i+1].c_str(), "%d", &analysisLevel_) )
				i+=2;
			else {
				errors_ = true;
				//printf("Scanf error on Analysis Level\n");
			}

			if (analysisLevel_ < MIN_LEXANALYSIS_LEVEL) analysisLevel_ = MIN_LEXANALYSIS_LEVEL;
			if (analysisLevel_ > MAX_LEXANALYSIS_LEVEL) analysisLevel_ = MAX_LEXANALYSIS_LEVEL;
		}	else 
		
		//printf("-c      [n]          Display character code.\n");
		/*
		if(parmList_[i] == "-c" && i < n-1) {
						// reveal ASCII/ANSI interpretation for the specified code
						m_bRevealCode = true;
						if ( n >= i + 2)
						{
							char *m_sCode = new char[12];
							if (m_sCode == NULL) complete(RET_MEM_ERR);
							strcpy(m_sCode, parmList_[i+1].c_str());
							m_cCode = (unsigned char)(atoi(m_sCode) % 256);
							i++;
						}
						i++;
		} else
    */

		//printf("-db     [database]   Database directory within warehouse (optional).\n");
		if(parmList_[i] == "-db" && i < n-1) {
			// specify database directory
			dbRootPath_ = parmList_[i+1].c_str();			
			//dwDataBasePath_ += "\\";
			//dwDataBasePath_ += parmList_[i+1].c_str();
			// make sure to turn off the iteration
			//iterateDataWarehouse_ = false;
			i+=2;
		} else

		//printf("-dt     [table]      Database table directory.\n");
		if(parmList_[i] == "-dt" && i < n-1) {
			//parsing one single file based on this switch, no iteration
			// specify table directory
			dtRootPath_ = parmList_[i+1].c_str();
			//set the filename to the system default
			// default database table input file name.
/*
			dtFileInputSpec_ += dwDataBasePath_;
			dtFileInputSpec_ += "\\";
			dtFileInputSpec_ += parmList_[i+1].c_str();
			dtFileInputSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileInputSpec_ += "\\";			
			dtFileInputSpec_ += DEFAULT_ZDW_DTFILEINPUTNAME;
			dtFileAttrSpec_ += dwDataBasePath_;
			dtFileAttrSpec_ += "\\";
			dtFileAttrSpec_ += parmList_[i+1].c_str();
			dtFileAttrSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileAttrSpec_ += "\\";			
			dtFileAttrSpec_ += DEFAULT_ZDW_DTFILEATTRNAME;
			dtFileDataSpec_ += dwDataBasePath_;
			dtFileDataSpec_ += "\\";
			dtFileDataSpec_ += parmList_[i+1].c_str();
			dtFileDataSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileDataSpec_ += "\\";			
			dtFileDataSpec_ += DEFAULT_ZDW_DTFILEDATANAME;
*/
			// make sure to turn off the dw iteration
			iterateDataWarehouse_ = false;
			i+=2;
		} else

		//printf("-dw     [root]       Path to the data warehouse. (default = current directory)\n");
		if(parmList_[i] == "-dw" && i < n-1) {
			// specify the Data Warehouse dir
			dwRootPath_ = parmList_[i+1].c_str();
			dwDataBasePath_ = dwRootPath_;
			dwDataBasePath_	+= DEFAULT_ZDW_DWDATABASEPATH;
			dwSchemaPath_ = dwRootPath_;
			dwSchemaPath_ += DEFAULT_ZDW_DWSCHEMAPATH;
			i+=2;
		} else
/*
		//printf("-dbtype [mysql]      Database server type.\n");
		if(parmList_[i] == "-mysql" ) {
			m_sTargetDB = "mysql";
			i++;
		} else 
*/		
		//printf("-e      [n]          Number of offset-lines to display upon parse error.\n");
		if(parmList_[i] == "-e" ) {
						// set the +/- offset for dumping lines from the file 
						if ( n >= i + 2)
						{							
							// specify number error lines to dump
							if ( sscanf(parmList_[i+1].c_str(), "%d", &lexErrPlusMinusOffset_) )
								i+=2;
							else
								errors_ = true;														
						}
						else
						{
							// set the default quiet level
							lexErrPlusMinusOffset_ = DEFAULT_LEX_PLUSMINUS_OFFSET;
						}
						if (lexErrPlusMinusOffset_ < 0) 
							lexErrPlusMinusOffset_ = 0;
						i++;
		} else 

		//printf("-f      [filename]   Specific file name to parse.\n");
		if(parmList_[i] == "-f" && i < n-1) {
			// specify text file for parsing, optional (will default to "input.zdi" if not specified
			dtFileInputSpec_ = parmList_[i+1].c_str();
			// make sure to turn off the dw iteration
			iterateDataWarehouse_ = false;
			i+=2;
		} else 
/*
		//printf("-g      [filename]   Generate database table schema.\n");
		if(parmList_[i] == "-g" && i < n-1) {
			m_bGenOutput = true;
			// specify the Data Warehouse dir
			m_sDgFileName = parmList_[i+1];			
			i+=2;
		} else
			
		//printf("-i      [n]          Iteration count (for testing iteration only!.\n");
		if(parmList_[i] == "-i" ) {
			// specify number of filesets to iterate
			if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nIterateCount) )
				i+=2;
			else
				errors_ = true;
		} else
*/			
		//printf("-l      [option]     List contents of warehouse only.\n");
		if(parmList_[i] == "-l" ) {
			// only list the databases in the data warehouse
			dwListOnly_ = true;			
			i++;
		} else

		//printf("-o      [filename]   Output file to use for reporting.\n");
		if(parmList_[i] == "-o" && i < n-1) {
			// specify the output file
			dwOutputLogSpec_ = parmList_[i+1];			
			i+=2;
		} else 
/*		
		//printf("-port   [nnnn]       Port.\n");		
		if(parmList_[i] == "-port" && i < n-1) {
			//printf("-port   [nnnn]       Port.\n");
			if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nPort) )
				i+=2;
			else
				errors_ = true;
		} else 
*/
		//printf("-pwd    [password]   Database password.\n");
		if(parmList_[i] == "-pwd" && i < n-1) {
			dbServerPassword_ = parmList_[i+1].c_str();
			i+=2;
		} else
/*
		//printf("-s      [option]     Output to stream [stderr | stdout].\n");
		if(parmList_[i] == "-o"  && i < n-1) {
						// specify output file
							if (parmList_[i+1].c_str() == "stdout")
							{
								dwOutputLogSpec_ = "";
								m_fpOutput = stdout;
								i++;
							}
							else if (parmList_[i+1].c_str() == "stderr")
							{
								dwOutputLogSpec_ = "";
								m_fpOutput = stderr;
								i++;
							}
							else
							{
								dwOutputLogSpec_ = parmList_[i+1].c_str();
								m_fpOutput = NULL;
								i++;
							}
							i++;
		} else 
*/
		//printf("-server [option]     Database Server ID.\n");
		if(parmList_[i] == "-server" && i < n-1) {			
			dbServerName_ = parmList_[i+1].c_str();
			i+=2;
		} else

		//printf("-t      [directory]  Temporary file location.\n");
		if(parmList_[i] == "-t" && i < n-1) {
			// specify directory for temporary files
			dwTmpPath_ = parmList_[i+1].c_str();		
			i+=2;
		} else

		//printf("-u      [option]     ANSI-ASCII file identifcation. option = params|file.\n");
		if(parmList_[i] == "-u" ) {
			// force the file to be_ first converted to Dos/Windows format before parsing
			convertFileToWin_ = true;
			i++;
		} else 

		//printf("-user   [username]   Username to connect to database server].\n");
		if(parmList_[i] == "-user" && i < n-1) {
		//printf("-user   [username]   Username to connect to database server].\n");
			dbServerUsername_ = parmList_[i+1].c_str();
			i+=2;
		} else
		
		//printf("-x      [option]     Encryption options. Set option = off|encode|decode|check.\n");
		if(parmList_[i] == "-x" && i < n-1) {
			// encryption options. Following option can be "encode" or "decode"
			if ( n >= i + 2)
			{
				if (parmList_[i+1] == "encode")
							{
								encode_ = true;
								encodeDecodeOptionSet_ = true;
							}
				else if (parmList_[i+1] == "decode")
							{
								decode_ = true;
								encodeDecodeOptionSet_ = true;
							}
				else if(parmList_[i+1] == "check")
							{
								checkEncoded_ = true;
								encodeDecodeOptionSet_ = true;
							}
				else if (parmList_[i+1] == "off")
							{
								encode_ = false;
								decode_ = false;
								encodeDecodeOptionSet_ = false;
							}
				i+=2;
			}
		} else 

		//printf("-z      [option]     Spawn from another process.\n");
		if(parmList_[i] == "-z" && i < n-3) {
			//printf("-z      [option]     Spawn from another process.\n");
			// spawned mode: we are a child process of another tool instance which is iterating a depot
			// the format of parameters that follow this option is: 
			// -z <spec file> <master database file> <master table file>
			spawnedMode_ = true;
			quietLevel_ = MAX_QUIET;	// make sure that output is not shown directly.
			// get the file name that follows as the statistics file
			dwStatsFileSpec_ = parmList_[i+1].c_str();
			// get the file name that follows this as the master database list file
			dwMasterDbFileSpec_ = parmList_[i+2].c_str();
			// get the file name that follows this as the master table list file
			dwMasterDtFileSpec_ = parmList_[i+3].c_str();
			i+=4;
		}	
		else {
			//printf("Param Errors.\n");
			errors_ = true;
			i++;
		}

	} 

	//check for mandatory switches
	//if( m_sDwRootDir == "" || m_sServer == "" )
	//	errors_ = true;

	return bReturn;

	//ContextToString();
}

/****************************************************************
*
* toString
*
****************************************************************/

std::string ZRuntime::toString()
{
	int i = 0;

	std::string allParams = "";
/*
	//concat all params
	while(parmList_[i] != "") {
		allParams += parmList_[i] + " ";
		i++;
		//if(parmList_[i] != "") {

	}
*/
	return allParams;
}

/*
std::string ZRuntime::toString()
{
		printf("\nUsage:\n");
		printf("-?      [option]     Display this usage list.\n");
		printf("-a      [n]          Analysis level.\n");
		printf("-db     [database]   Database directory within warehouse.\n");
		printf("-dt     [table]      Database table within database directory.\n");
		printf("-dw     [root]       Path to the data warehouse.\n");
		printf("-dbtype [mysql]      Database server type.\n");
		printf("-f      [filename]   Specific file name to parse.\n");
		printf("-g      [filename]   Generate database table schema.\n");
		printf("-l      [option]     List contents of warehouse only.\n");
		printf("-o      [filename]   Output file to use for reporting.\n");
		printf("-port   [nnnn]       Port.\n");
		printf("-pwd    [password]   Database password.\n");
		printf("-s      [option]     Output to stream [stderr | stdout].\n");
		printf("-server [id]         Database Server ID.\n");
		printf("-t      [directory]  Temporary file location.\n");
		printf("-user   [username]   Username to connect to database server].\n");
		
		printf("-c      [n]          Display character code.\n");
		printf("-e      [n]          Number of offset lines to display upon parse error.\n");
		printf("-i      [n]          Iteration count (for testing iteration only!.\n");
		printf("-x      [option]     Encryption options. Set option = off|encode|decode|check.\n");
		printf("-z      [option]     Spawn from another process.\n");


	for(i=1; i < n && !errors_; ) 
	{
		//printf("-?      [option]     Display this usage list.\n");
		if(parmList_[i] == "-?" ) {
			bReturn = false;
			// check if they asked for the hidden options to be shown...
			if(i < n-1)
			{
				if(parmList_[i+1] == "z2it") {
					m_bShowHiddenOptions = true;
					i+=2;
				}
			}
			i++;
		} else 

		//printf("-a      [n]          Analysis level.\n");
		if(parmList_[i] == "-a" ) {
			// specify analysis level
			if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nAnalysisLevel) )
				i+=2;
			else
				errors_ = true;
			if (m_nAnalysisLevel < MIN_LEXANALYSIS_LEVEL) m_nAnalysisLevel = MIN_LEXANALYSIS_LEVEL;
			if (m_nAnalysisLevel > MAX_LEXANALYSIS_LEVEL) m_nAnalysisLevel = MAX_LEXANALYSIS_LEVEL;
		}	else 
		
		//printf("-c      [n]          Display character code.\n");
		if(parmList_[i] == "-c" && i < n-1) {
						// reveal ASCII/ANSI interpretation for the specified code
						m_bRevealCode = true;
						if ( n >= i + 2)
						{
							char *m_sCode = new char[12];
							if (m_sCode == NULL) complete(RET_MEM_ERR);
							strcpy(m_sCode, parmList_[i+1].c_str());
							m_cCode = (unsigned char)(atoi(m_sCode) % 256);
							i++;
						}
						i++;
		} else

		//printf("-db     [directory]  Database directory within warehouse (optional).\n");
		if(parmList_[i] == "-b" && i < n-1) {
			// specify database directory
			dbRootDir_ = parmList_[i+1];
			// make sure to turn off the dw iteration
			//m_bIterateDataWarehouse = false;
			i+=2;
		} else
						
		//printf("-dbtype [mysql]      Database server type.\n");
		if(parmList_[i] == "-mysql" ) {
			m_sTargetDB = "mysql";
			i++;
		} else 
		
		//printf("-e      [n]          Number of offset lines to display upon parse error.\n");
		if(parmList_[i] == "-e" ) {
						// set the +/- offset for dumping lines from the file 
						if ( n >= i + 2)
						{							
							// specify number error lines to dump
							if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nLexErrPlusMinusOffset) )
								i+=2;
							else
								errors_ = true;														
						}
						else
						{
							// set the default quiet level
							m_nLexErrPlusMinusOffset = DEFAULT_PLUSMINUS_OFFSET;
						}
						if (m_nLexErrPlusMinusOffset < 0) 
							m_nLexErrPlusMinusOffset = 0;
						i++;
		} else 

		//printf("-f      [filename]   Specific file name to parse.\n");
		if(parmList_[i] == "-f" && i < n-1) {
			// specify table file for parsing, optional (will default to "input.zdi" if not specified
			m_sTxtFileName = parmList_[i+1];
			// make sure to turn off the dw iteration
			m_bIterateDataWarehouse = false;
			i+=2;
		} else 

		//printf("-g      [filename]   Generate database table schema.\n");
		if(parmList_[i] == "-g" && i < n-1) {
			m_bGenOutput = true;
			// specify the Data Warehouse dir
			m_sDgRootDir = parmList_[i+1];			
			i+=2;
		} else
			
		//printf("-i      [n]          Iteration count (for testing iteration only!.\n");
		if(parmList_[i] == "-i" ) {
			// specify number of filesets to iterate
			if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nIterateCount) )
				i+=2;
			else
				errors_ = true;
		} else
		
		//printf("-k      [directory]  Temporary file location.\n");
		if(parmList_[i] == "-k" && i < n-1) {
			// specify directory for temporary files
			m_sTmpDir = parmList_[i+1];		
			i+=2;
		} else
			
		//printf("-l      [option]     List contents of warehouse only.\n");
		if(parmList_[i] == "-l" ) {
			// only list the databases in the data warehouse
			m_bDwListOnly = true;			
			i++;
		} else 

		//printf("-o      [filename]   Output file to use for reporting.\n");
		if(parmList_[i] == "-o" && i < n-1) {
			// specify the output file
			dwOutputLogSpec_ = parmList_[i+1];			
			i+=2;
		} else 
		
		//printf("-port   [nnnn]       Port.\n");		
		if(parmList_[i] == "-port" && i < n-1) {
			//printf("-port   [nnnn]       Port.\n");
			if ( sscanf(parmList_[i+1].c_str(), "%d", &m_nPort) )
				i+=2;
			else
				errors_ = true;
		} else 

		//printf("-pwd    [password]   Database password.\n");
		if(parmList_[i] == "-pwd" && i < n-1) {
			m_sPassword = parmList_[i+1];
			i+=2;
		} else

		//printf("-s      [option]     Output to stream [stderr | stdout].\n");
		if(parmList_[i] == "-o" ) {
						// specify output file
							if (parmList_[i+1] == "stdout")
							{
								dwOutputLogSpec_ = "";
								m_fpOutput = stdout;
							}
							else if (parmList_[i+1] == "stderr")
							{
								dwOutputLogSpec_ = "";
								m_fpOutput = stderr;
							}
							else
							{
								dwOutputLogSpec_ = parmList_[i+1];
								m_fpOutput = NULL;
							}
		} else 

		//printf("-server [option]     Database Server ID.\n");
		if(parmList_[i] == "-server" && i < n-1) {			
			m_sServer = parmList_[i+1];
			i+=2;
		} else
		
		//printf("-t      [table]      Database table directory.\n");
		if(parmList_[i] == "-t" && i < n-1) {
			// specify table directory
			dtRootDir_ = parmList_[i+1];
			// make sure to turn off the dw iteration
			m_bIterateDataWarehouse = false;
			i+=2;
		} else
			
		//printf("-u      [option]     ANSI-ASCII file identifcation. option = params|file.\n");
		if(parmList_[i] == "-u" ) {
			// force the file to be first converted to Dos/Windows format before parsing
			m_bConvertFileToDosWindows = true;
			i++;
		} else 

		//printf("-user   [username]   Username to connect to database server].\n");
		if(parmList_[i] == "-user" && i < n-1) {
		//printf("-user   [username]   Username to connect to database server].\n");
			m_sUsername = parmList_[i+1];
			i+=2;
		} else
			
		//printf("-w      [path]       Path to the data warehouse. (default = current directory)\n");
		if(parmList_[i] == "-w" && i < n-1) {
			// specify the Data Warehouse dir
			m_sDwRootDir = parmList_[i+1];			
			i+=2;
		} else
		
		//printf("-x      [option]     Encryption options. Set option = off|encode|decode|check.\n");
		if(parmList_[i] == "-x" && i < n-1) {
			// encryption options. Following option can be "encode" or "decode"
			if ( n >= i + 2)
			{
				if (parmList_[i+1] == "encode")
							{
								m_bEncode = true;
								m_bEncodeDecodeOptionSet = true;
							}
				else if (parmList_[i+1] == "decode")
							{
								m_bDecode = true;
								m_bEncodeDecodeOptionSet = true;
							}
				else if(parmList_[i+1] == "check")
							{
								m_bCheckEncoded = true;
								m_bEncodeDecodeOptionSet = true;
							}
				else if (parmList_[i+1] == "off")
							{
								m_bEncode = false;
								m_bDecode = false;
								m_bEncodeDecodeOptionSet = false;
							}
				i+=2;
			}
		} else 

		//printf("-z      [option]     Spawn from another process.\n");
		if(parmList_[i] == "-z" ) {
			// spawned mode: we are a child process of another tool instance which is iterating a depot
			// the format of parameters that follow this option is: 
			// -z <spec file> <master class list file> <master fileset list file>
			m_bSpawnedMode = true;
			m_nQuietLevel = MAX_QUIET;	// make sure that output is not shown directly.
			// get the file name that follows as the statistics file
			if ( n >= i + 2)
			{
				m_sStatsFileSpec = parmList_[i+1];
			}
			// get the file name that follows this as the master fileset list file
			if (n >= i + 3)
			{
				m_sDwFileSpec = parmList_[i+3];
			}
			i+=3;
		}	
		else {
			errors_ = true;
			i++;
		}
	} 
}
*/
/****************************************************************
*
* SetDefaultOptions()
*
* Sets the default settings for parameters not explicitly put 
* on the command line.
*
****************************************************************/
void ZRuntime::setDefaultOptions()
{
	// can we get the computer name? 
	#ifdef WIN32
		char *szName = getenv("COMPUTERNAME");
	#else // Unix
		struct utsname osinfo;
		char szName[100];
		szName[0] = '\0';
		//const char *szName;
		if (uname(&osinfo) >= 0)
		{
			/*
			if ((unsigned long int)strlen(osinfo.nodename) < sizeof(szName) )
			{
				strcpy(szName, osinfo.nodename);
			}
			*/
		}
	#endif
	if ( (szName != NULL) && (szName[0] != '\0') )
	{
		//szName = new char[strlen(szName) + 1];
		//strcpy(m_szComputerName, szName);
		zdwClientId_ = szName;
	}

	if(dbServerUsername_	 == "")
	{ 
		dbServerUsername_	  = "igor";
	}

	if(dbServerPassword_ == "")
	{
		dbServerPassword_	 = "igorroot";
	}

	if(dbServerPort_		  == "")
	{
		dbServerPort_		   = "3365";
	}

	if(dwTargetDb_	== "")
	{
		dwTargetDb_	 = "mysql";
	}

	if (dwRootPath_ == "") 
	{
		// set default path to the root directory
		dwRootPath_ = DEFAULT_ZDW_DWROOTPATH;
	}

	if (dwDataBasePath_ == "") 
	{
		// set default path to the data directory
		dwDataBasePath_ = DEFAULT_ZDW_DWDATABASEPATH;
	}

	if (dwSchemaPath_ == "") 
	{
		// set default path to the schema directory
		dwSchemaPath_ = DEFAULT_ZDW_DWSCHEMAPATH;
	}

	if (dtFileInputSpec_ == "")
	{
		// default table file name
		dtFileInputSpec_ = DEFAULT_ZDW_DTFILEINPUTNAME;
	}

	if (dtFileAttrSpec_ == "")
	{
		// default database file name
		dtFileAttrSpec_ = DEFAULT_ZDW_DTFILEATTRNAME;
	}

	if (dtFileDataSpec_ == "")
	{
		// default database file name
		dtFileDataSpec_ = DEFAULT_ZDW_DTFILEDATANAME;
	}

	if (dwTmpPath_ == "")
	{
		// default TMP directory
		char *szTmp = getenv("TEMP");
		if (szTmp == NULL)
		{
			szTmp = getenv("TMP");
		}
		if (szTmp != NULL)
		{
			dwTmpPath_ = szTmp;
		}
		else
		{
			// no temporary directory available, use the local directory
			szTmp = new char[5];
			if (szTmp == NULL) complete(RET_MEM_ERR);
			sprintf(szTmp, ".%s",SEPARATOR);
			dwTmpPath_ = szTmp;
		}
	}

/*
	if ( (dwOutputLog_ == "") && (pOutput == NULL) )
	{
		if ( ( ! getSpawnedMode()) && ( ! getIterateDataWarehouse()) )
		{
			dwOutputLogName = "";
			pOutputLogFileFile__ = stdout;
		}
		else
		{

			// create a temporary file!
			char cFileSpec[100];
			if (zdwClientId_ != NULL)
			{
				sprintf(cFileSpec,".%s%s.%s",SEPARATOR,m_szComputerName,DEFAULT_OUTPUT_FILENAME);
			}
			else
			{
				sprintf(cFileSpec,".%s%s",SEPARATOR,DEFAULT_OUTPUT_FILENAME);
			}
			dwOutputLogSpec_ = cFileSpec;
		}
	}
*/
}


/*****************************************************************************
*
* complete
*
* Cleans up any memory we allocated, and terminates the thread.
*
*****************************************************************************/
void ZRuntime::complete(int nRetCode)
{
/*
	if ((nRetCode == RET_NO_ERR) && (iterateDataWarehouse_) && (dwOutputLogSpec_ != "") && ( ! m_bEncodeDecodeOptionSet ) )
	{
		outputConsoleMsg(L_REQ,"Complete results in %s\n",dwOutputLogSpec_.c_str());
	}
	else if ((nRetCode != RET_NO_ERR) && (!m_bSpawnedMode) && (!iterateDataWarehouse_) && (dwOutputLogSpec_ != "") )
	{
		outputConsoleMsg(L_REQ,"Complete results in %s\n",dwOutputLogSpec_.c_str());
	}
	else if (nRetCode == RET_NO_ERR)
	{
		outputConsoleMsg(L_STATUS,"\nDone.\n");
	}

	//if ( (nRetCode != RET_NO_ERR) && (nRetCode != RET_PARAM_ERR) && (nRetCode != RET_PARSE_ERR) )
	if ( (nRetCode != RET_NO_ERR) && (nRetCode != RET_PARSE_ERR) )
	{
		if ( (m_bSpawnedMode) && (m_fpOutput != NULL) )
		{
			switch(nRetCode)
			{
				case RET_MEM_ERR:	
					fprintf(m_fpOutput,"-> A fatal memory allocation error occurred, no further information avaialable.\n");	break;
				case RET_INT_ERR:
					fprintf(m_fpOutput,"-> A fatal internal error occurred, no further information avaialable.\n");	break;
				case RET_FILE_ERR:
					fprintf(m_fpOutput,"-> A fatal file processing error occurred, no further information avaialable.\n");	break;
				case RET_ARG_ERR:
					fprintf(m_fpOutput,"-> A fatal error occurred processing program arguments, no further information avaialable.\n");	break;
				case RET_PARSER_INT_ERR:
					fprintf(m_fpOutput,"-> An internal parser error ocurred, no further information avaialable.\n"); break;
				case RET_BUF_OVERFLOW:
					fprintf(m_fpOutput,"-> A fatal error occurred: buffer overflow, no further information avaialable.\n");	break;
				case RET_OS_ERR: 
					fprintf(m_fpOutput,"-> An operating-system specific error occurred, last error was: ");
					#ifdef WIN32
						fprintf(m_fpOutput, "%lu\n",GetLastError());
					#else	// Unix
						fprintf(m_fpOutput, "%d\n",errno);
					#endif
					break;
			}
		}
		else
		{
			switch (nRetCode)
			{
			case RET_MEM_ERR:		outputConsoleMsg(L_STATUS,"Internal memory error.\n"); break;
			case RET_INT_ERR:		outputConsoleMsg(L_STATUS,"Interal error occurred.\n"); break;
			default:						break;		// no error, or a parse error (output already given for these)
			}
		}
	}
*/
/*
	if (m_pDtIterator != NULL) 
	{
		delete m_pDtIterator;
	}
	if (m_pLogIterator != NULL)
	{
		delete m_pLogIterator;
	}
	if (m_pLexFileIdentifier != NULL)
	{
		delete m_pLexFileIdentifier;
	}
	if (m_pLexClassCollector != NULL)
	{
		delete m_pLexClassCollector;
	}
	if (m_szComputerName != NULL)
	{
		delete m_szComputerName;
	}
	if (m_sExecPath != NULL)
	{
		delete m_sExecPath;
	}
	if (m_sTxtFileName != NULL)
	{
		delete m_sTxtFileName;
	}
	if (m_sDwDir != NULL)
	{
		delete m_sDwDir;
	}
	if (m_sFileName != NULL)
	{
		delete m_sFileName;
	}
	if (m_sTmpDir != NULL)
	{
		delete m_sTmpDir;
	}
	if (m_sStatsFileSpec != NULL)
	{
		delete m_sStatsFileSpec;
	}
	if (m_sClassFileSpec != NULL)
	{
		delete m_sClassFileSpec;
	}
	if (m_sDwFileSpec != NULL)
	{
		delete m_sDwFileSpec;
	}
	if (m_pFileEncoder != NULL)
	{
		delete m_pFileEncoder;
	}
	if (m_pLexAnalyzer != NULL)
	{
		delete m_pLexAnalyzer;
	}
	if (m_bOutputLogOpen)
	{
		fclose(m_fpOutput);
		m_fpOutput = NULL;
	}
	if (dwOutputLogSpec_ != NULL)
	{
		delete dwOutputLogSpec_;
	}
	if (m_bConvertFileToDosWindows)
	{
		// remove the temp file spec
		if (m_sOldFileSpec != NULL)
		{
			OutputConsoleMsg(L_STATUS,"\nRemoving the temporary file %s\n",m_sFileSpec); 
			remove(m_sFileSpec);
			delete m_sOldFileSpec;
		}
	}
	if (m_sFileSpec != NULL)
	{
		delete m_sFileSpec;
	}
*/
	exit(nRetCode);
}

/*****************************************************************************
*
* OutputLogMsg
*
*****************************************************************************/
int ZRuntime::outputLogMsg(int nLevel, char *msg, ...)
{
/*
	// first check the global settings
	//if (quietLevel_ == MAX_QUIET)  return 0; // ignore it, we're in complete silent mode
	//if (quietLevel_ < nLevel)			return 0;

  char cMsgBuf[2048];    // the output message buffer
  int nCount = -1;
  va_list argp;
  int nResult = 1;

	va_start (argp, msg);
  nCount = vsprintf (cMsgBuf, msg, argp);
  va_end (argp);

  //strcat(cMsgBuf, "\n");
	//fprintf(m_pContext->getOutputLogPtr(),"No databases found in '%s'\n",m_pContext->getDwRootDir().c_str());

	return fprintf(pOutputLogFile_,cMsgBuf);
*/
	//temp - fix
	return 0;

}

/*****************************************************************************
*
* OutputConsoleMsg
*
*****************************************************************************/
int ZRuntime::outputConsoleMsg(int nLevel, char *msg, ...)
{
	// first check the global settings
	if (quietLevel_ == MAX_QUIET)  return 0; // ignore it, we're in complete silent mode
	if (quietLevel_ < nLevel)			return 0;

  char cMsgBuf[2048];    // the output message buffer
  int nCount = -1;
  va_list argp;
  int nResult = 1;

	va_start (argp, msg);
  nCount = vsprintf (cMsgBuf, msg, argp);
  va_end (argp);

  //strcat(cMsgBuf, "\n");

	return printf(cMsgBuf);
}

/****************************************************************
*
* BuildAndCheckInputFileSpecs
*
* Uses the following attributes:  
* dwRootPath_, dbRootPath_,
* dtRootPath_, dtFileInputSpec_
*
* Builds the complete path to a database input file
****************************************************************/
bool ZRuntime::buildAndCheckInputFileSpec()
{
	if ( (dwRootPath_ == "") || (dbRootPath_ == "") 
		|| (dtRootPath_ == "") || (dtFileInputSpec_ == "") ) 
		complete(RET_ARG_ERR);

	bool bReturn = true;

	//char cFileSpec[PATHMAX];
	//const char *cFileSpec;
	//strcpy(cFileSpec, dwRootPath_.c_str());
	//strcpy(cFileSpec, dwRootPath_);
	std::string cFileSpec;
	//dwRootPath_.copy(cFileSpec.c_str(), dwRootPath_.size() + 1);
	cFileSpec.append(dwRootPath_);
	cFileSpec[dwRootPath_.size()] = '\0';
	//int nLen = strlen(cFileSpec);
	int nLen = dwRootPath_.size();
	if (nLen > 0)
	{
		if ( (cFileSpec[nLen - 1] != '\\') || (cFileSpec[nLen - 1] != '/') )
		{
			//strcat(cFileSpec.c_str(), SEPARATOR);
			cFileSpec.append(SEPARATOR);

		}
	}

	//strcat(cFileSpec, DEFAULT_ZDW_DWDATABASEPATH);
	cFileSpec.append(DEFAULT_ZDW_DWDATABASEPATH);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, dbRootPath_.c_str());
	cFileSpec.append(dbRootPath_);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, dtRootPath_.c_str());
	cFileSpec.append(dtRootPath_);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, DEFAULT_ZDW_DTDATAINPATH);
	cFileSpec.append(DEFAULT_ZDW_DTDATAINPATH);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);
	
	//strcat(cFileSpec, dtFileInputSpec_.c_str());
	cFileSpec.append(dtFileInputSpec_);
	// make sure it exists and is available to us
	FILE *fp = fopen(cFileSpec.c_str(), "r");

	// is there a file there?
	/*
	if (fp == NULL)
	{
		// no - try again, assume a default local fileName
		sprintf(cFileSpec,"%s%s%s",DEFAULT_ROOT_DIR,SEPARATOR,dbFileName_.c_str());
		fp = fopen(cFileSpec,"r");

				dtFileInputSpec_ += dwDataBasePath_;
			dtFileInputSpec_ += "\\";
			dtFileInputSpec_ += parmList_[i+1].c_str();
			dtFileInputSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileInputSpec_ += "\\";			
			dtFileInputSpec_ += DEFAULT_ZDW_DTFILEINPUTNAME;
			dtFileAttrSpec_ += dwDataBasePath_;
			dtFileAttrSpec_ += "\\";
			dtFileAttrSpec_ += parmList_[i+1].c_str();
			dtFileAttrSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileAttrSpec_ += "\\";			
			dtFileAttrSpec_ += DEFAULT_ZDW_DTFILEATTRNAME;
			dtFileDataSpec_ += dwDataBasePath_;
			dtFileDataSpec_ += "\\";
			dtFileDataSpec_ += parmList_[i+1].c_str();
			dtFileDataSpec_ += DEFAULT_ZDW_DTDATAINPATH;
			dtFileDataSpec_ += "\\";			
			dtFileDataSpec_ += DEFAULT_ZDW_DTFILEDATANAME;
	}
	*/

	if (fp == NULL)
	{
		dtFileInputSpec_ = "";
		bReturn = false;
	}
	else
	{
		fclose(fp);
		dtFileInputSpec_ = cFileSpec;
		bReturn = true;
	}
	return bReturn;
}

/****************************************************************
*
* BuildAndCheckOutputLogSpecs
*
* Uses the following globals: 
* dwRootPath, dbRootPath, m_sFileName
*
* Builds the complete path to a database output file
****************************************************************/
bool ZRuntime::buildAndCheckOutputLogSpec()
{
	if ( (dwRootPath_ == "") || (dbRootPath_ == "") 
		|| (dtRootPath_ == "") || (dtFileInputSpec_ == "") ) 
		complete(RET_ARG_ERR);

//	if ( (dwSchemaDir == "") || (dbRootPath_ == "") 
//		|| (dtRootDir_ == "") || (m_sDgFileName == "") ) 
//		complete(RET_ARG_ERR);

	bool bReturn = true;

	//char cFileSpec[PATHMAX];
	std::string cFileSpec;
	//strcpy(cFileSpec, dwRootPath_.c_str());
	cFileSpec.append(dwRootPath_);
	//int nLen = strlen(cFileSpec);
	int nLen = cFileSpec.size();
	if (nLen > 0)
	{
		if ( (cFileSpec[nLen - 1] != '\\') || (cFileSpec[nLen - 1] != '/') )
		{
			//strcat(cFileSpec, SEPARATOR);
			cFileSpec.append(SEPARATOR);

		}
	}

	//strcat(cFileSpec, DEFAULT_ZDW_DWDATABASEPATH);
	cFileSpec.append(DEFAULT_ZDW_DWDATABASEPATH);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, dbRootPath_.c_str());
	cFileSpec.append(dbRootPath_);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, dtRootPath_.c_str());
	cFileSpec.append(dtRootPath_);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);

	//strcat(cFileSpec, DEFAULT_ZDW_DTDATAOUTPATH);
	cFileSpec.append(DEFAULT_ZDW_DTDATAOUTPATH);

	//strcat(cFileSpec, SEPARATOR);
	cFileSpec.append(SEPARATOR);
	
	//strcat(cFileSpec, dtFileInputSpec_.c_str());
	cFileSpec.append(dtFileInputSpec_);
	// make sure it exists and is available
	FILE *fp = fopen(cFileSpec.c_str(), "w");

	/*
	// is there a file there?
	if (fp == NULL)
	{
		// no - try again, assume a default local fileName
		sprintf(cFileSpec,"%s%s%s",DEFAULT_ROOT_DIR,SEPARATOR,m_sDbFileName.c_str());
		fp = fopen(cFileSpec,"r");
	}
	*/

	if (fp == NULL)
	{
		dwOutputLogSpec_ = "";
		bReturn = false;
	}
	else
	{
		fclose(fp);
		dwOutputLogSpec_ = cFileSpec;
		bReturn = true;
	}

	dwOutputLogSpec_= cFileSpec;
	return bReturn;

}
/*****************************************************************************
*
* BuildExecPath
*
* Unix program cannot figure out where it was started from.
*****************************************************************************/
/*
bool ZRuntime::buildExecPath(char *szProgName)
{
	bool bFound = false;
	char cPath[PATHMAX];

	#ifdef WIN32   // (easy)
		if (GetModuleFileName(NULL, cPath, sizeof(cPath)-1) > 0 ) bFound = true;
	#else // Unix - stupid
		cPath[0] = '\0';
		// Try the current directory
		if (getcwd(cPath, sizeof(cPath)-1) == NULL) return false;
		strcat(cPath,SEPARATOR);
		strcat(cPath,szProgName);
		FILE *fp = fopen(cPath,"r");
		if (fp == NULL)
		{
			// not found yet, search through all paths in the system path
      char *szSearchPath = getenv("PATH");
      if (szSearchPath)
      {
				char *szSearchPath1 = new char[strlen(szSearchPath) + 1];
				if (szSearchPath1 == NULL) complete(RET_MEM_ERR);
				strcpy(szSearchPath1, szSearchPath);
				char *szToken = strtok(szSearchPath1, PATH_SEPARATOR);
				while (szToken != NULL)
				{
					if (strcmp(szToken,"."))
					{
						strcpy(cPath,szToken);
						int n = strlen(cPath);
						if ( (n > 0) && (cPath[n-1] == SEPARATOR[0]) )
						{
							cPath[n-1] = '\0';
						}
						strcat(cPath,SEPARATOR);
						strcat(cPath,szProgName);
						fp = fopen(cPath,"r");
						if (fp != NULL)
						{
							bFound = true;
							fclose(fp);
							break;
						}
					}
					szToken = strtok(NULL,PATH_SEPARATOR);
				}
				delete szSearchPath1;
			}
		}
		else
		{
			bFound = true;
			fclose(fp);
		}

	#endif


	if (bFound)
	{
		dwExecPath_ = cPath;
	}

	return bFound;
}
*/

/*****************************************************************************
*
* RevealCode
*
* Note: The functions "IdentifyChar", "MapToAscii" and "MapToAnsi" are 
* located in the CharSetUtils library.
*****************************************************************************/
/*
int ZRuntime::revealCode()
{
	FormatType eType = IdentifyChar(m_cCode);
	char cType[100], cOtherType[100];
	cOtherType[0] = '\0';
	unsigned char cMappedType = 0;
	switch(eType)
	{
		case TypePage1: 
			sprintf(cType,"normal");	break;

		case TypePage2: 
			sprintf(cType,"non-classifiable ANSI/ASCII"); break;

		case TypePage2Mixed: 
			sprintf(cType,"mixed, possible ANSI or ASCII");	break;

		case TypePage2AnsiProbable:
			sprintf(cType,"probable ANSI");	
			sprintf(cOtherType, "ASCII");
			cMappedType = MaptoAscii(m_cCode);
			break;

		case TypePage2AnsiCertain: 
			sprintf(cType,"certain ANSI"); 
			sprintf(cOtherType, "ASCII");
			cMappedType = MaptoAscii(m_cCode);
			break;

		case TypePage2AsciiProbable: 
			sprintf(cType,"probable ASCII"); 
			sprintf(cOtherType, "ANSI");
			cMappedType = MapToAnsi(m_cCode);
			break;

		case TypePage2AsciiCertain: 
			sprintf(cType,"certain ASCII"); 
			sprintf(cOtherType, "ANSI");
			cMappedType = MapToAnsi(m_cCode);
			break;

		case TypeUnknown: 
		default: 
			sprintf(cType,"unknown"); break;
	}
	OutputConsoleMsg(L_REQ,"\n Code %d is classified as \"%s\" (%d)\n",m_cCode,cType,(int)eType);
	if (strlen(cOtherType) > 0)
	{
		OutputConsoleMsg(L_REQ," It maps to an %s code of %d.\n",cOtherType,cMappedType);
	}
	return RET_NO_ERR;
}
*/

/*****************************************************************************
*
* PrepareOutputLog
*
*****************************************************************************/
bool ZRuntime::prepareOutputLog()
{
		if (dwOutputLogSpec_ == "")
		{
			if (pOutputLogFile_ == NULL)
			{
				pOutputLogFile_ = stdout;
			}
			// else, a stream was specified using -o
			dwOutputLogOpen_ = false;
		}
		else
		{
			// try to open the file
			pOutputLogFile_ = fopen(dwOutputLogSpec_.c_str(), "w");
			if (pOutputLogFile_ == NULL)
			{
				outputConsoleMsg(L_ERR,"\nError, file %s could not be opened/created for writing.\n",dwOutputLogSpec_.c_str());
				outputConsoleMsg(L_ERR," restart with the \"-o\" option set.\n\n");
				showHelp(false);
				complete(RET_FILE_ERR);
			}
			dwOutputLogOpen_ = true;
		}
	
		if (iterateDataWarehouse_)
		{
			// fprintf(pOutputLogFile_,"zdwmain version %s - [%s ] - z2iT Technology.\n",PROG_VERSION, GEN_DATE);

			if (zdwClientId_ != "")
			{
				fprintf(pOutputLogFile_,"localhost '%s'",zdwClientId_);
			}

			// get some OS-specific information
			#ifdef WIN32
				ZCtrlOsVersionInfo *pOS = new ZCtrlOsVersionInfo();
				if (pOS != NULL)
				{
					fprintf(pOutputLogFile_," - Platform is %s [%s]\n",pOS->GetPlatformIdStr(), pOS->GetOSVersionStr());

					delete pOS;
				}
			#else // Unix
				struct utsname osinfo;
				if (uname(&osinfo) >= 0)
				{
					//if (m_szComputerName != NULL)
					fprintf(pOutputLogFile_," - %s (%s version %s %s)\n",
						osinfo.sysname, osinfo.machine, osinfo.release, osinfo.version);
				}
			#endif

			time_t t = time(NULL);
			struct tm *tp = localtime (&t);
			fprintf(pOutputLogFile_,"Local time is: %04d-%02d-%02d %02d:%02d:%02d\n",
									tp->tm_year + 1900,
									tp->tm_mon + 1,
									tp->tm_mday,
									tp->tm_hour, 
									tp->tm_min, 
									tp->tm_sec);
			fprintf(pOutputLogFile_,"------------------------------\n");

		}
		else
		{
			fprintf(pOutputLogFile_,"\n------------------------------\nDatabase: %s\n",dwRootPath_.c_str());
		}
		return dwOutputLogOpen_;
}

/*****************************************************************************
*
* ShowHelp
*
* Outputs the usage of this application. 
*
*****************************************************************************/
void ZRuntime::showHelp(bool bShowHidden)
{
		printf("\nUsage:\n");
		printf("-?      [option]     Display this usage list.\n");
		printf("-a      [n]          Analysis level.\n");
		printf("-db     [database]   Database directory within warehouse.\n");
		printf("-dt     [table]      Database table within database directory.\n");
		printf("-dw     [root]       Path to the data warehouse:(Z2IDW)\n");
		printf("-dbtype [mysql]      Database server type.\n");
		printf("-f      [filename]   Specific file name to parse.\n");
		printf("-g      [filename]   Generate database table schema.\n");
		printf("-l      [option]     List contents of warehouse only.\n");
		printf("-o      [filename]   Output file to use for reporting.\n");
		printf("-port   [nnnn]       Port.\n");
		printf("-pwd    [password]   Database password.\n");
		printf("-s      [option]     Output to stream [stderr | stdout].\n");
		printf("-server [id]         Database Server ID.\n");
		printf("-t      [directory]  Temporary file location.\n");
		printf("-user   [username]   Username to connect to database server.\n");
		
		if (bShowHidden)
		{
		printf("Extended options:\n");
		printf("-c      [n]          Display character code.\n");
		printf("-e      [n]          Number of offset lines to display upon parse error.\n");
		printf("-i      [n]          Iteration count (for testing iteration only!.\n");
		printf("-u      [option]     ANSI-ASCII file identifcation. option = params|file.\n");
		printf("-x      [option]     Encryption options. Set option = off|encode|decode|check.\n");
		printf("-z      [option]     Spawn from another process - internal usage.\n");
		}

		// give an example
		printf("\nExample:\n");
		#ifdef WIN32
			printf("  %s -dw c:\\dw -db office -dt contact \n",PROG_NAME);
		#else	// Unix
			printf("  %s -dw /dw -db office -dt contact \n",PROG_NAME);
		#endif
		printf("\n");
}

//******************************************************************************
//	Extract "name" from "[drive:\path\]name[.ext]".
//	Allocates memory which must be freed by caller.
//******************************************************************************
/*
char* ZRuntime::getBaseNameFromSpec(char* filename)
{
	char*					retval;
	unsigned int	pos;
	unsigned int	length;

	if (filename == (char*)NULL)
  {
  	return (char*)NULL;
  }

	pos = strlen(filename);
	length = 0;
	while ((pos != 0) && (filename[pos - 1] != '\\') &&
				 (filename[pos - 1] != ':') &&
				 (filename[pos - 1] != '.'))
	{
		pos--;
		length++;
	}
	if (pos != 0)
	{
		switch (filename[pos - 1])
		{
			case '\\': case ':':
				retval = (char*)malloc((length + 1) * sizeof (char));
				strncpy(retval, filename + pos, length);
				retval[length] = '\0';
			break;
			case '.':
				length = 0;
				while ((pos != 0) &&
							 (filename[pos - 1] != '\\') &&
							 (filename[pos - 1] != ':'))
				{
					pos--;
					length++;
				}
				length--;
				retval = (char*)malloc((length + 1) * sizeof(char));
				strncpy(retval, filename + pos, length);
				retval[length] = '\0';
			break;
		}
	}
	else
	{
		retval = (char*)malloc((length + 1) * sizeof(char));
		strncpy(retval, filename + pos, length);
		retval[length] = '\0';
	}
	return (retval);
}
*/







 
