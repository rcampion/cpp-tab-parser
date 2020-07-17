/****************************************************************************
*
* TabDataLineLexer.y  -parser definition.
*
* --------------------- 
* flex info:
* flex version 2.5.4  
* IDE settings
* DEBUG
* --------------------- 
* bison info:
* GNU Bison version 1.25
* IDE settings
* DEBUG
* bison -y -d --debug -v --verbose -p zData_ -o.\tab\BisonTabDataLineParser.cpp .\tab\TabDataLineParser.y
* RELEASE
* bison -y -d -p zData_ -o.\tab\BisonTabDataLineParser.cpp .\tab\TabDataLineParser.y
* MAKEFILE
* bison -y -d -p zData_ -o.\tab\BisonTabDataLineParser.cpp .\tab\TabDataLineParser.y
* ---------------------
* Note: file best viewed with tabs set = 2  
*
*****************************************************************************/

/****************************************************************************/
/***************************   Lines copied into parser     *****************/
/****************************************************************************/
%{
#include "zDwConst.h"
#include "zLexBase.h"
#include "zLexToken.h"
#include "zLexError.h"
#include "zLexParser.h"
#include "zLexFile.h"
#include "zLexSection.h"
#include "zLexEntry.h"

	/* debug code - remove from production - begin */
	//#ifdef _DEBUG
		#define YYDEBUG 1
	//#endif
	/* debug code - remove from production - end */

%}
/****************************************************************************/
/***************************   Definitions                  *****************/
/****************************************************************************/
/* union definition of all tokens */
%union {
	char    *sval;
	int			ival;
	ZLexToken	*stok;
}

%token <stok> ALPHA_CELL
%token <stok> ERROR_CELL
%token <stok> QUOTED_CELL
%token <stok> DELIMITER
%token <stok> NEW_LINE
%token <stok> COMMENT_LINE
%token <stok> END_OF_FILE
%token <stok> LINE_NUMBER

%type  <stok> file_lines
%type  <stok> file_line
%type  <stok> line_items
%type  <stok> line_item

%%

/****************************************************************************/
/****************				LANGUAGE GRAMMAR                    *****************/
/****************************************************************************/
textfile
	: END_OF_FILE
		{			
			return (E_LEX_OK);
			//return (0);
		}
	| file_lines END_OF_FILE
		{			
			return (E_LEX_OK);
			//return (0);
		}
	| file_lines error
		{
			//SetLexError(E_LEX_SECTION,GetLexCurrentToken(),"");
			//errortest = $3;
			///HACK!!! ... error token is being generated for unknown reason
			//return (E_LEX_PARSE);
			return (E_LEX_OK);
			//return (-1);
		}
	| error
		{
			//SetLexError(E_LEX_SECTION,GetLexCurrentToken(),"");
			//errortest = $3;
			return (E_LEX_PARSE);
			//return (-1);
		}
	;


/****************************************************************************/
/****************							 Header Section               *****************/
/****************************************************************************/
file_lines
	: file_line
		{
			ZLexToken * pLineItems;
			pLineItems = $1;	
		}
	| file_lines file_line
		{
			ZLexToken * pLineItems;
			pLineItems = $1;	
		}
	;

file_line
	: LINE_NUMBER line_items NEW_LINE
		{
			/* this is a complete entry(line of data) */
			ZLexToken * pLineItems;
			pLineItems = $1;
						
			//what to do with this list of tokens ??
			//gl_pLexTokenList = pLineItems;

			//EmitLEXTOKENLine(pHeaderItems,gl_nLexHeaderFieldCount);
      //an entry line is complete
			//ASection	*pSec = gl_pLexFile->GetCurrentSection();
			//pSec->AddEntryLine(pLineItems,false);
			//return (RET_PARSE_HEADER_OK);

			$$ = $1;
			//exit "done after each line for now...testing"
			//return (0);
		}
	| LINE_NUMBER COMMENT_LINE
	  {
			ZLexToken * pLineItems;
			pLineItems = $1;
			$$ = $1;
		}
	| LINE_NUMBER NEW_LINE
	  {
			ZLexToken * pLineItems;
			pLineItems = $1;
			$$ = $1;
		}
	| NEW_LINE
	  {
			ZLexToken * pLineItems;
			pLineItems = $1;
			$$ = $1;
		}
	;

line_items
	: line_item
		{
		  ZLexToken *tok = $1;
		  //EmitLEXTOKEN(gl_nQuietLevel,tok);
			$$ = $1;
		}
	| line_items line_item
		{
			ZLexToken * hn;
			if ($1 == NULL) {
				$$ = $2;
			} else {
				hn = $1;
				//no longer appending tokens here
				//while (hn->next_ != NULL)
				//	hn = hn->next_;
				//hn->next_ = $2;
				$$ = $1;
				//EmitLEXTOKEN(gl_nQuietLevel,hn);
			}		
		}
	;

line_item
	: ALPHA_CELL				   
		{
		  ZLexToken *tok = $1;
		  //EmitLEXTOKEN(gl_nQuietLevel,tok);
			$$ = $1; 	
		}
	| QUOTED_CELL				   
		{
		  ZLexToken *tok = $1;
		  //EmitLEXTOKEN(gl_nQuietLevel,tok);
			$$ = $1; 	
		}
	| DELIMITER				   
		{
		  ZLexToken *tok = $1;
		  //EmitLEXTOKEN(gl_nQuietLevel,tok);
			$$ = $1; 	
		}
	| ERROR_CELL					   
		{
		  ZLexToken *tok = $1;
		  //EmitLEXTOKEN(gl_nQuietLevel,tok);
			$$ = $1; 	
		}
	;

/************************ Subroutines        ********************************/
%%

void zData_error(char* msg)
{
	if(gl_nQuietLevel == 0)
	{
		fprintf(stderr, "\nERROR:\tMsg   : %s",msg);	
		fprintf(stderr, "\n      \tLine  : %d",gl_nLexLineCount);
		fprintf(stderr, "\n      \tText  : %s\n",zData_text);
	}
}
