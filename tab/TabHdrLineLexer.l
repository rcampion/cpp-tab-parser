/****************************************************************************
 *
 * TabHdrLineLexer.l - lexer definition for a tab delimitted file.
 * --------------------- 
 * flex info:
 * flex version 2.5.4  
 * IDE settings
 * DEBUG flex -8 -v -s -d -PzHdr_ -o.\tab\FlexTabHdrLineLexer.cpp .\tab\TabHdrLineLexer.l
 * RELEASE
 * flex -8 -PzHdr_ -o.\tab\FlexTabHdrLineLexer.cpp .\tab\TabHdrLineLexer.l
 * MAKEFILE
 * flex -8 -PzHdr_ -o.\tab\FlexTabHdrLineLexer.cpp .\tab\TabHdrLineLexer.l
 ***************************************************************************/

%{
//#include <io.h>

#include "zDwConst.h"
#include "zLexBase.h"
#include "zLexToken.h"
#include "zLexError.h"
#include "zLexParser.h"
#include "zLexFile.h"
#include "zLexSection.h"
#include "zLexEntry.h"

//include this last
#include "BisonTabHdrLineParser.hpp"

//use the following when pushing into and popping out of states
//%option stack
%}
%option noyywrap
 /*****************     Regular Expressions       *************************/
alpha_cell              [A-Za-z0-9]+
any_cell_first          ^[.]+
any_cell_next           [.]+
any_char_first          ^.+
carriage                \r
comment                 ^#.*\n
delimiter               \t
escape_char             \\[A-Za-z\\]{1,1}
line_number             ^[0-9]+[\)]{1}
newline                 \n
quote_double            \"
space                   [ ]

zentry_tag              ^"<zentry"[0-9]+"> "
zsection_tag            ^"<zsection"[0-9]+"> "

 /*****************     Options                   *************************/


 /*****************     Parser States             *************************/
%s PARSE
%s PARSE_CELL
%s PARSE_CHARS
%s PARSE_STRING
%s PARSE_ESCAPES
%s PARSE_ERROR
%%

 /*****************     Setup Code                *************************/
%{
/*
{zsection_tag}          {	// create a new section 
                             //ZLexFileSection* gl_pZLexFileSection = new ZLexFileSection(zHdr_text);
                             gl_pZLexFileSection = new ZLexFileSection(zHdr_text);
														 gl_pZLexFile->addSection(gl_pZLexFileSection,false);
													// stay in this state 
													// return to caller 
														 return (LINE_NUMBER);
												}									 
{zentry_tag}            {	// create a new section 
                             //ZLexEntry* gl_pZLexEntry = new ZLexEntry(zHdr_text);
                             gl_pZLexEntry = new ZLexEntry(zHdr_text);
                             gl_pZLexFileSection->addEntry(gl_pZLexEntry,false);
												     BEGIN PARSE_CELL;
													// return to caller 
														 return (LINE_NUMBER);
												}
*/
	if (gl_nLexFirstTime) 
	{
    //set the first time flag
		gl_nLexFirstTime  = false;
		
		/* make sure file or buffer is set at the beginning */
		/* this works on files, not buffers */
		//zHdr_restart(zHdr_in);
		zHdr_debug = 1;

		/* create a new section */
		gl_pZLexFileSection = new ZLexFileSection("<header>");
		gl_pZLexFile->addSection(gl_pZLexFileSection,false);

		/* create a new entry */
		gl_pZLexEntry = new ZLexEntry("<fields>");
		gl_pZLexFileSection->addEntry(gl_pZLexEntry,false);
		
		/* get ready: => set the initial state */
		BEGIN PARSE;
	}
%}
 /*****************     General Shared Rules      *************************/
<<EOF>>                 { /* end of file */


							 /* return EOF */
                             return END_OF_FILE; 
												}
{carriage}							{	/* ignore */
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
														 BEGIN PARSE;
												}

 /*****************     Initial Start State       *************************/
