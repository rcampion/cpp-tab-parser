/**************************************************************************
 *
 * zLexLexer.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexLexer.cpp
*/
#include <iostream>
using namespace std;

#include "zLexError.h"
#include "zLexLexer.h"
#include "zLexUtil.h"
#include "zLexToken.h"
#include "zLexFile.h"
#include "zLexBuffer.h"
//#include "..\syntax\tab\BisonTabHdrLineParser.cpp.h"
#include "BisonTabHdrLineParser.hpp"

//#include <iostream.h>

#include <string>
#include <sstream>
#include <math.h>  

//global lexer/parser specific variables
bool  gl_nLexFirstTime = true;        // one time set up flag for parse
int   gl_nLexLineCount = 0;           // count number of lines in file 
int 	gl_nLexRowCount = 0;            // count number of lines in section 
int   gl_nLexColumnCount = 0;         // count number of columns on this line
int   gl_nLexFieldCount = 0;          // count number of fields on this line
int 	gl_nLexFileCharOffset = 0;      // count number of chars scanned in this file 
int		gl_nLexLineCharOffset = 0;      // count number of chars on this line 
int 	gl_nLexHeaderRowCount = 0;      // count number of lines in header section  
int 	gl_nLexHeaderFieldCount = 0;    // count number of fields in header row 
int 	gl_nLexHeaderDelimiterCount = 0;// count number of delimiters in header row  
int 	gl_nLexDataRowCount = 0;        // count number of lines in data section  
int 	gl_nLexDataFieldCount = 0;      // count number of fields in data row 
int 	gl_nLexDataDelimiterCount = 0;  // count number of delimiters in data row 

LexFileSection gl_nLexCurrentSection = LEXFILESECTION_ROOT;
ZLexToken* gl_pLexCurrentToken = 0;

//the list of tokens accumulated during a parse
ZLexToken* gl_pLexTokenList = 0;

//the list of tokens with errors during a parse
ZLexToken* gl_pLexErrorList = 0;

int gl_nQuietLevel = MAX_QUIET;

/***************************************************************************
 *
 * default contructor
 *
 ***************************************************************************/
ZLexLexer::ZLexLexer() {
	init();
}
/***************************************************************************
 *
 * contruct using char* (buffer)
 *
 ***************************************************************************/
ZLexLexer::ZLexLexer(ZLexFile *zfile, ZLexFileBuffer *zbuf) {
	// also 
	init();

	//do we have anything to parse?
	if(zbuf->getSize() > 0) {
		m_eLexerStatus = zbuf->prepareBufferFromBinary();
	}
	else
	{
		m_eLexerStatus = E_BufferEmpty;
		m_eLexerError = E_LEX_BUFFER_EMPTY;
	}
}
/***************************************************************************
 *
 * contruct using STL text string (buffer)
 *
 ***************************************************************************/
/*
ZLexLexer::ZLexLexer(std::string &s) {
	//todo: buffer is empty! whose responsibility is it to fill it?
	// also 
	init();

	//do we have anything to parse?
	if(s.length() > 0) {
		m_sTextBuffer = s;
		m_eLexerStatus = prepareBufferFromText();
	}
	else
	{
		m_eLexerStatus = E_BufferEmpty;
		m_eLexerError = E_LEX_BUFFER_EMPTY;
	}
}
*/
/***************************************************************************
 *
 * contruct using char* (buffer)
 *
 ***************************************************************************/
/*
ZLexLexer::ZLexLexer(ZLexFileBuffer *zbuf) {
	//todo: buffer is empty! whose responsibility is it to fill it?
	// also 
	init();

	//do we have anything to parse?
	if(zbuf->getSize() > 0) {
		m_sTextBuffer = ""; //we are using a char* based buffer, nullify this
		m_pLexCharBuffer = NULL; //we are using a char* based buffer, initialize
		m_eLexerStatus = prepareBufferFromBinary(zbuf);
	}
	else
	{
		m_eLexerStatus = E_BufferEmpty;
		m_eLexerError = E_LEX_BUFFER_EMPTY;
	}
}
*/
/***************************************************************************
 *
 * contruct using vector (buffer)
 *
 ***************************************************************************/
