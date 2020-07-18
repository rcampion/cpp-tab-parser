/**************************************************************************
 *
 * zLexEntry.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexEntry.cpp
*/

#include "zLexEntry.h"
#include "zLexUtil.h"

#include <string.h>
#include <string>
#include <sstream>

ZLexEntry* gl_pZLexEntry = NULL;

/***************************************************************************
*
* ZLexEntry  //default contructor
*
***************************************************************************/
ZLexEntry::ZLexEntry() {
  m_nEntryId = 0;
	m_szName = NULL;
	m_pNextEntry = NULL;
	m_pPrevEntry = NULL;
	m_pElementList = NULL;
	m_nListSize = 0;
	m_eStatus = E_NoError;
	m_pCurrentElement = NULL;
}

/***************************************************************************
*
* reInit
*
***************************************************************************/
StatusCode ZLexEntry::reInit() {

  deleteElements();

  m_nEntryId = 0;
	m_szName = NULL;
	m_pNextEntry = NULL;
	m_pPrevEntry = NULL;
	m_pElementList = NULL;
	m_nListSize = 0;
	m_eStatus = E_NoError;
	m_pCurrentElement = NULL;

	return m_eStatus;
}

/***************************************************************************
*
* ZLexEntry //destructor
*
***************************************************************************/
ZLexEntry::~ZLexEntry() {
	m_nEntryId = 0;
	m_pNextEntry = NULL;

	deleteElements();

	if (m_szName)
		delete m_szName;

}

/***************************************************************************
 *
 * DeleteElements
 *
 ***************************************************************************/
StatusCode ZLexEntry::deleteElements() {
	ZLexElement *pLast, *pThis;
	pThis = m_pElementList;

	while (pThis != NULL) {
		pLast = pThis;
		pThis = pThis->getNextPtr();
		delete (pLast);
		pLast = NULL;
	}

	return E_NoError;
}

/***************************************************************************
*
* ZLexEntry //construct an entry from a string
*
***************************************************************************/
ZLexEntry::ZLexEntry(char* szName) {
	if ( (szName != NULL) && (szName[0] != '\0') ) {
		m_szName = new char[strlen(szName) + 1];
		if (m_szName != NULL) {
			strcpy(m_szName, szName);
		}
	}
	else m_szName = NULL;

	m_eStatus         = E_NoError;
	m_nEntryId        = 0;
	m_nListSize       = 0;
	m_pCurrentElement = NULL;
	m_pNextEntry      = NULL;
	m_pPrevEntry      = NULL;
	m_pElementList    = NULL;
}

