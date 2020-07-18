/**************************************************************************
 *
 * zLexElement.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexElement.h
*/

#ifndef ___ZLEXELEMENT_H___
#define ___ZLEXELEMENT_H___

#include "zDwConst.h"
#include "zLexToken.h"
#include <string>

/***************************************************************************
*
* ZLexElement
*
***************************************************************************/
class ZLexElement {
public:
	ZLexElement();                                  // default constructor
	ZLexElement(const ZLexToken* t);                // construct using lexer token
	ZLexElement(const ZLexElement& rhs);            // copy contructor
	ZLexElement& operator= (const ZLexElement& rhs);// assignment operator
	~ZLexElement();				                    // destructor

	//constant member functions
	int            getId()                        { return m_nId;   }       // id
	StatusCode getText(char szText[] /* out */,int nBufferSize1 /* in */);
	StatusCode getName(char szName[] /* out */,int nBufferSize1 /* in */);
	int getColumn()                               { return m_nColumn;    }

	StatusCode     getValueText(char szText[] /* out */,int nBufferSize1 /* in */);
	int            getLength() const              { return m_nLength;    }  // length of scanned token
	int            getLineNo() const              { return m_nLine;      }  // line number
	int            getLineOffset() const          { return m_nLineOffset;}  // offset from beginning of line
	int            getFileOffset() const          { return m_nFileOffset;}  // offset from beginning of file
	LexDataType    getDataType() const            { return m_eDataType;  }  // datatype
	LexTokenType   getTokenType() const           { return m_eTokenType; }  // lexer token type
	LexScanStatus  getStatus() const              { return m_eStatus;    }  // scan status
	const std::string toXmlSchema();

	ZLexElement*   getValuePtr() const            { return m_pValue;     }
	ZLexElement*   getParentPtr() const           { return m_pParent;    }
	ZLexElement*   getChildPtr() const            { return m_pChild;     }
	ZLexElement*   getNextPtr() const             { return m_pNext;      }
	ZLexElement*   getPrevPtr() const             { return m_pPrev;      }

	void           setNextPtr(ZLexElement* pNext);
	void           setPrevPtr(ZLexElement* pPrev);
	void           setParentPtr(ZLexElement* pParent);
	void           setChildPtr(ZLexElement* pChild);
	void           setValuePtr(ZLexElement* pValue);
	int            setText(const char* szText);
	int            setName(const char* szName);
	void           setLength(int nLen) {m_nLength = nLen;};

private:
	int            m_nId;            // id
	char*          m_szName;         // field name
	char*          m_szText;         // scanned text of token
	int            m_nLength;        // length of scanned token
	int            m_nLine;          // line number
	int            m_nRow;           // line number from section
	int            m_nColumn;        // column number
	int            m_nLineOffset;    // offset from beginning of line
	int            m_nFileOffset;    // offset from beginning of file

	LexDataType	   m_eDataType;      // datatype
	LexTokenType   m_eTokenType;     // lexer token type
	LexScanStatus  m_eStatus;        // scan status
	ZLexElement*   m_pValue;         // mapped value token
	ZLexElement*   m_pParent;        // parent
	ZLexElement*   m_pChild;         // child
	ZLexElement*   m_pNext;          // next token
	ZLexElement*   m_pPrev;          // previous token
};

#endif //___ZLEXELEMENT_H___