/*
ZLexLexer::ZLexLexer(std::vector v) {

	m_sTextBuffer = s;

	m_pLexCharBuffer.buf = NULL;
	m_eLexerStatus = E_NoError;
	m_eLexerError = E_LEX_BUFFER_EMPTY;
}
*/

/***************************************************************************
 *
 * destructor
 *
 ***************************************************************************/
ZLexLexer::~ZLexLexer() {
	release();
}

/***************************************************************************
 *
 * init
 *
 ***************************************************************************/
StatusCode ZLexLexer::init() {
	//initialize the error messages
	if(!gl_nErrInitialized)
		InitLexErrMsg();
	
	m_pLexCharBuffer = NULL;
	m_eLexerStatus = E_NoError;
	m_eLexerError = E_LEX_OK;
	m_eLexerStatus = E_NoError;
	m_eLexerError = E_LEX_OK;

	gl_nLexFirstTime = true;	

	m_eLexerStatus = E_NoError;
	m_eLexerError = E_LEX_OK;
 
	//reset accumulators
  gl_nLexLineCount=0;       // reset number of lines in file 
	gl_nLexRowCount=0;        // reset number of lines in section 
  gl_nLexColumnCount=0;     // reset number of columns on this line
  gl_nLexFieldCount=0;      // reset number of fields on this line
  gl_nLexFileCharOffset=0;  // reset number of chars scanned in this file 
  gl_nLexLineCharOffset=0;  // reset number of chars on this line 

	//winging it
	gl_pLexCurrentToken = NULL;
	gl_pLexErrorList = NULL;

	//not sure if this is necessary - will test
	//gl_nLexCurrentSection = LEXFILESECTION_ROOT;

	// create a default root token
 	gl_pLexTokenList = new ZLexToken( 
																 "<root\\>",                         // text   		 
																 gl_nLexLineCount,                   // line
                                 gl_nLexRowCount,                    // row
                                 gl_nLexColumnCount,                 // column
                                 gl_nLexLineCharOffset,              // char offset from beginning of line								 
                                 gl_nLexFileCharOffset,              // char offset from beginning of file							 
                                 LEXTOKEN_SECTION,                   // id, value, field
																 LEXTOKEN_DATATYPE_STRING,           // datatype representation                     
																 LEXSCAN_COMPLETE,                   // scan status
																 NULL);							                 // parent token;								

	gl_pLexCurrentToken = gl_pLexTokenList;
	
	//gl_bQuiet = gl_pContext->getQuiet();
	//gl_nQuietLevel = gl_pContext->getQuietLevel();
	//gl_bShowProgress = gl_pContext->getShowProgress();
	//gl_bPrintResults = gl_pContext->getPrintResults();
	//gl_bPrintTokens = gl_pContext->getPrintTokens();
	//gl_bDebugMem = gl_pContext->getDebugMem();

	//m_pLexCharBuffer = new ZLexFileBuffer();	

	return E_NoError;
}

/***************************************************************************
 *
 * prepareBufferFromText
 *
 ***************************************************************************/
/*
StatusCode ZLexLexer::prepareBufferFromText() {

	StatusCode sc = E_NoError;
	//reset parser state, variables
	//init();

	//fill the parser buffer
	//int byteCounter = 0;
	sc = fillLexBuffer(m_sTextBuffer.c_str());
	//if(sc!=E_NoError)
	//	return sc;

	// assign the buffer to Flex
	gl_pLexInputBuf = m_pLexCharBuffer->getBuffer();
	gl_pLexInputPtr = m_pLexCharBuffer->getBuffer();
	//gl_pLexInputLim = m_pLexCharBuffer.buf + (m_pLexCharBuffer.size - 1);
	gl_pLexInputLim = m_pLexCharBuffer->getBuffer() + (m_pLexCharBuffer->getSize());
	//gl_nLexInputLim = (m_pLexCharBuffer->getSize());

 	//  set parser start conditions
	gl_nLexFirstTime = true;

	return E_NoError;
}
*/

