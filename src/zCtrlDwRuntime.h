/**************************************************************************
 *
 * zCtrlDwRuntime.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 **************************************************************************/
/*! \file zCtrlDwRuntime.h
*/
#ifndef _ZCTRLDWRUNTIME_H_
#define _ZCTRLDWRUNTIME_H_

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

class ZRuntime  
{
public:
	ZRuntime();                                  // default constructor
	ZRuntime(int n, char * argv[]);              // construct using passed args
	ZRuntime(const ZRuntime& rhs);               // copy constructor
	ZRuntime& operator= (const ZRuntime& rhs);   // assignment operator
	~ZRuntime();				                 // destructor

private:
	//debug begin
	static int objectCount;
	void print(const std::string& msg);
	//debug end

public:
	bool buildAndCheckInputFileSpec();
	bool buildAndCheckOutputLogSpec();
	bool buildExecPath(char *szProgName);
	void complete(int nRetCode);
	void init();
	int  outputConsoleMsg(int nLevel, char *msg, ...);
	int  outputLogMsg(int nLevel, char *msg, ...);
	bool parseCommandLineOptions(int argc, char *argv[]);
	bool prepareOutputLog();
	void setDefaultOptions();
	void showHelp(bool bShowHidden);
	std::string toString();

	void setListOnly(bool bFlag) {dwListOnly_ = bFlag;}
	void setOutputLogOpen(bool bFlag) {dwOutputLogOpen_ = bFlag;}
	void setOutputLogName(char *szFileName) {dwOutputLogSpec_ = szFileName;}
	void setDbRootPath(std::string sDbSpec) {dbRootPath_ = sDbSpec;}

	// run time switches
	// public getters
	int	 getAnalysisLevel();           //
	bool getCheckEncoded();            //
	bool getCheckFileType();           //
	bool getCheckLexParamTypes();      //
	bool getConvertFileToDosWindows(); //
	bool getDebugMem();                //
	bool getDecode();                  //
	bool getDwListOnly();              //
	bool getEncode();                  //
	bool getEncodeDecodeOptionSet();   //
	bool getError();                   //
	bool getGenOutput();               //
	int  getIterateCount();            //
	bool getIterateDataWarehouse();    //
	bool getNoEncode();                //
	bool getOutputLogOpen();           //
	int  getParseErrPlusMinusOffset(); //
	bool getPrintResults();            //
	bool getPrintTokens();             //
	bool getQuiet();                   //
	int  getQuietLevel();              //
	bool getShowHiddenOptions();       //
	bool getShowProgress();            //
	bool getSpawnedMode();             //

	// getters
	//database specific
	std::string getDbConfigName();			// database: configuration/preference settings	
	std::string getDbRootPath();			// database: full path to the root directory
	std::string getDbServerName();			// database: server name	
	std::string getDbServerPassword();		// database: user password to database server
	std::string getDbServerPort();			// database: server port as string	
	std::string getDbServerUsername();		// database: user id to database server

	//datatable specific
	std::string getDtConfigName();			// datatable: configuration/preference settings	
	std::string getDtDataInPath();			// datawarehouse: full path to data input directory	
	std::string getDtDataOutPath();			// datawarehouse: full path to data output directory	
	std::string getDtFileAttribSpec();		// datatable: filespec for datatable attributes
	std::string getDtFileDataSpec();		// datatable: filespec for datatable data load	
	std::string getDtFileInputSpec();		// datatable: filespec for the input file name = dwroot + dir + db + name	
	std::string getDtFileSchemaName();		// datatable: schema file name to be generated

