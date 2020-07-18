/**************************************************************************
 *
 * zLexParser.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexParser.h
 */

#ifndef ___ZLEXPARSER_H___
#define ___ZLEXPARSER_H___

//#include "..\..\common\zdwconst.h"
#include "zDwConst.h"
#include "zLexToken.h"
#include "zLexError.h"
#include "zLexFile.h"
#include "zLexBuffer.h"

#include <string>

/****************************************************************************
 *
 * ZLexParser
 *
 *****************************************************************************/
/*! A parser is intended to be constructed with a buffer object which contains
 a char buffer of text to be parsed; 
 PreCondition:
 The text buffer is assigned to Flex, see YY_INPUT.
 Processing:
 doParse...() calls the Flex/Bison lex/parse routine yylex()/yyparse.
 Bison's yyparse() calls the Flex yylex() for input tokens.
 Flex scans for tokens according to the flex lexer specfication..see .l file
 Bison checks for token arrangements verifies the token order, see .y file
 Bison appends all tokens into a linked list.
 PostCondition
 A global linked list of tokens (ZLexToken's) is available to caller via:
 getTokenList();
 */
class ZLexParser {
public:
	ZLexParser();                                //default constructor
	ZLexParser(ZLexFile *zfile, ZLexFileBuffer *zbuf);
	~ZLexParser(); //destructor
	StatusCode doLexData();
	LexError doLexDataProc();
	StatusCode doLexHeader();
	LexError doLexHeaderProc();
	StatusCode doParseData();
	StatusCode doParseHeader();
	StatusCode getParseStatus() {
		return m_eParseStatus;
	}
	;
	LexError getParseError() {
		return m_eParseError;
	}
	;
	ZLexToken* getTokenList();
	StatusCode release();
	void showTokens();

private:

	ZLexFile *m_pLexFile;
	ZLexFileBuffer *m_pLexCharBuffer;

	ZLexToken *m_pLexTokenList;
	ZLexToken *m_pLexErrorList;

	FILE* fileOpen(const char *name);
	FILE* fileOpenExt(const char *name);

	StatusCode init();
	void initTokenList();
	void initErrorList();

	void destroyTokenList();
	void destroyToken(ZLexToken *tok);

	void destroyErrorList();
	void destroyError(ZLexToken *tok);

private:
	StatusCode m_eParseStatus;
	LexError m_eParseError;
};

#endif // ___ZLEXPARSER_H___