/***************************************************************************
 *
 * prepareBufferFromBinary
 *
 ***************************************************************************/
/*
StatusCode ZLexLexer::prepareBufferFromBinary(ZLexFileBuffer *zbuf) {

	StatusCode sc = E_NoError;
	//reset parser state, variables
	init();

	//assign the passed buffer to the parser buffer pointer
	m_pLexCharBuffer = zbuf;

	// assign the buffer to Flex
	gl_pLexInputBuf = m_pLexCharBuffer->getBuffer();
	gl_pLexInputPtr = m_pLexCharBuffer->getBuffer();
	//gl_pLexInputLim = m_pLexCharBuffer.buf + (m_pLexCharBuffer.size - 1);
	gl_pLexInputLim = m_pLexCharBuffer->getBuffer() + (m_pLexCharBuffer->getSize() -1);
	//gl_nLexInputLim = (m_pLexCharBuffer->getSize());

 	//  set parser start conditions
	gl_nLexFirstTime = true;

	return E_NoError;
}
*/
/***************************************************************************
 *
 * doLexHeader
 *
 ***************************************************************************/
StatusCode ZLexLexer::doLexHeader() {
  m_eLexerStatus = E_NoError;
  m_eLexerError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eLexerStatus = prepare();
	//if(m_eLexerStatus != E_NoError)
	//	return m_eLexerStatus;

  try{
		//call the lexer loop proc
		m_eLexerError = doLexHeaderProc();
		//rc = (LexError) zFile_parse();
  }
  catch(...)
  {
		m_eLexerError = E_LEX_EXCEPTION;
		return m_eLexerStatus = E_Exception;
  }

	return m_eLexerStatus;
}

/***************************************************************************
 *
 * doLexData
 *
 ***************************************************************************/
StatusCode ZLexLexer::doLexData() {
  m_eLexerStatus = E_NoError;
  m_eLexerError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eLexerStatus = prepare();
	//if(m_eLexerStatus != E_NoError)
	//	return m_eLexerStatus;

  try{
		//call the lexer loop proc
		m_eLexerError = doLexDataProc();
		//rc = (LexError) zFile_parse();
  }
  catch(...)
  {
		m_eLexerError = E_LEX_EXCEPTION;
		return m_eLexerStatus = E_Exception;
  }

	return m_eLexerStatus;
}

/***************************************************************************
 *
 * doLexHeaderProc
 *
 ***************************************************************************/
LexError ZLexLexer::doLexHeaderProc() {
		//lex the file
	std::string s = "";
	LexError e = E_LEX_OK;
	int lexReturn = -1;
		
	while(lexReturn != END_OF_FILE) {	
		lexReturn = zHdr_lex();
		switch(lexReturn) { 
		case LINE_NUMBER:  s = "LINE_NUMBER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;

		case ALPHA_CELL:  s = "ALPHA_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case ERROR_CELL:  s = "ERROR_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case QUOTED_CELL: s = "QUOTED_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case DELIMITER:  s = "DELIMITER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case NEW_LINE:    s = "NEW_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case COMMENT_LINE: s = "COMMENT_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case END_OF_FILE: s = "END_OF_FILE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
		default: break;
		}
	}
	return e;
}


/***************************************************************************
 *
 * doLexDataProc
 *
 ***************************************************************************/
