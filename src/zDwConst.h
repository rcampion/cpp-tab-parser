/**************************************************************************
 *
 * zlexconst.h
 * author: Richard Campion
 *  
 **************************************************************************/

#ifndef __ZDWCONSTANTS_H__
#define __ZDWCONSTANTS_H__

// Unix does not have the "stricmp" function, must remap it to strcasecmp
#if defined(SOLARIS) || defined(LINUX) 
	#ifndef _UNIX_STRICMP_
		#define _UNIX_STRICMP_
		#define stricmp(a,b) strcasecmp((a),(b))
	#endif
#endif

#ifdef WIN32
	#define SEPARATOR "\\"
	#define PATHMAX	_MAX_PATH
  #define PATH_SEPARATOR ";"
#else
	#define SEPARATOR	"/"
	#define PATHMAX 1024
	#define PATH_SEPARATOR ":"
#endif

/***************************************************************************
 *
 * LexFileSection enumeration
 *
 **************************************************************************/
typedef enum LEXFILESECTION {
	LEXFILESECTION_DESC_MIN = 0,
	LEXFILESECTION_UNDEFINED,
	LEXFILESECTION_ROOT,
	LEXFILESECTION_HEADING,
	LEXFILESECTION_DATA,
	LEXFILESECTION_DESC_MAX
} LexFileSection;

/***************************************************************************
 *
 * LexTokenType enumeration
 *
 **************************************************************************/
typedef enum LEXTOKEN_TYPE {
	LEXTOKEN_TYPE_MIN = 0,
	LEXTOKEN_TYPE_UNDEFINED,
	LEXTOKEN_BLANKLINE,
	LEXTOKEN_COMMAND,
	LEXTOKEN_COMMENT,
	LEXTOKEN_CONTINUE,
	LEXTOKEN_DATA,
	LEXTOKEN_DATACELL,
	LEXTOKEN_ESCAPE,
	LEXTOKEN_FIELDNAME,
	LEXTOKEN_FIELDVALUE,
	LEXTOKEN_FORMAT,
	LEXTOKEN_HEADERCELL,
	LEXTOKEN_IDENTIFIER,
	LEXTOKEN_NEWLINE,
	LEXTOKEN_OPERATOR,
	LEXTOKEN_PATH,
	LEXTOKEN_PROMPT,
	LEXTOKEN_ROOT,
	LEXTOKEN_SECTION,
	LEXTOKEN_STRINGLINE,
	LEXTOKEN_STRINGVALUE,
	LEXTOKEN_SWITCH,
	LEXTOKEN_UNDEFINED,
	LEXTOKEN_VALUE,
	LEXTOKEN_WHITESPACE,
	LEXTOKEN_TYPE_MAX

} LexTokenType;

// <column name="Title" required="false" type="VARCHAR" size="255" description="Title" />
/***************************************************************************
 *
 * LexDataType enumeration
 *
 **************************************************************************/
typedef enum LEXTOKEN_DATATYPE {
	LEXTOKEN_DATATYPE_MIN = 0,
	LEXTOKEN_DATATYPE_UNDEFINED,
	LEXTOKEN_DATATYPE_TINY,
	LEXTOKEN_DATATYPE_SHORT,
	LEXTOKEN_DATATYPE_LONG,
	LEXTOKEN_DATATYPE_INT24,
	LEXTOKEN_DATATYPE_INTEGER,
	LEXTOKEN_DATATYPE_LONGLONG,
	LEXTOKEN_DATATYPE_DECIMAL,
	LEXTOKEN_DATATYPE_FLOAT,
	LEXTOKEN_DATATYPE_DOUBLE,
	LEXTOKEN_DATATYPE_TIMESTAMP,
	LEXTOKEN_DATATYPE_DATE,
	LEXTOKEN_DATATYPE_TIME,
	LEXTOKEN_DATATYPE_DATETIME,
	LEXTOKEN_DATATYPE_YEAR,
	LEXTOKEN_DATATYPE_STRING,
	LEXTOKEN_DATATYPE_VAR_STRING,
	LEXTOKEN_DATATYPE_BLOB,
	LEXTOKEN_DATATYPE_SET,
	LEXTOKEN_DATATYPE_ENUM,
	LEXTOKEN_DATATYPE_NULL,
	LEXTOKEN_DATATYPE_CHAR,
	LEXTOKEN_DATATYPE_TEXT,
	LEXTOKEN_DATATYPE_VARCHAR,
	LEXTOKEN_DATATYPE_DEFAULT,
	LEXTOKEN_DATATYPE_MAX
} LexDataType;

/***************************************************************************
 *
 * LexScanStatus enumeration
 *
 **************************************************************************/
typedef enum LEXSCAN_STATUS {
	LEXSCAN_UNKNOWN = 0, // no scan / no parse	/ no value								
	LEXSCAN_ASSIGNED,    // scanned / parsed / value assigned - no error                 
	LEXSCAN_SEEN,        // scanned/parsed - no value  
	LEXSCAN_ERROR,       // severe error                                               
	LEXSCAN_PENDING,     // token scanned                              
	LEXSCAN_WARNING,     // non-severe error																						
	LEXSCAN_DELETED,     // remove from memory                                        
	LEXSCAN_COMPLETE     // token values are set														
} LexScanStatus;


