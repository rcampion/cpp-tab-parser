/**************************************************************************
 *
 * zLexElement.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexElement.cpp
*/

#include "zLexElement.h"
#include "zLexUtil.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <string>
#include <sstream>  

/***************************************************************************
*
* ZLexElement //default contructor
*
***************************************************************************/
ZLexElement::ZLexElement() {
	m_nId         = 0;                           // id
	m_szText      = NULL;                        // scanned text of token
	m_szName      = NULL;                        // field name
	m_nLength     = 0;                           // length of scanned token
	m_nLine       = 0;                           // line number
	m_nRow        = 0;                           // line number from section
	m_nColumn     = 0;                           // column number	
	m_nLineOffset = 0;                           // offset from beginning of line
	m_nFileOffset = 0;                           // offset from beginning of file
	m_eDataType   = LEXTOKEN_DATATYPE_STRING;    // datatype
	m_eTokenType  = LEXTOKEN_TYPE_UNDEFINED;     // lexer token type
	m_eStatus     = LEXSCAN_UNKNOWN;             // scan status
	m_pValue      = NULL;                        // mapped value token
	m_pParent     = NULL;                        // does this token belong to some other token?
	m_pChild      = NULL;                        // is there a child of this token?
	m_pNext       = NULL;                        // next token
	m_pPrev       = NULL;                        // previous token
}

/***************************************************************************
*
* ZLexElement  //destructor
*
***************************************************************************/
ZLexElement::~ZLexElement() {
  if(m_szText != NULL)
    delete []m_szText;

  m_szText = NULL;

  if(m_szName != NULL)
    delete []m_szName;

  m_szName = NULL;
}

/***************************************************************************
*
* ZLexElement  //contructor via ZLexToken pointer
*
***************************************************************************/
ZLexElement::ZLexElement(const ZLexToken* tok) {
	//token name
	if ( (tok->name_ != NULL) && (tok->name_[0] != '\0') ) {
		m_szName = new char[strlen(tok->name_) + 1];
		if (m_szName != NULL) {
			strcpy(m_szName, tok->name_);
		}
	}
	else m_szName = NULL;

	//token text
	if ( (tok->text_ != NULL) && (tok->text_[0] != '\0') ) {
		m_szText = new char[strlen(tok->text_) + 1];
		if (m_szText != NULL) {
			strcpy(m_szText, tok->text_);
		}
	}
	else m_szText = NULL;

	m_nId         = 0;                              // id
	m_nLength     = tok->length_;		                // length of scanned tok
	m_nLine       = tok->line_;                     // line number
	m_nRow        = tok->row_;                      // line number from section
	m_nColumn     = tok->column_;                   // column number	
	m_nLineOffset = tok->lineOffset_;	              // offset from beginning of line
	m_nFileOffset = tok->fileOffset_;	              // offset from beginning of filE

	m_eDataType   = (LexDataType) tok->dataType_;   // datatype
	m_eTokenType  = (LexTokenType) tok->tokenType_;	// lexer token type
	m_eStatus     = (LexScanStatus) tok->status_;   // scan status
	m_pValue      = (ZLexElement*) NULL;            // mapped value token
	m_pParent     = (ZLexElement*) NULL;            // does this token belong to some other token?
	m_pChild      = (ZLexElement*) NULL;            // child token
	m_pNext       = (ZLexElement*) NULL;            // next token scanned
	m_pPrev       = (ZLexElement*) NULL;            // previous token scanned
}