LexError ZLexLexer::doLexDataProc() {
		//lex the file
	std::string s = "";
	LexError e = E_LEX_OK;
	int lexReturn = -1;
		
	while(lexReturn != END_OF_FILE) {	
		lexReturn = zData_lex();
		switch(lexReturn) { 
		case LINE_NUMBER:  s = "LINE_NUMBER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;

		case ALPHA_CELL:  s = "ALPHA_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case ERROR_CELL:  s = "ERROR_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case QUOTED_CELL: s = "QUOTED_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case DELIMITER:  s = "DELIMITER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case NEW_LINE:    s = "NEW_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case COMMENT_LINE: s = "COMMENT_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
			case END_OF_FILE: s = "END_OF_FILE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount      << endl;       // count number of lines in file 
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount       << endl;       // count number of lines in section 
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount    << endl;       // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount     << endl;       // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl;       // count number of chars scanned in this file 
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl;       // count number of chars on this line 
									break;
		default: break;
		}
	}
	return e;
}

/***************************************************************************
 *
 * doParseHeader
 *
 ***************************************************************************/
StatusCode ZLexLexer::doParseHeader() {
  m_eLexerStatus = E_NoError;
  m_eLexerError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eLexerStatus = prepare();
	//if(m_eLexerStatus != E_NoError)
	//	return m_eLexerStatus;

  try{
		//call the lexer loop proc
		m_eLexerError = (LexError) zHdr_parse();
  }
  catch(...)
  {
		m_eLexerError = E_LEX_EXCEPTION;
		return m_eLexerStatus = E_Exception;
  }

	return m_eLexerStatus;
}

/***************************************************************************
 *
 * doParseData
 *
 ***************************************************************************/
StatusCode ZLexLexer::doParseData() {
  m_eLexerStatus = E_NoError;
  m_eLexerError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eLexerStatus = prepare();
	//if(m_eLexerStatus != E_NoError)
	//	return m_eLexerStatus;

  try{
		//call the lexer loop proc
		m_eLexerError = (LexError) zData_parse();
  }
  catch(...)
  {
		m_eLexerError = E_LEX_EXCEPTION;
		return m_eLexerStatus = E_Exception;
  }

	return m_eLexerStatus;
}

/***************************************************************************
 *
 * doParse
 *
 ***************************************************************************/
/*
LexError ZLexLexer::doFileLineParse() {
  LexError rc       = E_LEX_OK;

	//fill the buffer and get ready to parse
	prepare();

  try{
		//call the flex/bison parser
		//rc = (LexError) zFile_lex();
		//rc = (LexError) zFile_parse();
  }
  catch(...)
  {
		m_eLexerStatus = E_Exception;
		return rc;
  }

  if(rc != E_LEX_OK) {
    m_eLexerStatus = E_LexerError;
		return rc;
	}

	m_eLexerStatus = E_NoError;
	return rc;
}
*/

/***************************************************************************
 *
 * doParse
 *
 ***************************************************************************/
/*
LexError ZLexLexer::doHeaderLineParse() {
  LexError rc       = E_LEX_OK;

	//fill the buffer and get ready to parse
	prepare();

  try{
		//call the flex/bison parser
		//rc = (LexError) zHdr_lex();
		//rc = (LexError) zHdr_parse();
  }
  catch(...)
  {
		m_eLexerStatus = E_Exception;
		return rc;
  }

  if(rc != E_LEX_OK) {
    m_eLexerStatus = E_LexerError;
		return rc;
	}

	m_eLexerStatus = E_NoError;
	return rc;
}
*/
/***************************************************************************
 *
 * doParse
 *
 ***************************************************************************/
/*
LexError ZLexLexer::doDataLineParse() {
  LexError rc       = E_LEX_OK;

	//fill the buffer and get ready to parse
	prepare();

  try{
		//call the flex/bison parser
		//rc = (LexError) zData_lex();
		//rc = (LexError) zData_parse();
  }
  catch(...)
  {
		m_eLexerStatus = E_Exception;
		return rc;
  }

  if(rc != E_LEX_OK) {
    m_eLexerStatus = E_LexerError;
		return rc;
	}

	m_eLexerStatus = E_NoError;
	return rc;
}
*/

