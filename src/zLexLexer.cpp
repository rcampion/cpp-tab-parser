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
bool gl_nLexFirstTime = true;        // one time set up flag for parse
int gl_nLexLineCount = 0;            // count number of lines in file
int gl_nLexRowCount = 0;             // count number of lines in section
int gl_nLexColumnCount = 0;          // count number of columns on this line
int gl_nLexFieldCount = 0;           // count number of fields on this line
int gl_nLexFileCharOffset = 0;     // count number of chars scanned in this file
int gl_nLexLineCharOffset = 0;       // count number of chars on this line
int gl_nLexHeaderRowCount = 0;       // count number of lines in header section
int gl_nLexHeaderFieldCount = 0;     // count number of fields in header row
int gl_nLexHeaderDelimiterCount = 0; // count number of delimiters in header row
int gl_nLexDataRowCount = 0;         // count number of lines in data section
int gl_nLexDataFieldCount = 0;       // count number of fields in data row
int gl_nLexDataDelimiterCount = 0;   // count number of delimiters in data row

LexFileSection gl_nLexCurrentSection = LEXFILESECTION_ROOT;
ZLexToken *gl_pLexCurrentToken = 0;

//the list of tokens accumulated during a parse
ZLexToken *gl_pLexTokenList = 0;

//the list of tokens with errors during a parse
ZLexToken *gl_pLexErrorList = 0;

int gl_nQuietLevel = MAX_QUIET;

/***************************************************************************
 *
 * default constructor
 *
 ***************************************************************************/
ZLexLexer::ZLexLexer() {
	init();
}
/***************************************************************************
 *
 * construct using char* (buffer)
 *
 ***************************************************************************/
ZLexLexer::ZLexLexer(ZLexFile *zfile, ZLexFileBuffer *zbuf) {

	init();

	//do we have anything to parse?
	if (zbuf->getSize() > 0) {
		m_eLexerStatus = zbuf->prepareBufferFromBinary();
	} else {
		m_eLexerStatus = E_BufferEmpty;
		m_eLexerError = E_LEX_BUFFER_EMPTY;
	}
}

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
	if (!gl_nErrInitialized)
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
	gl_nLexLineCount = 0;       // reset number of lines in file
	gl_nLexRowCount = 0;        // reset number of lines in section
	gl_nLexColumnCount = 0;     // reset number of columns on this line
	gl_nLexFieldCount = 0;      // reset number of fields on this line
	gl_nLexFileCharOffset = 0;  // reset number of chars scanned in this file
	gl_nLexLineCharOffset = 0;  // reset number of chars on this line

	//winging it
	gl_pLexCurrentToken = NULL;
	gl_pLexErrorList = NULL;

	//not sure if this is necessary - will test
	//gl_nLexCurrentSection = LEXFILESECTION_ROOT;

	// create a default root token
	gl_pLexTokenList = new ZLexToken("<root\\>",             // text
			gl_nLexLineCount,                   // line
			gl_nLexRowCount,                    // row
			gl_nLexColumnCount,                 // column
			gl_nLexLineCharOffset, // char offset from beginning of line
			gl_nLexFileCharOffset, // char offset from beginning of file
			LEXTOKEN_SECTION,                   // id, value, field
			LEXTOKEN_DATATYPE_STRING, // datatype representation
			LEXSCAN_COMPLETE,                   // scan status
			NULL);				// parent token;

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

	try {
		//call the lexer loop proc
		m_eLexerError = doLexHeaderProc();
		//rc = (LexError) zFile_parse();
	} catch (...) {
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

	try {
		//call the lexer loop proc
		m_eLexerError = doLexDataProc();
		//rc = (LexError) zFile_parse();
	} catch (...) {
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

	while (lexReturn != END_OF_FILE) {
		lexReturn = zHdr_lex();
		switch (lexReturn) {
		case LINE_NUMBER:
			s = "LINE_NUMBER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;

		case ALPHA_CELL:
			s = "ALPHA_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case ERROR_CELL:
			s = "ERROR_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case QUOTED_CELL:
			s = "QUOTED_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case DELIMITER:
			s = "DELIMITER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case NEW_LINE:
			s = "NEW_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case COMMENT_LINE:
			s = "COMMENT_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case END_OF_FILE:
			s = "END_OF_FILE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		default:
			break;
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

	while (lexReturn != END_OF_FILE) {
		lexReturn = zData_lex();
		switch (lexReturn) {
		case LINE_NUMBER:
			s = "LINE_NUMBER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;

		case ALPHA_CELL:
			s = "ALPHA_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case ERROR_CELL:
			s = "ERROR_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case QUOTED_CELL:
			s = "QUOTED_CELL";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case DELIMITER:
			s = "DELIMITER";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case NEW_LINE:
			s = "NEW_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case COMMENT_LINE:
			s = "COMMENT_LINE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		case END_OF_FILE:
			s = "END_OF_FILE";
			cout << s.c_str() << endl;
			cout << "gl_nLexLineCount.......:" << gl_nLexLineCount << endl; // count number of lines in file
			cout << "gl_nLexRowCount........:" << gl_nLexRowCount << endl; // count number of lines in section
			cout << "gl_nLexColumnCount.....:" << gl_nLexColumnCount << endl; // count number of columns on this line
			cout << "gl_nLexFieldCount......:" << gl_nLexFieldCount << endl; // count number of fields on this line
			cout << "gl_nLexFileCharOffset..:" << gl_nLexFileCharOffset << endl; // count number of chars scanned in this file
			cout << "gl_nLexLineCharOffset..:" << gl_nLexLineCharOffset << endl; // count number of chars on this line
			break;
		default:
			break;
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

	try {
		//call the lexer loop proc
		m_eLexerError = (LexError) zHdr_parse();
	} catch (...) {
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

	try {
		//call the lexer loop proc
		m_eLexerError = (LexError) zData_parse();
	} catch (...) {
		m_eLexerError = E_LEX_EXCEPTION;
		return m_eLexerStatus = E_Exception;
	}

	return m_eLexerStatus;
}

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
	ZLexToken *z = gl_pLexTokenList;
	ZLexToken *c = z;
	while (c != NULL) {
		if (c->text_ != NULL)
			cout << c->text_ << endl;
		c = c->next_;
	}
}

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
StatusCode ZLexLexer::fillLexBuffer(const char *inputBuffer) {
	return m_pLexCharBuffer->fillFromBuffer(inputBuffer);
}

/***************************************************************************
 * fileOpen
 ***************************************************************************/
FILE* ZLexLexer::fileOpen(const char *fileName) {
	FILE *fp;

	if (fileName != (char*) NULL) {
		fp = fopen(fileName, "rb");
	} else {
		fp = (FILE*) NULL;
	}
	return (fp);
}

/***************************************************************************
 * destroyTokenList
 ***************************************************************************/
void ZLexLexer::destroyTokenList() {
	ZLexToken *tok = gl_pLexTokenList;
	ZLexToken *temp = NULL;
	while (tok != NULL) {
		temp = tok;
		destroyToken(temp);
		tok = tok->next_;
	}
}

/***************************************************************************
 * destroyToken
 ***************************************************************************/
void ZLexLexer::destroyToken(ZLexToken *tok) {
	ZLexToken *temp = tok;

	if (temp->text_) {
		free(temp->text_);
		//free(temp->text, temp->text);
	}

	if (temp->name_) {
		free(temp->name_);
		//free(temp->desc_, temp->text_);
	}
	free(temp);
	//free(temp, "ZLEXTOKEN *");
}

