/**************************************************************************
 *
 * zLexToken.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexToken.cpp
*/
#include "zLexToken.h"
#include "zLexUtil.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

/***************************************************************************
 *
 * default constructor
 *
 ***************************************************************************/
ZLexToken::ZLexToken() {
	name_ = NULL;
	text_ = NULL;
	
	//bitwise copy
	length_    = 0;          // length of scanned text											 
	line_      = 0;          // line number from file
	row_       = 0;          // line number from section
	column_    = 0;          // column number
	lineOffset_= 0;	         // offset from beginning of line								 
	fileOffset_= 0;	         // offset from beginning of file								          
	tokenType_ = LEXTOKEN_TYPE_UNDEFINED;    // what is this symbol_ id_ value_ operator 
	dataType_  = LEXTOKEN_DATATYPE_UNDEFINED;// datatype representation                      
	status_    = LEXSCAN_UNKNOWN;            // scan status

	//initialize 
	value_     = NULL;       // mapped token to some other token     
	parent_    = NULL;       // relationship to other token  
	child_     = NULL;       // child of this token_ map                    
	next_      = NULL;       // next token scanned                           
	prev_      = NULL;       // previous token scanned 
}

/***************************************************************************
 *
 * copy constructor
 *
 ***************************************************************************/
ZLexToken::ZLexToken(const ZLexToken& rhs) {
	//deep copy
	//token name
	if ( (rhs.name_ != NULL) && (rhs.name_[0] != '\0') ) {
		name_ = new char[strlen(rhs.name_) + 1];
		if (rhs.name_ != NULL) {
			strcpy(name_, rhs.name_);
		}
	}
	else name_ = NULL;

	//token text
	if ( (rhs.text_ != NULL) && (rhs.text_[0] != '\0') ) {
		text_ = new char[strlen(rhs.text_) + 1];
		if (rhs.text_ != NULL) {
			strcpy(text_, rhs.text_);
		}
	}
	else text_ = NULL;     				   

	//bitwise copy
	length_    = rhs.length_;      // length of scanned text											 
	line_      = rhs.line_;        // line number from file
	row_       = rhs.row_;         // line number from section
	column_    = rhs.column_;      // column number
	lineOffset_= rhs.lineOffset_;	 // offset from beginning of line								 
	fileOffset_= rhs.fileOffset_;	 // offset from beginning of file								          
	tokenType_ = rhs.tokenType_;   // what is this symbol_ id_ value_ operator 
	dataType_  = rhs.dataType_;    // datatype representation                      
	status_    = rhs.status_;      // scan status

	//initialize 
	value_     = rhs.value_;       // mapped token to some other token     
	parent_    = rhs.parent_;      // relationship to other token  
	child_     = rhs.child_;       // child of this token_ map                    
	next_      = rhs.next_;        // next token scanned                           
	prev_      = rhs.prev_;        // previous token scanned 
}

/***************************************************************************
 *
 * assignment operator
 *
 ***************************************************************************/
ZLexToken& ZLexToken::operator=(const ZLexToken& rhs) {
	//deep copy
	//token name

	//check for self-assignment
	if(this == &rhs)
		return *this;

	if ( (rhs.name_ != NULL) && (rhs.name_[0] != '\0') ) {
		name_ = new char[strlen(rhs.name_) + 1];
		if (rhs.name_ != NULL) {
			strcpy(name_, rhs.name_);
		}
	}
	else name_ = NULL;

	//token text
	if ( (rhs.text_ != NULL) && (rhs.text_[0] != '\0') ) {
		text_ = new char[strlen(rhs.text_) + 1];
		if (rhs.text_ != NULL) {
			strcpy(text_, rhs.text_);
		}
	}
	else text_ = NULL;     				   

	//bitwise copy
	length_    = rhs.length_;      // length of scanned text											 
	line_      = rhs.line_;        // line number from file
	row_       = rhs.row_;         // line number from section
	column_    = rhs.column_;      // column number
	lineOffset_= rhs.lineOffset_;	 // offset from beginning of line								 
	fileOffset_= rhs.fileOffset_;	 // offset from beginning of file								          
	tokenType_ = rhs.tokenType_;   // what is this symbol_ id_ value_ operator 
	dataType_  = rhs.dataType_;    // datatype representation                      
	status_    = rhs.status_;      // scan status

	//initialize 
	value_     = rhs.value_;       // mapped token to some other token     
	parent_    = rhs.parent_;      // relationship to other token  
	child_     = rhs.child_;       // child of this token_ map                    
	next_      = rhs.next_;        // next token scanned                           
	prev_      = rhs.prev_;        // previous token scanned
	
	return *this;
}

/***************************************************************************
 *
 * constructor
 *
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

 ***************************************************************************/