/***************************************************************************
 *
 * getTokenList
 *
 ***************************************************************************/
ZLexToken* ZLexLexer::getTokenList() {
  return gl_pLexTokenList;
}

/***************************************************************************
 *
 * release
 *
 ***************************************************************************/
StatusCode ZLexLexer::release() {
  //destroy all parser memory
  // todo: copy global error info
  //LexDestroy();

	//delete m_pLexCharBuffer;

	return E_NoError;
}

/***************************************************************************
 *
 * showTokens
 *
 ***************************************************************************/
void ZLexLexer::showTokens() {
	ZLexToken* z = gl_pLexTokenList;
	ZLexToken* c = z;
	while(c!=NULL) {
		if(c->text_ != NULL)
			cout << c->text_ << endl;
    c = c->next_;
	}
}

/***************************************************************************
 *
 * fillLexBufferFromFile
 *
 ***************************************************************************/
/*
StatusCode ZLexLexer::fillLexBufferFromFile(LEXFILEBUFFER* pLexFileBuf, const char* fname, int& byteCounter)
{
	StatusCode retval = E_NoError;
	FILE		*ifp = NULL;
	size_t	byteCount = 0;
	int			nextChar = 0;
  //bool    b_unix = false;

	// initialize the file buffer
	pLexFileBuf->buf = (char*)NULL;
	//b_unix = IsFileUnixFormat(fname);

	if ((ifp = fileOpen(fname)) == (FILE*)NULL)
	{
		retval = E_FileError;
		return retval;
	}
	else
	{
		fileBufferFree(pLexFileBuf);
		//init the m_pLexCharBuffer buffer
		pLexFileBuf->size = LEXFIRSTBUFSIZE;
		pLexFileBuf->buf = (char*)malloc(LEXFIRSTBUFSIZE * sizeof(char));
		//m_pLexCharBuffer->buf = (char*)malloc(LEXFIRSTBUFSIZE * sizeof(char),"m_pLexCharBuffer->buf");
		pLexFileBuf->off = 0;
		if (pLexFileBuf->buf == (char*)NULL)
		{
			fclose(ifp);
			retval = E_MemError;
		}
		else
		{
			//fill the m_pLexCharBuffer->buf
			byteCount = 0;
			while ((nextChar = fgetc(ifp)) != EOF)
			{
				if (fileBufferPutChar(pLexFileBuf,(unsigned char)nextChar) == E_BufferError)
				{
					fclose(ifp);
					retval = E_BufferError;
					break;
				}
				else
				{
					byteCount++;
				}
			}
			//"help" the lexer out by appending a newline ('\n') as the last char in line
			//so that a line of delimitted fields has, at minimum, a leading tab
			//and an ending new line.  This case would essentially be an empty field line
			//only add the new-line char if the last char is not a new-line
			int lastChar = pLexFileBuf->buf[byteCount];
			if(lastChar != 10)
			{
				nextChar = 10; 
				if (fileBufferPutChar(pLexFileBuf,(unsigned char)nextChar) == E_BufferError)
				{
						fclose(ifp);
						retval = E_BufferError;
				}
				else
				{
					byteCount++;
				}
			}			
			if (retval >= 0)
			{
				//  reset the buffer offset and size 
				pLexFileBuf->off = 0;
				pLexFileBuf->size = byteCount+1;
				fclose(ifp);
			}
		}
	}
	return retval;
}
*/
/***************************************************************************
 *
 * fillLexBuffer(const char* inputBuffer)
 *
 * preconditions: 
 * the input buffer has a delimitter preceeding any text that is to be scanned
 *
 * postconditions:
 * the output buffer will have a trailing new-line char to ensure proper lexer
 * termination
 *
 ***************************************************************************/
StatusCode ZLexLexer::fillLexBuffer(const char* inputBuffer)
{
	return m_pLexCharBuffer->fillFromBuffer(inputBuffer);
}

