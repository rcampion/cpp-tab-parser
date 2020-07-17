/**************************************************************************
 *
 * zLexError.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexError.cpp
*/
#include "zLexError.h"

bool gl_nErrInitialized = false;

char* LexErrMsg[E_LEX_MAXERR];

bool InitLexErrMsg(void)
{
	int	i = 0;

	if (gl_nErrInitialized == false) 
	{

		for (i = 0; i < E_LEX_MAXERR; i++)
		{
			LexErrMsg[i] = "Internal error number %d.";
		}

	 /* Internal parser errors */
		LexErrMsg[E_LEX_OK] = "No error or Message not available";

   /* Bison returns 1 for different errors.
		* Identifying the exact error would require modification of the bison sources, 
		* (not worth the effort). 
		* Return "cannot parse/parser error/parser jammed" which is definitely correct.
		* (parser jammed is the message seen during debug) */
		LexErrMsg[E_LEX_PARSE] = "Parser Error, Parser Jammed, Cannot Parse.";

		LexErrMsg[E_LEX_PARSEPARAM] = "Invalid Run Parameter";

	/* These are bison generated, normally syntax errors */ 
		LexErrMsg[E_LEX_PARSESTACK]="Fatal Parser Error - Parse stack overflow.";
 
	/* Lexical errors */
	/* Parse errors from grep of lexer */
		LexErrMsg[E_LEX_EMBEDDED_SPACE]= "Blank space not allowed.";
		LexErrMsg[E_LEX_INVALID_ESCAPE]= "Invalid Escape Error";               
		LexErrMsg[E_LEX_SECTION]="Section Error";                        
		LexErrMsg[E_LEX_STRING_LENGTH]= "String Length";
		LexErrMsg[E_LEX_UNTERMINATED_STRING]= "Unterminated String Error";

	/* Semantic errors */			
		LexErrMsg[E_LEX_DUPLICATE_FIELD] = "FieldName Previously Defined";
		LexErrMsg[E_LEX_DUPLICATE_LABEL] = "Label Previously Defined";
		LexErrMsg[E_LEX_DUPLICATE_ID] = "Identifier Previously Defined";
		LexErrMsg[E_LEX_DUPLICATE_SECTION] = "Section Previously Defined";
		LexErrMsg[E_LEX_DUPLICATE_VALUE] = "Value Previously Defined";
		LexErrMsg[E_LEX_HEADER_ENTRY				] = "Error";
		LexErrMsg[E_LEX_HEADER_NAME_ENTRY		] = "Error";
		LexErrMsg[E_LEX_LABEL								] = "Error";
		LexErrMsg[E_LEX_LABEL_DECLARED_TWICE] = "Error";
		LexErrMsg[E_LEX_LABEL_NOT_DEFINED		] = "Error";
		LexErrMsg[E_LEX_MISSING_SECTION] = "File Format: Missing %s Section ";
		LexErrMsg[E_LEX_PARAMETER]= "Invalid Parameter error";
		LexErrMsg[E_LEX_STRING]="String Error";
		LexErrMsg[E_LEX_UNEXPECTED_TEXT] = "Misplaced Text";

	/* Critical internal errors */
		LexErrMsg[E_LEX_EXCEPTION]= "Exception error";
		LexErrMsg[E_LEX_INVALID_RTN_VALUE]= "ReturnValue error";	
		LexErrMsg[E_LEX_NOMEMORY]= "Insufficient Memory";
		LexErrMsg[E_LEX_MEM_ERROR]= "Memory allocation error";

	/* File errors */
		LexErrMsg[E_LEX_BUFFER_EMPTY]= "Buffer Error: Buffer is empty.";
		LexErrMsg[E_LEX_BUFFER_ERROR]= "Buffer Error: Buffer could not be parsed.";
		LexErrMsg[E_LEX_FILE_EMPTY] = "File Error: File is empty.";
		LexErrMsg[E_LEX_FILE_ERROR] = "File Error: File could not be parsed.";
	}
	return gl_nErrInitialized = true;
}

char* GetLexErrorString(unsigned int errNo)
{
	if (errNo < E_LEX_MAXERR)
	{
		return (LexErrMsg[errNo]);
	}
	else
	{
		return "Error code exceeds maximum error entries!";
	}
}