/***************************************************************************
 *
 * StatusCode enumeration
 *
 **************************************************************************/
enum StatusCode
{
	E_NoError = 0,
	E_BadParameter,
	E_BufferEmpty,
	E_BufferError,
	E_BufferOverflow,
	E_DirectorySearchError,
	E_EntrySearchError,
	E_Exception,
	E_FileNameError,
	E_FileNotFound,
	E_FileError,
	E_ListUninitialized,
	E_ListInitialized,
	E_NoStartDirectory,
	E_NoMoreEntries,
	E_MemError,
	E_OSSpecificError,
	E_LexerError,
	E_ParserError,
  E_ValueNotFound
};


enum IteratorCode
{
	NoError = 0,
	NoMoreEntries,
	ListUninitialized,
	ListInitialized,
	BadParameter,
	OSSpecificError,					// call GetLastError() for Win32, or use "errno" variable for other platforms
	BufferOverflow,
	NoStartDirectory,
	DirectorySearchError,
	MemError,
};

// error/return codes
#define RET_NO_ERR									0			// OK
// -> application specific
#define RET_PROG_BASE								2000
#define RET_MEM_ERR									RET_PROG_BASE + 1			// memory problem
#define RET_ARG_ERR									RET_PROG_BASE + 2			// problem with arguments
#define RET_INT_ERR									RET_PROG_BASE + 3			// internal error in this application
#define RET_FILE_ERR								RET_PROG_BASE + 4			// file handling error
#define RET_BUF_OVERFLOW						RET_PROG_BASE + 5			// buffer overflow
#define RET_UNINIT_ERR							RET_PROG_BASE + 6 		// unitialized data error
#define RET_OS_ERR									RET_PROG_BASE + 7			// OS specific error
#define RET_NO_DIR_ERR							RET_PROG_BASE + 8 		// no directory available
#define RET_DIR_SEARCH_ERR					RET_PROG_BASE + 9 		// cannot search a directory
#define RET_FILE_OVERRUN						RET_PROG_BASE + 10		// went too far in file processing
#define RET_EMPTY_LIST							RET_PROG_BASE + 11		// empty list
#define RET_NOMORE_ENTRIES					RET_PROG_BASE + 12		// no more entries in a list
#define RET_PRINT_ERR  					    RET_PROG_BASE + 13		// print routine error
#define RET_EXCEPTION_ERR						RET_PROG_BASE + 14		// exception error in this application
#define RET_PROG_BASE_MAX						RET_PROG_BASE + 29

// -> parser errors / status
#define RET_PARSER_BASE							RET_PROG_BASE + 500
#define RET_PARSER_INT_ERR					RET_PARSER_BASE + -1	// internal error in the parser
#define RET_PARSE_ERR								RET_PARSER_BASE + 0		// file could not be parsed
#define RET_PARSE_HEADER_OK				  RET_PARSER_BASE + 1		// header line was parsed
#define RET_PARSER_BASE_MAX					RET_PARSER_BASE + 200	// errors in between are parser specific

// parse status levels
#define PARSE_LEVEL_NOT_PARSED			0
#define PARSE_LEVEL_PARSE_ERR				1
#define PARSE_LEVEL_PARSE_OK				2

#define LEXANALYSIS_BASE						    RET_PROG_BASE + 90
#define LEXANALYSIS_PARSE_INT_ERR       LEXANALYSIS_BASE +1
#define LEXANALYSIS_PARSE_ERR           LEXANALYSIS_BASE +2
#define LEXANALYSIS_PARAM_ERR           LEXANALYSIS_BASE +3
#define LEXANALYSIS_INFO                LEXANALYSIS_BASE +4
#define LEXANALYSIS_DEPOTDIR_ERR        LEXANALYSIS_BASE +5
#define LEXANALYSIS_CLASS_ERR           LEXANALYSIS_BASE +6
#define LEXANALYSIS_DEPEND_ERR          LEXANALYSIS_BASE +7
#define LEXANALYSIS_SUBST_ERR           LEXANALYSIS_BASE +8
#define LEXANALYSIS_DEPOT_DIR_NONE      LEXANALYSIS_BASE +9
#define LEXANALYSIS_DEPOT_DIR_SUBDIR    LEXANALYSIS_BASE +10
#define LEXANALYSIS_DEPOT_DIR_BADSTRING LEXANALYSIS_BASE +11
#define LEXANALYSIS_BASE_MAX				    LEXANALYSIS_BASE +20