/***************************************************************************
 *
 * fillLexBufferFromBuf(output,input,outputsize)
 *
 * preconditions: 
 * the input buffer has a delimitter preceeding any text that is to be scanned
 *
 * postconditions:
 * the output buffer will have a trailing new-line char to ensure proper lexer
 * termination
 *
 ***************************************************************************/
/*
StatusCode ZLexLexer::fillLexBufferFromBuf(LEXFILEBUFFER* pLexFileBuf, const char* inputBuffer, int& byteCounter)
{
	StatusCode retval = E_NoError;
	size_t byteCount = 0;
	int	nextChar = 0;
  //bool    b_unix = false;

	// initialize the file inputBuffer
	pLexFileBuf->buf = (char*)NULL;
	//b_unix = IsFileUnixFormat(fname);

	if (inputBuffer == NULL) {
		retval = E_BufferEmpty;
	}
	else {
		fileBufferFree(pLexFileBuf);
		//init the m_pLexCharBuffer buffer
		//m_pLexCharBuffer->size = FIRSTBUFSIZE;
		//pLexFileBuf->size = sizeof(inputBuffer + 1); //make room for "helper" '\n' char
		pLexFileBuf->size = sizeof(inputBuffer); //make room for "helper" '\n' char
    
		//allocate buffer memory
		pLexFileBuf->buf = (char*)malloc(pLexFileBuf->size * sizeof(char));
		//m_pLexCharBuffer->buf = (char*)malloc(m_pLexCharBuffer->size,"m_pLexCharBuffer->buf");
		//memset
		int chk = sizeof(pLexFileBuf->buf);
		memset(pLexFileBuf->buf,'\0',pLexFileBuf->size);
		pLexFileBuf->off = 0;
		if (pLexFileBuf->buf == (char*)NULL) {
			retval = E_BufferError;
		}
		else {			
			//copy all the chars from input buffer to parser buffer
			while ((nextChar = inputBuffer[byteCount]) != NULL) {
				if (fileBufferPutChar(pLexFileBuf,(unsigned char)nextChar) == E_BufferError) {
					retval = E_BufferError;
					break;
				}
				else {
					byteCount++;
				}
			}
			//"help" the lexer out by appending a newline ('\n') as the last char in line
			//so that a line of delimitted fields has, at minimum, a leading tab
			//and an ending new line.  This case would essentially be an empty field line
			//only add the new-line char if the last char is not a new-line
			int lastChar = pLexFileBuf->buf[byteCount-1];
			if(lastChar != 10) {
				nextChar = 10; 
				if (fileBufferPutChar(pLexFileBuf,(unsigned char)nextChar) == E_BufferError) {
						retval = E_BufferError;
				}
				else {
					byteCount++;
				}
			}

			if (retval >= 0) {
				//reset the buffer offset and size
				pLexFileBuf->off = 0;
				pLexFileBuf->size = byteCount;
			}
		}
	}
	byteCounter = byteCount;
	return retval;
}
*/
/******************************************************************************
 *	fileBufferInit
 ******************************************************************************/
/*
StatusCode ZLexLexer::fileBufferInit(LEXFILEBUFFER* fb, int size)
{
	StatusCode retval = E_NoError;

	fileBufferFree(fb);

	fb->size = size;
	fb->buf = (char*)malloc(size * sizeof(unsigned char));
	//fb->buf = (char*)malloc(size * sizeof(unsigned char), "filebuffer");
	fb->off = 0;
	if (fb->buf != (char*)NULL)
	{
		return (fb->size);
	}
	else
	{
		return 0;
	}
}
*/

/******************************************************************************
 *	fileBufferFree
 ******************************************************************************/