ZLexToken::ZLexToken( 

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
	) {

	int textlen = 0;
	//-------------------------------------------------------------
	//token text
	if ( (text != NULL) && (text[0] != '\0') ) {
		text_ = new char[strlen(text) + 1];
		if (text != NULL) {
			strcpy(text_, text);
		}
	}
	else text_ = NULL;
  
	//-------------------------------------------------------------
	//I can't believe I wrote this crap.
  char lineText[10];
  //itoa(line,lineText,10);
  snprintf (lineText, sizeof(lineText), "%d",10);
  char rowText[10];
  //itoa(row,rowText,10);
  snprintf (rowText, sizeof(rowText), "%d",10);
  char colText[10];
  //itoa(column,colText,10);
  snprintf (colText, sizeof(colText), "%d",10);

	std::string fieldname;
	fieldname = "_line";
	fieldname += lineText;
	fieldname += "_row";
	fieldname += rowText;
	fieldname += "_col";
	fieldname += colText;
	if(!fieldname.empty()) {
		//void StripChar(newtext, char* t,'\"');
		//RightTrim and StripChar need SAFE rewrites using buffer and size passed in
		//too lazy today 07/21/2005
		name_ = new char[fieldname.length() + 1];
		strcpy(name_,fieldname.c_str());
	}
	else {
		name_ = NULL;
	}

	//-------------------------------------------------------------
	// assign the remaining attribute values 							   				   										 
	length_     = strlen(text_);
	line_       = line;       // line number from file
	row_        = row;        // line number from section
	column_     = column;     // column number
	lineOffset_ = lineOffset; // offset from beginning of line								 
	fileOffset_ = fileOffset; // offset from beginning of file								 	
	dataType_   = dataType;   // datatype representation                      
	tokenType_  = tokenType;  // what is this thing? id, value, class       	 
	status_     = status;     // scan status
	value_      = NULL;       // mapped value of text     
	parent_     = parent;     // relationship to other token  
	child_      = NULL;       // child of this token, map                    
	next_       = NULL;       // next token scanned                           
	prev_       = NULL;       // previous token scanned                       
}

/***************************************************************************
 *
 * destructor
 *
 ***************************************************************************/
ZLexToken::~ZLexToken() {
	if(text_ != 0)
		//free(tok->text_,"DestroyLexToken");
		delete(text_);

	if(name_ != 0)
		//free(tok->name_,"DestroyLexToken");
		delete(name_);
}

/***************************************************************************
 *
 * CreateErrorToken
 *
 ***************************************************************************/
/*
 ZLexToken* ZErrorToken::CreateErrorToken(ZLexToken* tok, LexError err) {
	 ZErrorToken zerrTok = new ZErrorToken(tok,err);
	 //tok->error_      = E_LEX_OK;   // error index of LexError message
}
*/


/***************************************************************************
 *
 * DestroyErrorToken
 *
 ***************************************************************************/
StatusCode DestroyErrorToken(ZLexToken* tok) {
	if(tok->text_ != 0)
		//free(tok->text_,"DestroyLexToken");
		free(tok->text_);

	if(tok->name_ != 0)
		//free(tok->name_,"DestroyLexToken");
		free(tok->name_);
	tok = 0;

	return E_NoError;
}

/***************************************************************************
 *
 * GetLexTokenDataTypeDesc
 *
 ***************************************************************************/
int	GetLexTokenDataTypeDesc(int nDesc, char szBuffer[], int nBufferSize) {
	if (nBufferSize < 100) return -1;
	szBuffer[0] = '\0';

	// handle special cases first
	if ( (nDesc >= LEXTOKEN_DATATYPE_MAX) && (nDesc <= LEXTOKEN_DATATYPE_MIN) ) {
		strcpy(szBuffer,"Token Datatype Description Error");
		return -1;
	}

	switch(nDesc) {
		case 	LEXTOKEN_DATATYPE_UNDEFINED: strcpy(szBuffer,"NULL");break;
		case 	LEXTOKEN_DATATYPE_TINY:      strcpy(szBuffer,"TINY");break;
		case 	LEXTOKEN_DATATYPE_SHORT:     strcpy(szBuffer,"SHORT");break;
		case 	LEXTOKEN_DATATYPE_LONG:      strcpy(szBuffer,"LONG");break;
		case 	LEXTOKEN_DATATYPE_INT24:     strcpy(szBuffer,"INT24");break;
		case 	LEXTOKEN_DATATYPE_LONGLONG:  strcpy(szBuffer,"LONGLONG");break;
		case 	LEXTOKEN_DATATYPE_DECIMAL:   strcpy(szBuffer,"DECIMAL");break;
		case 	LEXTOKEN_DATATYPE_FLOAT:     strcpy(szBuffer,"FLOAT");break;
		case 	LEXTOKEN_DATATYPE_DOUBLE:    strcpy(szBuffer,"DOUBLE");break;
		case 	LEXTOKEN_DATATYPE_TIMESTAMP: strcpy(szBuffer,"TIMESTAMP");break;
		case 	LEXTOKEN_DATATYPE_DATE:      strcpy(szBuffer,"DATE");break;
		case 	LEXTOKEN_DATATYPE_TIME:      strcpy(szBuffer,"TIME");break;
		case 	LEXTOKEN_DATATYPE_DATETIME:  strcpy(szBuffer,"DATETIME");break;
		case 	LEXTOKEN_DATATYPE_YEAR:      strcpy(szBuffer,"YEAR");break;
		case 	LEXTOKEN_DATATYPE_STRING:    strcpy(szBuffer,"STRING");break;
		case 	LEXTOKEN_DATATYPE_VAR_STRING:strcpy(szBuffer,"VAR_STRING");break;
		case 	LEXTOKEN_DATATYPE_BLOB:      strcpy(szBuffer,"BLOB");break;
		case 	LEXTOKEN_DATATYPE_SET:       strcpy(szBuffer,"SET");break;
		case 	LEXTOKEN_DATATYPE_ENUM:      strcpy(szBuffer,"ENUM");break;
		case 	LEXTOKEN_DATATYPE_NULL:      strcpy(szBuffer,"NULL");break;
		case 	LEXTOKEN_DATATYPE_TEXT:      strcpy(szBuffer,"TEXT");break;
		case 	LEXTOKEN_DATATYPE_CHAR:      strcpy(szBuffer,"CHAR");break;
		case 	LEXTOKEN_DATATYPE_VARCHAR:   strcpy(szBuffer,"VARCHAR");break;
		case 	LEXTOKEN_DATATYPE_DEFAULT:   strcpy(szBuffer,"VARCHAR");break;		
		default:
			strcpy(szBuffer,"VARCHAR");
			//PrintLexNode(tok,i);
	}		

	return 0;
}