<PARSE>
{
{any_char_first}        {	/* first character of a line */
                             gl_nLexColumnCount++;     // increment number of columns on this line
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // increment number of chars on this line 
                          /* push back token chars, keep (0) token chars */
													   yyless(0);
													/* transition to PARSE_CELL state */
														 BEGIN PARSE_CELL;
													/* return to caller */
														 return (LINE_NUMBER);															
											  } 
{delimiter}             {	/* first character of a line must be a leading tab */
                             gl_nLexColumnCount++;     // increment number of columns on this line
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // increment number of chars on this line 
                          /* transition to PARSE_CELL state */
														 BEGIN PARSE_CELL;
													/* return to caller */
														 return (DELIMITER);															
												}

{newline}								{	/* first character is a new-line */
                             gl_nLexLineCount++;       // increment number of lines in file 
														 gl_nLexRowCount++;        // increment number of lines in section 
                             gl_nLexColumnCount=0;     // reset number of columns on this line
                             gl_nLexFieldCount=0;      // reset number of fields on this line
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset=0;  // reset number of chars on this line 
													/* stay in this state */
													/* return to caller */
														 return (NEW_LINE);
												}

{comment}               {	/* line is a comment new-line */
                             gl_nLexLineCount++;       // increment number of lines in file 
														 gl_nLexRowCount++;        // increment number of lines in section 
                             gl_nLexColumnCount=0;     // reset number of columns on this line
                             gl_nLexFieldCount=0;      // reset number of fields on this line
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset=0;  // reset number of chars on this line 
													/* stay in this state */
													/* return to caller */
														 return (COMMENT_LINE);
												}
. 											{	/* this is an error, unexpected char */
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // increment number of chars on this line 
													/* start collecting error chars in new state */
 														 yymore();
														 BEGIN PARSE_ERROR;
												}	                
}

 /*****************     Exclusive State           ***********************/
<PARSE_CELL>
{
{delimiter}						  {	/* delimiter found */
                             gl_nLexColumnCount++;     // reset number of columns on this line
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // count number of chars on this line 
													/* stay in this state */
														 return (DELIMITER);	
												}
{newline}								{	/* new line found */
                             gl_nLexLineCount++;       // count number of lines in file 
														 gl_nLexRowCount++;        // count number of lines in section 
                             gl_nLexColumnCount=0;     // reset number of columns on this line
                             gl_nLexFieldCount=0;      // reset number of fields on this line
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset=0;  // count number of chars on this line 
													/* transition to initial start state */
														 BEGIN PARSE;
														 return (NEW_LINE);
												}
{quote_double}					{	/* beginning of quoted string processing */
 														 yymore();
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset=0;  // count number of chars on this line 
													/* transition to PARSE_STRING state */
														 BEGIN PARSE_STRING;
												}
.												{	/* beginning of char cell (any char) found */
 														 yymore();
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // count number of chars on this line 
													/* transition to PARSE_CHARS state */
														 BEGIN PARSE_CHARS;
												}

}
 /*****************     Exclusive State           ***********************/
<PARSE_CHARS>
{
{delimiter}						  { /* transition back to PARSE_CELL */
                             //send the token over to bison
														 //int len1 = strlen(zHdr_text);
														 //save the tab char ?
														 //yyless(strlen(zHdr_text) - 1);
														 //int len2 = strlen(zHdr_text);
														 ZLexToken* newtok = 	new ZLexToken( 
																 zHdr_text,                         /* scanned text of token         			 */   		 
																 gl_nLexLineCount,                   /* line number from file                */
                                 gl_nLexRowCount,                    /* row number from section              */
                                 gl_nLexColumnCount,                 /* column number                        */
                                 gl_nLexLineCharOffset,              /* char offset from beginning of line   */								 
                                 gl_nLexFileCharOffset,              /* char offset from beginning of file   */								 
                                 LEXTOKEN_FIELDNAME,                 /* id, value, field,...                 */ 
																 LEXTOKEN_DATATYPE_DEFAULT,          /* datatype representation              */                     
																 LEXSCAN_COMPLETE,                   /* scan status                          */
																 gl_pLexCurrentToken);															
														 //InsertLexToken(gl_pLexCurrentToken);
                             ZLexElement* newelmnt = 	new ZLexElement(newtok);
														 gl_pZLexEntry->addElement(newelmnt,false);
														  
														 gl_pLexCurrentToken->next_ = newtok;
														 gl_pLexCurrentToken = newtok;
														 zHdr_lval.stok = gl_pLexCurrentToken;

                             gl_nLexColumnCount++;     // reset number of columns on this line
                             gl_nLexFieldCount++;      // increment number of fields on this line
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // increment number of chars on this line 

                             //start on next field
														 BEGIN PARSE_CELL;

														 //send this token to parser
														 return (ALPHA_CELL);
												}
{newline}								{ /* end of line, no more fields on this line */
                          /* save acumulated text as varchar */
														 // save the newline char 
														 //yyless(strlen(zHdr_text) - 1);

														 ZLexToken* newtok = 	new ZLexToken( 
																 zHdr_text,                         /* scanned text of token         			 */   		 
																 gl_nLexLineCount,                   /* line number from file                */
                                 gl_nLexRowCount,                    /* row number from section              */
                                 gl_nLexColumnCount,                 /* column number                        */
                                 gl_nLexLineCharOffset,              /* char offset from beginning of line   */								 
                                 gl_nLexFileCharOffset,              /* char offset from beginning of file   */								 
                                 LEXTOKEN_FIELDNAME,                 /* id, value, field,...                 */ 
																 LEXTOKEN_DATATYPE_DEFAULT,          /* datatype representation              */                     
																 LEXSCAN_COMPLETE,                   /* scan status                          */
																 gl_pLexCurrentToken);															
														 //InsertLexToken(gl_pLexCurrentToken);

                             ZLexElement* newelmnt = 	new ZLexElement(newtok);
														 gl_pZLexEntry->addElement(newelmnt,false);

														 gl_pLexCurrentToken->next_ = newtok;
														 gl_pLexCurrentToken= newtok;
														 zHdr_lval.stok = gl_pLexCurrentToken;

                             gl_nLexLineCount++;       // count number of lines in file 
														 gl_nLexRowCount++;        // count number of lines in section 
                             gl_nLexColumnCount=0;     // reset number of columns on this line
                             gl_nLexFieldCount=0;      // reset number of fields on this line
														 gl_nLexLineCharOffset=0;
														 gl_nLexFileCharOffset++;

                             //start on new line, checks for EOF
														 BEGIN PARSE_CELL;

														 //send this token to parser
													/* return to caller */
														 return (NEW_LINE);
												}
{space} 								{	/* this is an error, blanks only allowed within quotes */
                             gl_nLexFileCharOffset++;  // increment number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // increment number of chars on this line 
													/* start collecting error chars in new state */
 														 yymore();
														 BEGIN PARSE_ERROR;
														 //send this token to parser
														 return (ERROR_CELL);
												}
.		                    { /* Proceed adding one more character to zHdr_text. */
					                   gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
														 yymore();
				                }
}

 /*****************     Exclusive State           ***********************/