/*
StatusCode ZLexLexer::fileBufferFree(LEXFILEBUFFER* fb)
{
	StatusCode retval = E_NoError;
	if (fb->buf != (char*)NULL)
	{
		free(fb->buf);
		//free(fb->buf, "filebuffer");
		fb->buf = (char*)NULL;
	}
	return E_NoError;
}
*/
/******************************************************************************
 *	Write a char to memory.
 ******************************************************************************/
/*
StatusCode ZLexLexer::fileBufferPutChar(LEXFILEBUFFER* fb, char byte)
{
	StatusCode retval = E_NoError;
	
	if (fb->off == fb->size)
	{
		if (fileBufferEnlarge(fb) == E_BufferError)
		{
			return E_BufferError;
		}
	}
	fb->buf[fb->off++] = byte;

	return retval;
}
*/
/******************************************************************************
 *	fileBufferEnlarge
 * double the output memory size.
 ******************************************************************************/
/*
StatusCode ZLexLexer::fileBufferEnlarge(LEXFILEBUFFER* fb)
{
	char* newbuf;

	newbuf = (char*)calloc((fb->size << 1) * sizeof(char), sizeof(char));
	if (newbuf != (char*)NULL)
	{
		memcpy(newbuf, fb->buf, fb->size);
		free(fb->buf);
		//free(fb->buf,"fb->buf");
		fb->size <<= 1;
		fb->buf = newbuf;
	}
	else
		return E_BufferError;

	return E_NoError;
}
*/
/******************************************************************************
 *	return the validity of reading the next char in the buffer.
 ******************************************************************************/
/*
bool ZLexLexer::fileBufferCanRead(LEXFILEBUFFER* fb)
{
	bool	retval;

	if (fb->off < fb->size)
	{
		retval = true;
	}
	else
	{
		retval = false;
	}
	return retval;
}
*/
/***************************************************************************
 * fileOpen
 ***************************************************************************/
FILE* ZLexLexer::fileOpen(const char* fileName)
{
	FILE* fp;

	if (fileName != (char*)NULL)
	{
		fp = fopen(fileName, "rb");
	}
	else
	{
		fp = (FILE*)NULL;
	}
	return (fp);
}

/******************************************************************************
 *	fileOpenExt
 ******************************************************************************/
/*
FILE* ZLexLexer::fileOpenExt(const char* name)
{
	FILE* fp;

	if (name != (char*)NULL)
	{
		fp = fopen(name, "rb");
	}
	else
	{
		fp = (FILE*)NULL;
	}
	return (fp);
}
*/

/***************************************************************************
 * initTokenList
 ***************************************************************************/
/*
ZLexToken* ZLexLexer::initLexTokenList(void) { 	
	ZLexToken* rootToken = new ZLexToken( 
			"",                        // scanned text of token         	        		 
			0,                         // line number from file                
      0,                         // row number from section              
      0,                         // column number                        
      0,                         // char offset from beginning of line   							 
      0,                         // char offset from beginning of file   							                 
			LEXTOKEN_ROOT,             // id, value, field,...                  
			LEXTOKEN_DATATYPE_NULL,    // datatype representation                                   
			LEXSCAN_COMPLETE,          // scan status                                                 
			gl_pLexCurrentToken);

	return rootToken;
}
*/

/***************************************************************************
 * destroyTokenList
 ***************************************************************************/
void ZLexLexer::destroyTokenList() {
	ZLexToken* tok = gl_pLexTokenList;	
	ZLexToken* temp = NULL;	
	while(tok != NULL)
	{		
		temp = tok;
		destroyToken(temp);
		tok = tok->next_;
	}
}

/***************************************************************************
 * destroyToken
 ***************************************************************************/
void ZLexLexer::destroyToken(ZLexToken* tok)
{
	ZLexToken* temp = tok;

	if(temp->text_)
	{
		free(temp->text_);
		//free(temp->text, temp->text);
	}

	if(temp->name_)
	{
		free(temp->name_);
		//free(temp->desc_, temp->text_);
	}
	free(temp);
	//free(temp, "ZLEXTOKEN *");
}




