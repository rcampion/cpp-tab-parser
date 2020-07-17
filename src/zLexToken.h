/**************************************************************************
 *
 * zLexToken.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexToken.h
*/
#ifndef __ZLEXTOKEN_H__
#define __ZLEXTOKEN_H__

//#include "..\..\common\zdwconst.h"
#include "zDwConst.h"
#include "zLexError.h"

/***************************************************************************
 *
 * ZLexToken
 *
 **************************************************************************/
class ZLexToken {
public:
	ZLexToken();                    //default constructor
	ZLexToken(const ZLexToken& z);  //copy constructor
	ZLexToken& operator= (const ZLexToken& rhs);// assignment operator
	ZLexToken( 
		char *text,                  /* scanned text of token         			 */   		 
		int  line,                   /* line number from file                */
		int  row,                    /* row number from section              */
		int  column,                 /* column number                        */
		int  lineOffset,	           /* offset from beginning of line        */								 
		int  fileOffset,	           /* offset from beginning of file        */								         
		LexTokenType tokenType,      /* id, value, field,...                 */ 
		LexDataType dataType,        /* datatype representation              */                     
		LexScanStatus status,        /* scan status                          */  
		ZLexToken *parent            /* relationship to other token          */  
		);

	~ZLexToken(); //destructor

public:
	char*         name_;        // field name
	char*         text_;        // scanned text of token         				   
	int           length_;      // length of scanned text											 
	int           line_;        // line number from file
	int           row_;         // line number from section
	int           column_;      // column number
	int           lineOffset_;	// offset from beginning of line								 
	int           fileOffset_;	// offset from beginning of file								          
	LexTokenType  tokenType_;   // what is this symbol_; id_; value_; operator 
	LexDataType   dataType_;    // datatype representation                      
	LexScanStatus status_;      // scan status
	ZLexToken*    value_;       // mapped token to some other token     
	ZLexToken*    parent_;      // relationship to other token  
	ZLexToken*    child_;       // child of this token_; map                    
	ZLexToken*    next_;        // next token scanned                           
	ZLexToken*    prev_;        // previous token scanned   
};

/***************************************************************************
 *
 * ZErrorToken
 *
 **************************************************************************/
class ZErrorToken :public ZLexToken {
	ZErrorToken(ZLexToken z, LexError error) : ZLexToken(z) {};       
	LexError error_;       // error index of LexError message
};

/***************************************************************************
 *
 * function prototyes
 *
 **************************************************************************/
int	GetLexTokenTypeDesc(int nDesc, char szBuffer[], int nBufferSize);
int	GetLexTokenDataTypeDesc(int nDesc, char szBuffer[], int nBufferSize);

#endif // __ZLEXTOKEN_H__
