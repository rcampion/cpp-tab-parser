/**************************************************************************
 *
 * zLexLexer.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexLexer.h
*/

#ifndef ___ZLEXLEXER_H___
#define ___ZLEXLEXER_H___

#include "zDwConst.h"
#include "zLexBase.h"
#include "zLexToken.h"
#include "zLexError.h"
#include "zLexFile.h"
#include "zLexBuffer.h"

#include <string>

/****************************************************************************
 * Flex function and variable renaming for multiple Lexers
 * To understand what's going on here:
 * read page 162 of O'Reilly's "lex & yacc"
 * also see implementation of generated scanner(s)
 ****************************************************************************/
//the zFile_ prefix is defined in the custom build of the flex/bison specs
/*
#define      zFile_wrap() 1
extern int   zFile_lex();
extern char* zFile_text;
extern int   zFile_leng;
extern void  zFile_error(char * msg);
extern int   zFile_parse();
extern int   zFile_debug;
*/
//the zHdr_ prefix is defined in the custom build of the flex/bison specs
//#define      zHdr_wrap() (/*CONSTCOND*/1)
extern int   zHdr_lex();
extern int   zHdr_parse();
extern char* zHdr_text;
extern int   zHdr_leng;
extern void  zHdr_error(char * msg);
extern int   zHdr_debug;

//the zData_ prefix is defined in the custom build of the flex/bison specs
//#define      zData_wrap() (/*CONSTCOND*/1)
extern int   zData_lex();
extern int   zData_parse();
extern char* zData_text;
extern int   zData_leng;
extern void  zData_error(char * msg);
extern int   zData_debug;

// gobal counters maintained by lexer/parser
extern int   gl_nLexLineCount;            // count number of lines in file 
extern int 	 gl_nLexRowCount;             // count number of lines in section 
extern int   gl_nLexColumnCount;          // count number of columns on this line
extern int   gl_nLexFieldCount;           // count number of fields on this line
extern int   gl_nLexFileCharOffset;       // count number of chars scanned in this file 
extern int   gl_nLexLineCharOffset;       // count number of chars on this line 

// global flags maintained by the context/runtime
extern bool  gl_nLexFirstTime;
extern int   gl_nQuietLevel;

// global pointers available to callers
extern ZLexToken *gl_pLexCurrentToken;
extern ZLexToken *gl_pLexTokenList;
extern ZLexToken *gl_pLexErrorList;


/****************************************************************************
*
* ZLexLexer
*
*****************************************************************************/
/*! This lexer is intended to be constructed with a buffer object which contains
 a char buffer of text to be parsed; 
PreCondition:
	The text buffer is assigned to Flex, see YY_INPUT. 
Processing:
  doLex...() calls the Flex/Bison lex/parse routine yylex()/yyparse.
	Bison's yyparse() calls the Flex yylex() for input tokens.
  Flex scans for tokens according to the flex lexer specfication..see .l file
	Bison checks for token arrangements verifies the token order, see .y file
  Bison appends all tokens into a data structure
PostCondition
	A global linked list of tokens (ZLexToken's) is available to caller via:
	getTokenList();
*/
class ZLexLexer {
public:
	ZLexLexer();                                //default constructor
	ZLexLexer(ZLexFile *zfile, ZLexFileBuffer *zbuf); 
	~ZLexLexer(); //destructor
	StatusCode		doLexData();
	LexError			doLexDataProc();
	StatusCode		doLexHeader();
	LexError			doLexHeaderProc();
	StatusCode    doParseData();
	StatusCode    doParseHeader();
	StatusCode    getLexerStatus() { return m_eLexerStatus; };
	LexError      getLexerError()  { return m_eLexerError; };
	ZLexToken*    getTokenList();
	StatusCode    release();
	void	        showTokens();

private:

	//std::string		m_sTextBuffer;
	ZLexFile*		m_pLexFile;	
	ZLexFileBuffer*		m_pLexCharBuffer;

	ZLexToken*    m_pLexTokenList;
	ZLexToken*    m_pLexErrorList;

	StatusCode fillLexBuffer(const char* buf);

	FILE* fileOpen(const char* name);
	FILE* fileOpenExt(const char* name);

	StatusCode		init();	
	void initTokenList();
	void initErrorList();

	void destroyTokenList();
	void destroyToken(ZLexToken* tok);
	
	void destroyErrorList();
	void destroyError(ZLexToken* tok);
	
	//StatusCode		prepareBufferFromText();
	StatusCode		prepareBufferFromBinary(ZLexFileBuffer *zbuf);

private:
	StatusCode    m_eLexerStatus;
	LexError      m_eLexerError;
};

#endif // ___ZLEXLEXER_H___
