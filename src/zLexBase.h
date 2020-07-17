/**************************************************************************
 *
 * zLexBase.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 * 
 **************************************************************************/
/*! \file zLexBase.h
*/

#ifndef ___ZLEXBASE_H___
#define ___ZLEXBASE_H___

#include <stdio.h>

#include "zLexToken.h"

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

//globals

//the zHdr_ prefix is defined in the custom build of the flex/bison specs
// #define      zHdr_wrap() 1
extern int   zHdr_lex();
extern int   zHdr_parse();
extern char* zHdr_text;
extern int   zHdr_leng;
extern void  zHdr_error(char * msg);
extern int   zHdr_debug;

//the zData_ prefix is defined in the custom build of the flex/bison specs
// #define      zData_wrap() 1
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



#endif //___ZLEXBASE_H___