/***************************************************************************
*
* ZLexEntry   //copy contructor
*
***************************************************************************/
ZLexEntry::ZLexEntry(const ZLexEntry &rhs) {
  m_nEntryId = rhs.m_nEntryId;
	if ( (rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0') ) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	}
	else m_szName = NULL;

	m_nEntryId        = rhs.m_nEntryId;
	m_nListSize       = rhs.m_nListSize;
	m_pCurrentElement = rhs.m_pCurrentElement;
	m_pNextEntry      = rhs.m_pNextEntry;
	m_pPrevEntry      = rhs.m_pPrevEntry;
	m_pElementList    = rhs.m_pElementList;
	m_eStatus         = rhs.m_eStatus;
}

/***************************************************************************
*
* ZLexEntry //overloaded assignment operator
*
***************************************************************************/
ZLexEntry& ZLexEntry::operator= (const ZLexEntry& rhs) {
	if(&rhs == this)
		return *this;

	if(m_szName) 
		delete m_szName;

	if ( (rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0') ) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	}
	else m_szName = NULL;

	m_nEntryId        = rhs.m_nEntryId;
	m_nListSize       = rhs.m_nListSize;
	m_pCurrentElement = rhs.m_pCurrentElement;
	m_pNextEntry      = rhs.m_pNextEntry;
	m_pPrevEntry      = rhs.m_pPrevEntry;
	m_pElementList    = rhs.m_pElementList;
	m_eStatus         = rhs.m_eStatus;
	
	return *this;
};

/***************************************************************************
*
* getName
*
***************************************************************************/
StatusCode ZLexEntry::getName(char szText[],int nBufferSize) const {
	StatusCode eStatus = E_NoError;
	szText[0] = '\0';

	if ((int)strlen(m_szName) < nBufferSize) {
		strcpy(szText,m_szName);
	}
	else {
		eStatus = E_BufferOverflow;
	}

	return eStatus;
}

/***************************************************************************
*
* getFirstElement
*
***************************************************************************/
ZLexElement* ZLexEntry::getFirstElement() { return getNextElement(true);};

/***************************************************************************
*
* getLastElement
*
***************************************************************************/
ZLexElement* ZLexEntry::getLastElement() {
	
	ZLexElement* elmnt = getFirstElement();

	while(elmnt->getNextPtr()!=NULL)
		elmnt = elmnt->getNextPtr();

	return elmnt;
};

/***************************************************************************
*
* getNextElement
*
***************************************************************************/
ZLexElement* ZLexEntry::getNextElement(bool bStart) {
	StatusCode eStatus = E_NoError;

	if (bStart) {
		m_pCurrentElement = m_pElementList;
		eStatus = E_NoError;
		return m_pCurrentElement;
	}

	if (m_pCurrentElement != NULL) {
		m_pCurrentElement = m_pCurrentElement->getNextPtr();
		eStatus = E_NoError;
	}
	else {
		eStatus = E_NoMoreEntries;
	}

	return m_pCurrentElement;
}

/***************************************************************************
 *
 * findElementByName
 *
 ***************************************************************************/
ZLexElement* ZLexEntry::findElementByIndex(int n, bool begin) {

	ZLexElement *tok = getFirstElement();

	/* sequential search */

	// traverse entry tokens
	while (tok != NULL) {
		if (tok->getId() == n) {
			return ((ZLexElement*) tok);
		}
		tok = getNextElement(false);
	}

	return ((ZLexElement*) NULL);
}

/***************************************************************************
 *
 * findElementByName
 *
 ***************************************************************************/
ZLexElement* ZLexEntry::findElementByName(const char *srch1, bool begin) {

	ZLexElement *elmnt = getFirstElement();

	char srch11[1028];
	char srch12[1028];
	char cName[1024];

	/* sequential search */

	// traverse entry elmntens
	while (elmnt != NULL) {
		if (elmnt->getTokenType() == LEXTOKEN_FIELDNAME) {
			elmnt->getName(cName, sizeof(cName) - 1);
			if (cName != NULL) {
				//convert to same case
				strcpy(srch11, srch1);
				lcase(srch11);
				strcpy(srch12, cName);
				lcase(srch12);
				if (strcmp(srch11, srch12) == 0) {
					return ((ZLexElement*) elmnt);
				}
			}
		}
		elmnt = getNextElement(false);
	}

	return ((ZLexElement*) NULL);
}

/***************************************************************************
*
* addElement
*
***************************************************************************/
StatusCode ZLexEntry::addElement(ZLexElement* elmnt, bool begin) {
	if (elmnt == NULL) return E_BadParameter;

	ZLexElement *pNew = elmnt;
	if (m_pElementList == NULL) {
		m_pElementList = pNew;
		m_nListSize = 1;
	}
	else {		
		if(begin) {
			//insert element at beginning of list;
			pNew->setNextPtr(m_pElementList);
			m_pElementList->setPrevPtr(pNew);
			m_pElementList = pNew;
			m_nListSize++;
		}
		else {
			//traverse the list to the end
			ZLexElement *pList = m_pElementList;
			while (pList->getNextPtr() != NULL) {
				pList = pList->getNextPtr();
			}
			pNew->setPrevPtr(pList);
			pList->setNextPtr(pNew);
			m_nListSize++;
		}
	}
	
	m_pCurrentElement = pNew;
	return E_NoError;
}

/***************************************************************************
*
* addElement
*
***************************************************************************/
StatusCode ZLexEntry::addElement(const ZLexElement& elmnt, bool begin) {
	//if (elmnt == NULL) return E_BadParameter;

	ZLexElement* pElementList = m_pElementList;
	ZLexElement* pNew = new ZLexElement(elmnt);
	//pNew = (ZLexElement*) elmnt;
	
	if (m_pElementList == NULL) {
		m_pElementList = pNew;
		m_nListSize = 1;
		m_pCurrentElement = pNew;
	}
	else {
		if(begin) {
			//insert entry at beginning of list;
			pNew->setNextPtr(pElementList);
			pElementList->setPrevPtr(pNew);
			m_pCurrentElement = pNew;
		}
		else {
			while (pElementList->getNextPtr() != NULL) {
				pElementList = pElementList->getNextPtr();
			}
			pNew->setPrevPtr(pElementList);
			pElementList->setNextPtr(pNew);
			//m_pCurrentElement = pNew;
			m_nListSize++;
		}
	}
	return E_NoError;
}

/***************************************************************************
 *
 * InsertElement
 * add a token after the referenced token
 ***************************************************************************/
StatusCode ZLexEntry::insertElement(ZLexElement *a_token, ZLexElement *a_pos) {
	if (a_pos == NULL)
		return E_BadParameter;
	if (a_token == NULL)
		return E_BadParameter;

	ZLexElement *pThis = m_pElementList;
	ZLexElement *pPrev = NULL;

	if (m_pElementList == NULL) {
		// first entry
		m_pElementList = a_token;
		m_nListSize = 1;
	} else {

		pPrev = a_pos->getPrevPtr();

		if (pPrev != NULL)
			pPrev->setNextPtr(a_token);

		a_token->setPrevPtr(pPrev);

		a_pos->setPrevPtr(a_token);

		a_token->setNextPtr(a_pos);

		m_nListSize++;

	}
	m_pCurrentElement = a_token;
	return E_NoError;
}

/***************************************************************************
 *
 * toXmlSchema()
 *
 ***************************************************************************/
const std::string ZLexEntry::toXmlSchema() const {
	std::ostringstream streamOut;

	ZLexElement *pElementList = m_pElementList;

	bool bReadNext = true;
	// traverse elements
	while (pElementList != NULL) {
		streamOut << pElementList->toXmlSchema();

		if (bReadNext) {
			pElementList = pElementList->getNextPtr();
		}
		bReadNext = true;
	}

	return streamOut.str();
}

/***************************************************************************
*
* CreateMockElement
*
***************************************************************************/
ZLexElement* ZLexEntry::createMockElement() {
	ZLexToken* testToken = 0;
	
	static int n = 0;

	testToken = new ZLexToken(
		"Test Element",              // scanned text of token         			          		 
		n,                           // line number from file                      
		n,                           // row number from section                    
		n,                           // column number                              
		n,	                         // offset from beginning of line              								 
		n,	                         // offset from beginning of file              								          
		LEXTOKEN_HEADERCELL,         // id, value, field,...                        
		LEXTOKEN_DATATYPE_VARCHAR,   // datatype representation                                         
		LEXSCAN_PENDING,             // scan status                                
		0                            // relationship to other token                  
	);

	n++;

	//Process
	//create one element on the heap from the token above 
	ZLexElement* element = new ZLexElement(testToken);
	//DestroyLexToken(testToken);
	delete testToken;
	testToken = 0;

	return element;
}

