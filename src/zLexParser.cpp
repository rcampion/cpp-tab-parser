/**************************************************************************
 *
 * zLexParser.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexParser.cpp
 */
#include <iostream>
using namespace std;

#include "zLexBase.h"
#include "zLexError.h"
#include "zLexParser.h"
#include "zLexUtil.h"
#include "BisonTabHdrLineParser.hpp"

#include <string>
#include <sstream>
#include <math.h>  

/***************************************************************************
 *
 * default constructor
 *
 ***************************************************************************/
ZLexParser::ZLexParser() {
	init();
}
/***************************************************************************
 *
 * construct using char* (buffer)
 *
 ***************************************************************************/
ZLexParser::ZLexParser(ZLexFile *zfile, ZLexFileBuffer *zbuf) {
	//todo: buffer is empty! whose responsibility is it to fill it?
	// also 
	init();

	//do we have anything to parse?
	if (zbuf->getSize() > 0) {
		//m_sTextBuffer = ""; //we are using a char* based buffer, nullify this
		//m_pLexCharBuffer = NULL; //we are using a char* based buffer, initialize
		m_eParseStatus = zbuf->prepareBufferFromBinary();
	} else {
		m_eParseStatus = E_BufferEmpty;
		m_eParseError = E_LEX_BUFFER_EMPTY;
	}
}

/***************************************************************************
 *
 * destructor
 *
 ***************************************************************************/
ZLexParser::~ZLexParser() {
	release();
}

/***************************************************************************
 *
 * init
 *
 ***************************************************************************/
StatusCode ZLexParser::init() {
	//initialize the error messages
	if (!gl_nErrInitialized)
		InitLexErrMsg();

	m_pLexCharBuffer = NULL;
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

	gl_nLexFirstTime = true;

	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

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

	//not sure if this is necessary - test
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
 * doLexHdr
 *
 ***************************************************************************/
StatusCode ZLexParser::doLexHeader() {
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eParseStatus = prepare();
	//if(m_eParseStatus != E_NoError)
	//	return m_eParseStatus;

	try {
		//call the lexer loop proc
		m_eParseError = doLexHeaderProc();
		//rc = (LexError) zFile_parse();
	} catch (...) {
		m_eParseError = E_LEX_EXCEPTION;
		return m_eParseStatus = E_Exception;
	}

	return m_eParseStatus;
}

/***************************************************************************
 *
 * doLexData
 *
 ***************************************************************************/
StatusCode ZLexParser::doLexData() {
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

	//fill the buffer and get ready to parse
	//m_eParseStatus = prepare();
	//if(m_eParseStatus != E_NoError)
	//	return m_eParseStatus;

	try {
		//call the lexer loop proc
		m_eParseError = doLexDataProc();
		//rc = (LexError) zFile_parse();
	} catch (...) {
		m_eParseError = E_LEX_EXCEPTION;
		return m_eParseStatus = E_Exception;
	}

	return m_eParseStatus;
}

/***************************************************************************
 *
 * doLexHdrProc
 *
 ***************************************************************************/
LexError ZLexParser::doLexHeaderProc() {
	//lex the file
	std::string s = "";
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
	return (LexError) lexReturn;
}

/***************************************************************************
 *
 * doLexDataProc
 *
 ***************************************************************************/
LexError ZLexParser::doLexDataProc() {
	//lex the file
	std::string s = "";
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
	return (LexError) lexReturn;
}

/***************************************************************************
 *
 * doParseHeader
 *
 ***************************************************************************/
StatusCode ZLexParser::doParseHeader() {
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

	try {
		//call the lexer loop proc
		m_eParseError = (LexError) zHdr_parse();
	} catch (...) {
		m_eParseError = E_LEX_EXCEPTION;
		return m_eParseStatus = E_Exception;
	}

	return m_eParseStatus;
}

/***************************************************************************
 *
 * doParseData
 *
 ***************************************************************************/
StatusCode ZLexParser::doParseData() {
	m_eParseStatus = E_NoError;
	m_eParseError = E_LEX_OK;

	try {
		//call the lexer loop proc
		m_eParseError = (LexError) zData_parse();
	} catch (...) {
		m_eParseError = E_LEX_EXCEPTION;
		return m_eParseStatus = E_Exception;
	}

	return m_eParseStatus;
}

/***************************************************************************
 *
 * getTokenList
 *
 ***************************************************************************/
ZLexToken* ZLexParser::getTokenList() {
	return gl_pLexTokenList;
}

/***************************************************************************
 *
 * release
 *
 ***************************************************************************/
StatusCode ZLexParser::release() {
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
void ZLexParser::showTokens() {
	ZLexToken *z = gl_pLexTokenList;
	ZLexToken *c = z;
	while (c != NULL) {
		if (c->text_ != NULL)
			cout << c->text_ << endl;
		c = c->next_;
	}
}

/***************************************************************************
 * fileOpen
 ***************************************************************************/
FILE* ZLexParser::fileOpen(const char *fileName) {
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
void ZLexParser::destroyTokenList() {
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
void ZLexParser::destroyToken(ZLexToken *tok) {
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