// analysis levels
#define LEXANALYSIS_LEVEL_0					0		// lex only
#define LEXANALYSIS_LEVEL_1					1		// lex + parse 
#define LEXANALYSIS_LEVEL_2					2		// lex + parse + syntax 
#define LEXANALYSIS_LEVEL_3					3		// lex + parse + syntax + detail
#define LEXANALYSIS_LEVEL_4					4		// lex + parse + syntax + detail + debug
#define MIN_LEXANALYSIS_LEVEL				LEXANALYSIS_LEVEL_0
#define MAX_LEXANALYSIS_LEVEL				LEXANALYSIS_LEVEL_4
#define DEFAULT_LEXANALYSIS_LEVEL		LEXANALYSIS_LEVEL_2

// syntax errors 
#define RET_SYNTAX_BASE							RET_PROG_BASE + 30
#define RET_SYNTAX_ERR							RET_SYNTAX_BASE + 0		// general or unknown syntax error
#define RET_SYNTAX_WARN							RET_SYNTAX_BASE + 1		// general or unknown syntax warning
#define RET_FILESYSTEM_SUBDIR_ERR		RET_SYNTAX_BASE + 3		// the directory entry has a sub directory
#define RET_FILESYSTEM_STRING_ERR	  RET_SYNTAX_BASE + 4		// the entry has a bad string
#define RET_DUPLICATE_ERR					  RET_SYNTAX_BASE + 5		// the entry appears more than once
#define RET_SYNTAX_BASE_MAX					RET_SYNTAX_BASE	+ 19

// 'quiet' mode constants
#define NOT_QUIET			3		// max output mode
#define MAX_QUIET			0		// silent mode
#define L_STATUS			3		// status message
#define L_WARN				2		// warning
#define L_ERR					1		// error 
#define L_REQ					0		// requested output
#define QUIET_DEFAULT	L_ERR

// default filesystem constants
#define	DEFAULT_ZDW_DBCONFIGNAME        "config.zdb"  // database configuration/preference settings
#define	DEFAULT_ZDW_DBROOTPATH          "db"          // default database root directory
#define	DEFAULT_ZDW_DBSEVERNAME         "zdbserver"   // database server name
#define	DEFAULT_ZDW_DBSERVERPASSWORD    "password"    // database user password to database server
#define	DEFAULT_ZDW_DBSERVERPORT        "3365"        // database server port as string
#define	DEFAULT_ZDW_DBSERVERUSERNAME    "admin"       // database user id to database server
#define	DEFAULT_ZDW_DTCONFIGNAME        "config.zdt"  // datatable configuration/preference settings
#define	DEFAULT_ZDW_DTDATAINPATH        "in"          // relative path to dt - input file directore
#define	DEFAULT_ZDW_DTDATAOUTPATH       "out"         // relative path to dt - output file directory
#define	DEFAULT_ZDW_DTFILEATTRNAME      "input.zta"   // datatable file spec for datatable attributes
#define	DEFAULT_ZDW_DTFILEDATANAME      "input.ztd"   // datatable file spec for datatable data load
#define	DEFAULT_ZDW_DTFILEINPUTNAME     "input.txt"   // datatable filespec for the datatable input file name = dwroot + dir + db + name
#define	DEFAULT_ZDW_DTSCHEMANAME        "schema.xml"  // datatable schema file name to be generated
#define	DEFAULT_ZDW_DWCONFIGNAME        "config.zdw"  // datawarehouse configuration/preference settings
#define	DEFAULT_ZDW_DWDATABASEPATH      "db"          // default database root directory
#define	DEFAULT_ZDW_DWDRIVEPATH         "C:"          // w32 drive letter
#define	DEFAULT_ZDW_DWEXECPATH          "bin"         // relative path to system executable directory
#define	DEFAULT_ZDW_DWLOGNAME           "zdwsys.log"  // file spec for the system log
#define	DEFAULT_ZDW_DWLOGPATH           "log"         // relative path to dw - system log directory
#define	DEFAULT_ZDW_DWROOTPATH          "dw"          // relative path to dw - system root directory
#define	DEFAULT_ZDW_DWSCHEMAPATH        "schema"      // relative path to dw - database schema directory
#define	DEFAULT_ZDW_DWSTATSFILESPEC     "zdwstat.log" // relative spec for statisics file
#define	DEFAULT_ZDW_DWTMPPATH           "tmp"         // relative path for temporary files
#define	DEFAULT_ZDW_ZDWSERVER           "zdwserver"   // the host name of the zdwRemoteServer

// other constants
#define DEFAULT_LEX_PLUSMINUS_OFFSET	5
#define DEFAULT_LEX_BUFSIZE 128
#define	DEFAULT_ITEM_PREFIX             "FLD_"        // the header field element name prefix

struct ANSIASCII_ANALYSIS_STATS
{
	int nSum;
	int nErr;
	int nUnknown;
	int nNormal;

	int nAnsi_Sum;
	int nAnsi_100;
	int nAnsi_High;
	int nAnsi_MediumHigh;
	int nAnsi_Medium;
	int nAnsi_MediumLow;
	int nAnsi_Low;

	int nAscii_Sum;
	int nAscii_100;
	int nAscii_High;
	int nAscii_MediumHigh;
	int nAscii_Medium;
	int nAscii_MediumLow;
	int nAscii_Low;
};
#endif // __LEXCONSTANTS_H__