/***************************************************************************
*
* ZLexElement //overload assignment operator
*
***************************************************************************/
ZLexElement& ZLexElement::operator= (const ZLexElement& rhs) {

	//token name
	if ( (rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0') ) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	}
	else m_szName = NULL;

	//token text
	if ( (rhs.m_szText != NULL) && (rhs.m_szText[0] != '\0') ) {
		m_szText = new char[strlen(rhs.m_szText) + 1];
		if (rhs.m_szText != NULL) {
			strcpy(m_szText, rhs.m_szText);
		}
	}
	else m_szText = NULL;

	m_nId          = rhs.m_nId;         // id
	m_nLength      = rhs.m_nLength;     // length of scanned token
	m_nLine        = rhs.m_nLine;       // line number
	m_nRow         = rhs.m_nRow;        // line number from section
	m_nColumn      = rhs.m_nColumn;     // column number
	m_nLineOffset  = rhs.m_nLineOffset; // offset from beginning of line
	m_nFileOffset  = rhs.m_nFileOffset; // offset from beginning of file
	m_eDataType    = rhs.m_eDataType;   // datatype
	m_eTokenType   = rhs.m_eTokenType;  // lexer token type
	m_eStatus      = rhs.m_eStatus;     // scan status
	m_pValue       = rhs.m_pValue;      // mapped value token
	m_pParent      = rhs.m_pParent;     // parent
	m_pChild       = rhs.m_pChild;      // child
	m_pNext        = rhs.m_pNext;       // next token
	m_pPrev        = rhs.m_pPrev;       // previous token

	return *this;
}

/***************************************************************************
*
* ZLexElement  //copy contructor
*
***************************************************************************/
ZLexElement::ZLexElement(const ZLexElement& rhs) {

	//element name
	if ( (rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0') ) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	}
	else m_szName = NULL;

	//element text
	if ( (rhs.m_szText != NULL) && (rhs.m_szText[0] != '\0') ) {
		m_szText = new char[strlen(rhs.m_szText) + 1];
		if (rhs.m_szText != NULL) {
			strcpy(m_szText, rhs.m_szText);
		}
	}
	else m_szText = NULL;

	m_nId          = rhs.m_nId;         // id
	m_nLength      = rhs.m_nLength;     // length of scanned token
	m_nLine        = rhs.m_nLine;       // line number
	m_nRow         = rhs.m_nRow;        // line number from section
	m_nColumn      = rhs.m_nColumn;     // column number
	m_nLineOffset  = rhs.m_nLineOffset; // offset from beginning of line
	m_nFileOffset  = rhs.m_nFileOffset; // offset from beginning of file
	m_eDataType    = rhs.m_eDataType;   // datatype
	m_eTokenType   = rhs.m_eTokenType;  // lexer token type
	m_eStatus      = rhs.m_eStatus;     // scan status
	m_pValue       = rhs.m_pValue;      // mapped value token
	m_pParent      = rhs.m_pParent;     // parent
	m_pChild       = rhs.m_pChild;      // child
	m_pNext        = rhs.m_pNext;       // next token
	m_pPrev        = rhs.m_pPrev;       // previous token
}

/***************************************************************************
*
* GetText
*
***************************************************************************/
StatusCode ZLexElement::getText(char szText[] /* out */,int nBufferSize) {
	StatusCode eStatus = E_NoError;
	szText[0] = '\0';

  if(m_szText == NULL)
    return E_ValueNotFound;

	if ((int)strlen(m_szText) < nBufferSize) {
		strcpy(szText,m_szText);
	}
	else {
		eStatus = E_BufferOverflow;
	}

	return eStatus;
}

/***************************************************************************
*
* GetName
*
***************************************************************************/
StatusCode ZLexElement::getName(char szName[] /* out */,int nBufferSize) {
	StatusCode eStatus = E_NoError;
	szName[0] = '\0';

  if(m_szName == NULL)
    return E_ValueNotFound;

	if ((int)strlen(m_szName) < nBufferSize) {
		strcpy(szName,m_szName);
	}
	else {
		eStatus = E_BufferOverflow;
	}

	return eStatus;
}

/***************************************************************************
*
* GetValueText
*
***************************************************************************/
StatusCode ZLexElement::getValueText(char szText[] /* out */,int nBufferSize) {
	StatusCode eStatus = E_NoError;
	szText[0] = '\0';

	if (m_pValue != NULL) {
		eStatus = m_pValue->getText(szText,nBufferSize);
	}

	return eStatus;
}

/***************************************************************************
*
* setText
*
***************************************************************************/
int ZLexElement::setText(const char *szText) {
  if(m_szText != NULL) {
    delete m_szText;
		m_szText = NULL;
	}

	if ((szText != NULL) && (szText[0] != '\0') ) {
		m_szText = new char[strlen(szText) + 1];
		if (m_szText != NULL) {
			strcpy(m_szText, szText);
		}
	}
	else
    m_szText = NULL;

	return 0;
}

/***************************************************************************
*
* setName
*
***************************************************************************/
int ZLexElement::setName(const char *szName) {
  if(m_szName != NULL) {
    delete m_szName;
		m_szName = NULL;
	}

	if ((szName != NULL) && (szName[0] != '\0') ) {
		m_szName = new char[strlen(szName) + 1];
		if (m_szName != NULL) {
			strcpy(m_szName, szName);
		}
	}
	else
    m_szName = NULL;

	return 0;
}

/***************************************************************************
*
* setNext
*
***************************************************************************/
void ZLexElement::setNextPtr(ZLexElement* pNext) {
  if(pNext != NULL)
    m_pNext = pNext;
  else
    m_pNext = NULL;
}

void ZLexElement::setPrevPtr(ZLexElement* pPrev) {
  if(pPrev != NULL)
    m_pPrev = pPrev;
  else
    m_pPrev = NULL;
}

void ZLexElement::setParentPtr(ZLexElement* pParent) {
  if(pParent != NULL)
    m_pParent = pParent;
  else
    m_pParent = NULL;
}

void ZLexElement::setChildPtr(ZLexElement* pChild) {
  if(pChild != NULL)
    m_pChild = pChild;
  else
    m_pChild = NULL;
}

void ZLexElement::setValuePtr(ZLexElement* pValue) {
  if(pValue != NULL)
    m_pValue = pValue;
  else
    m_pValue = NULL;
}

/***************************************************************************
*
* toXmlSchema()
*
***************************************************************************/
const std::string ZLexElement::toXmlSchema() {

	char cDesc[1024];
	char cName[1024];
	char cText[1024];
	bool bReadNext = true;

	std::ostringstream streamOut;

	//format a xml schema column
	if(this->getTokenType() == LEXTOKEN_FIELDNAME) {
		
		GetLexTokenDataTypeDesc(this->getDataType(), cDesc,sizeof(cDesc)-1);
		//this->getName(cName,sizeof(cName)-1);
		getName(cName,sizeof(cName)-1);
		this->getText(cText,sizeof(cText)-1);
		
		//to print size or not print size, that is the question,...whether tis nobler...
		if(	(this->getDataType() == LEXTOKEN_DATATYPE_BLOB)
			||(this->getDataType() == LEXTOKEN_DATATYPE_INTEGER) ) {
			//print the line without the size attribute
			//<column name="Number" required="false" type="INTEGER" description="Number" />

			streamOut << "<\t\t\tcolumn name=\"" << cName << "required=false\"" <<
				"type=\"" << cDesc << "\"description=\"" << cText << "\"";

		}
		else {
			//print the line with the size attribute
			streamOut << "<\t\t\tcolumn name=\"" << cName << "required=\"false\"" <<
				" type=\"" << cDesc << "\" size=\"" << this->getLength() 
				<< "\" description=\"" << cText << "\"";

		}
	}
	return streamOut.str();
}
