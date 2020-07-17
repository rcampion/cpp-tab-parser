/**************************************************************************
 *
 * zLexError.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexError.h
*/
#ifndef ___ZLEXERR_H___
#define ___ZLEXERR_H___

//grep the code for E_LEX_
//provide and index here and a message in .cpp

// variables/functions defined in lexerror.cpp
extern bool   gl_nErrInitialized;
extern bool		InitLexErrMsg(void);
extern char*	GetLexErrorString(unsigned int ErrNo);

	/******************** Error messages ****************/
enum LexError {
 E_LEX_OK	= 0                           ,

	/* Internal parser errors */
 E_LEX_PARSE                            ,
 E_LEX_PARSEPARAM                       ,
 E_LEX_PARSESTACK                       ,

	/* Lexical errors */
 E_LEX_EMBEDDED_SPACE                   ,	
 E_LEX_INVALID_ESCAPE                   ,
 E_LEX_SECTION                          ,
 E_LEX_STRING_LENGTH                    ,
 E_LEX_UNTERMINATED_STRING              ,	

	/* Semantic errors */	
 E_LEX_DUPLICATE_FIELD                  ,
 E_LEX_DUPLICATE_LABEL                  ,
 E_LEX_DUPLICATE_ID                     ,
 E_LEX_DUPLICATE_SECTION                ,
 E_LEX_DUPLICATE_VALUE                  ,
 E_LEX_HEADER_ENTRY                     ,
 E_LEX_HEADER_NAME_ENTRY                ,
 E_LEX_LABEL                            ,	
 E_LEX_LABEL_DECLARED_TWICE             ,
 E_LEX_LABEL_NOT_DEFINED                ,
 E_LEX_MISSING_SECTION                  ,
 E_LEX_PARAMETER,
 E_LEX_STRING                           ,
 E_LEX_UNEXPECTED_TEXT                  ,

	/* Critical internal errors */
 E_LEX_EXCEPTION                        ,
 E_LEX_INVALID_RTN_VALUE                ,	
 E_LEX_NOMEMORY                         ,				
 E_LEX_MEM_ERROR,

	/* File errors */			
 E_LEX_BUFFER_EMPTY                     ,	
 E_LEX_BUFFER_ERROR                     ,
 E_LEX_FILE_EMPTY                       ,
 E_LEX_FILE_ERROR                       ,	


	/* Default error */			
 E_LEX_UNDEFINED,

 E_LEX_MAXERR
};

#endif /* ___LEXERROR_H___ */