<PARSE_STRING>
{ 
\\			                { /* A '\\' marks the start of an escape sequence within a string */
									        /* Start ESCAPE mode. */
							               yyless(0);
														 //gl_nLexLineCharOffset += zHdr_leng;
														 //gl_nLexFileCharOffset += zHdr_leng;
														 BEGIN PARSE_ESCAPES;
						            }													
{quote_double}	        { /* End of string found => good accept this. */
 														 gl_nLexFieldCount++;
														 ZLexToken* newtok = 	new ZLexToken( 
																 zHdr_text,                         /* scanned text of token         			 */   		 
																 gl_nLexLineCount,                   /* line number from file                */
                                 gl_nLexRowCount,                    /* row number from section              */
                                 gl_nLexColumnCount,                 /* column number                        */
                                 gl_nLexLineCharOffset,              /* char offset from beginning of line   */								 
                                 gl_nLexFileCharOffset,              /* char offset from beginning of file   */								 
                                 LEXTOKEN_FIELDVALUE,                /* id, value, field,...                 */ 
																 LEXTOKEN_DATATYPE_BLOB,             /* datatype representation              */                     
																 LEXSCAN_COMPLETE,                   /* scan status                          */
																 gl_pLexCurrentToken);															
														 //InsertLexToken(gl_pLexCurrentToken);

                             ZLexElement* newelmnt = 	new ZLexElement(newtok);
														 gl_pZLexEntry->addElement(newelmnt,false);

														 gl_pLexCurrentToken->next_ = newtok;
														 gl_pLexCurrentToken= newtok;

														 zHdr_lval.stok = gl_pLexCurrentToken;

														 gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
						                 // return "" with empty strings
					                   if (zHdr_leng == 2)
					                   {
														  /* empty string */
				                     }
				                     else
					                   {
					                   }
														 /* Back to PARSE_CELL state. */
														 BEGIN PARSE_CELL;
					                   return (QUOTED_CELL);
				                }
{delimiter}             { /* delimiters are permitted within quotes */
                          /* so don't increment the column count */
						              /* Proceed adding one character to zHdr_text. */
					                   gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
														 yymore();
				                }
{newline}               { /* new lines are permitted within quotes */
                          /* so don't increment the line count */
						              /* Proceed adding one character to zHdr_text. */
					                   gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
														 yymore();
				                }
.		                    { /* Proceed adding one more character to zHdr_text. */
					                   gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
														 yymore();
				                }
}

 /*****************     Exclusive State           ***********************/