	//datawarehouse specific
	std::string getDwConfigName();			// datawarehouse: configuration/preference settings	
	std::string getDwDataBasePath();		// datawarehouse: default database location	
	std::string getDwDriveName();			// datawarehouse: installation drive letter
	std::string getDwExecPath();			// datawarehouse: full path to system executable directory
	std::string getDwLogName();				// datawarehouse: file spec for the system log
	std::string getDwLogPath();				// datawarehouse: full path to system log directory
	std::string getDwRootPath();			// datawarehouse: full path to the root directory
	std::string getDwSchemaPath();			// datawarehouse: full path to the schema directory	
	std::string getDwStatsFileSpec();		// datawarehouse: file spec for statisics file	
	std::string getDwTmpPath();				// datawarehouse: path for temporary files
	std::string getZdwServerId();			// datawarehouse: the host name of the zdwRemoteServer
	std::string getZdwClientId();			// datawarehouse: computer id for logging
	
	char* getBaseNameFromSpec(char* filename);
	FILE* getOutputLogPtr() {return pOutputLogFile_;}

private:
	// runtime switches
	int	 analysisLevel_;               // detail level of analysis
	bool checkEncoded_;                // tbd
	bool checkFileType_;               // tbd
	bool convertFileToWin_;            //
	bool debugMem_;                    //
	bool decode_;                      //
	bool dwListOnly_;                  //
	bool encode_;                      //
	bool encodeDecodeOptionSet_;       //
	bool errors_;                      //
	bool genOutput_;                   //
	int  iterateCount_;                //
	bool iterateDataWarehouse_;        //
	bool noEncode_;                    //
	int  parseErrPlusMinusOffset_;     //
	bool printResults_;                //
	bool printTokens_;                 //
	bool quiet_;                       //
	int  quietLevel_;                  //
	bool showHiddenOptions_;           //
	bool showProgress_;                //
	bool spawnedMode_;                 //
	bool dwOutputLogOpen_;             //
	std::string dwOutputLogName_;      //
	std::string dwOutputLogSpec_;      //

	FILE* pOutputLogFile_;

	// data members
	//database specific
	std::string dbConfigName_;         // database: configuration/preference settings	
	std::string dbRootPath_;           // database: full path to the root directory
	std::string dbServerName_;         // database: server name	
	std::string dbServerPassword_;     // database: user password to database server	
	std::string dbServerPort_;         // database: server port as string	
	std::string dbServerUsername_;     // database: user id to database server	

	//datatable specific	
	std::string dtConfigName_;         // datatable: configuration/preference settings	
	std::string dtDataInPath_;         // datatable: full path to data input directory	
	std::string dtDataOutPath_;        // datatable: full path to data output directory	
	std::string dtFileAttrSpec_;       // datatable: filespec for datatable attributes	
	std::string dtFileDataSpec_;       // datatable: filespec for datatable data load	
	std::string dtFileInputSpec_;      // datatable: filespec for the input file name = dwroot + dir + db + name	
	std::string dtFileSchemaName_;     // datatable: schema file name to be generated
	std::string dtRootPath_;           // datatable: full path to the root directory
	
	//datawarehouse specific
	std::string dwConfigName_;         // datawarehouse: configuration/preference settings	
	std::string dwDataBasePath_;       // datawarehouse: default data location	
	std::string dwDriveName_;          // datawarehouse: installation drive letter	
	std::string dwExecPath_;           // datawarehouse: full path to system executable directory	
	std::string dwLogName_;            // datawarehouse: file spec for the system log	
	std::string dwLogPath_;            // datawarehouse: full path to system log directory	
	std::string dwMasterDbFileSpec_;   // datawarehouse: file spec for master database list	file
	std::string dwMasterDtFileSpec_;   // datawarehouse: file spec for master table list list file
	std::string dwRootPath_;           // datawarehouse: full path to the root directory	
	std::string dwSchemaPath_;         // datawarehouse: full path to the schema directory	
	std::string dwStatsFileSpec_;      // datawarehouse: file spec for statisics file	
	std::string dwTargetDb_;           // datawarehouse: database type	
	std::string dwTmpPath_;            // datawarehouse: path for temporary files	
	std::string zdwServerId_;          // datawarehouse: the host name of the zdwRemoteServer	
	std::string zdwClientId_;          // datawarehouse: computer id for logging	

	//switches
	int	  lexErrPlusMinusOffset_;

};

extern ZRuntime* gl_pContext;

#endif
 