<PARSE_ESCAPES>
{
{escape_char}	          {	/* start escape char */
							               yymore();
                             gl_nLexFileCharOffset+= zHdr_leng;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset+= zHdr_leng;  // count number of chars on this line 
								          /* Back to string state */
							               BEGIN PARSE_STRING;
						            }
.				                {	/* one unexpected "escaped" character */
						              /* Proceed adding one character to zHdr_text. */
														 yymore();
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // count number of chars on this line 
								          /* Back to string state. */
							               BEGIN PARSE_STRING;
						            }
}
 /*****************     Exclusive State           ***********************/
<PARSE_ERROR>
{
{delimiter}						  { /* transition back to PARSE_CELL after posting an error*/
                             //send the token over to bison
														 //int len1 = strlen(zHdr_text);
														 //save the tab char ?
														 //yyless(strlen(zHdr_text) - 1);
														 //int len2 = strlen(zHdr_text);
														 ZLexToken* newtok = 	new ZLexToken( 
																 zHdr_text,                         /* scanned text of token         			 */   		 
																 gl_nLexLineCount,                   /* line number from file                */
                                 gl_nLexRowCount,                    /* row number from section              */
                                 gl_nLexColumnCount,                 /* column number                        */
                                 gl_nLexLineCharOffset,              /* char offset from beginning of line   */								 
                                 gl_nLexFileCharOffset,              /* char offset from beginning of file   */								 
                                 LEXTOKEN_FIELDNAME,                 /* id, value, field,...                 */ 
																 LEXTOKEN_DATATYPE_DEFAULT,          /* datatype representation              */                     
																 LEXSCAN_COMPLETE,                   /* scan status                          */
																 gl_pLexCurrentToken);															
														 //InsertErrorToken(gl_pLexErrorToken);

                             ZLexElement* newelmnt = 	new ZLexElement(newtok);
														 gl_pZLexEntry->addElement(newelmnt,false);

														 gl_pLexCurrentToken->next_ = newtok;
														 gl_pLexCurrentToken= newtok;
														 zHdr_lval.stok = gl_pLexCurrentToken;

                             //gl_nLexLineCount++;       // count number of lines in file 
														 //gl_nLexRowCount++;        // count number of lines in section 
                             gl_nLexColumnCount++;     // reset number of columns on this line
                             //gl_nLexFieldCount=0;      // reset number of fields on this line
                             gl_nLexFileCharOffset++;  // count number of chars scanned in this file 
                             gl_nLexLineCharOffset++;  // count number of chars on this line 

                             //continue parsing, start on next field
														 BEGIN PARSE_CELL;

														 //send this token to parser
														 return (ERROR_CELL);
												}
{newline}								{ /* end of line, no more fields on this line */
                          /* save acumulated text as varchar */
														 // save the newline char 
														 //yyless(strlen(zHdr_text) - 1);
														 ZLexToken* newtok = 	new ZLexToken( 
																 zHdr_text,                         /* scanned text of token         			 */   		 
																 gl_nLexLineCount,                   /* line number from file                */
                                 gl_nLexRowCount,                    /* row number from section              */
                                 gl_nLexColumnCount,                 /* column number                        */
                                 gl_nLexLineCharOffset,              /* char offset from beginning of line   */								 
                                 gl_nLexFileCharOffset,              /* char offset from beginning of file   */								 
                                 LEXTOKEN_FIELDNAME,                 /* id, value, field,...                 */ 
																 LEXTOKEN_DATATYPE_DEFAULT,          /* datatype representation              */                     
																 LEXSCAN_COMPLETE,                   /* scan status                          */
																 gl_pLexCurrentToken);															
														 //InsertErrorToken(gl_pLexErrorToken);

                             ZLexElement* newelmnt = 	new ZLexElement(newtok);
														 gl_pZLexEntry->addElement(newelmnt,false);

														 gl_pLexCurrentToken->next_ = newtok;
														 gl_pLexCurrentToken= newtok;

														 zHdr_lval.stok = gl_pLexCurrentToken;

                             gl_nLexLineCount++;       // count number of lines in file 
														 gl_nLexRowCount++;        // count number of lines in section 
                             gl_nLexColumnCount=0;     // reset number of columns on this line
                             gl_nLexFieldCount=0;      // reset number of fields on this line
														 gl_nLexLineCharOffset=0;
														 gl_nLexFileCharOffset++;

                             //continue parsing, start on new line, checks for EOF
														 BEGIN PARSE_CELL;

														 //send this token to parser
														 return (ERROR_CELL);
												}
.		                    { /* Proceed adding one more character to zHdr_text. */
					                   gl_nLexLineCharOffset++;
														 gl_nLexFileCharOffset++;
														 yymore();
				                }
}

%%

 /***************************************************************************/
