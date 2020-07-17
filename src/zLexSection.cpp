/**************************************************************************
 *
 * zLexSection.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexSection.cpp
 */

class zLexEntry;
#include "zLexSection.h"

#include <string.h>
#include <string>
#include <sstream>  

ZLexFileSection *gl_pZLexFileSection = NULL;

/***************************************************************************
 *
 * ZLexFileSection   //default contructor
 *
 ***************************************************************************/
ZLexFileSection::ZLexFileSection() {
	m_szName = NULL;
	m_nSectionId = 0;
	m_pCurrentEntry = NULL;
	m_pEntryList = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_nListSize = 0;
	m_eStatus = E_NoError;

	m_nFileBeginOffset = 0;
	m_nFileEndOffset = 0;
}

/***************************************************************************
 *
 * ZLexFileSection  //contructor
 *
 ***************************************************************************/
ZLexFileSection::ZLexFileSection(char *szName) {

	if ((szName != NULL) && (szName[0] != '\0')) {
		m_szName = new char[strlen(szName) + 1];
		if (m_szName != NULL) {
			strcpy(m_szName, szName);
		}
	} else
		m_szName = NULL;

	m_nSectionId = 0;
	m_pCurrentEntry = NULL;
	m_pEntryList = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_nListSize = 0;
	m_eStatus = E_NoError;

	m_nFileBeginOffset = 0;
	m_nFileEndOffset = 0;
}

/***************************************************************************
 *
 * ZLexFileSection  //contructor
 *
 ***************************************************************************/
/*
 ZLexFileSection::ZLexFileSection(char* szName, ZLexEntry* entry)
 {
 if ( (szName != NULL) && (szName[0] != '\0') )
 {
 m_szName = new char[strlen(szName) + 1];
 if (m_szName != NULL)
 {
 strcpy(m_szName, szName);
 }
 }
 else m_szName = NULL;

 m_nSectionId++;
 m_pCurrentEntry = entry;
 m_pEntryList    = entry;
 m_pNext         = NULL;
 m_pPrev         = NULL;
 m_nListSize     = 0;
 m_eStatus       = E_NoError;

 m_nFileBeginOffset = entry->getFirstElement()->getFileOffset();
 m_nFileEndOffset   = entry->getLastElement()->getFileOffset();
 }
 */

/***************************************************************************
 *
 * ZLexFileSection   //copy contructor
 *
 ***************************************************************************/
ZLexFileSection::ZLexFileSection(const ZLexFileSection &rhs) {
	if ((rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0')) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	} else
		m_szName = NULL;

	m_pCurrentEntry = rhs.m_pCurrentEntry;
	m_pEntryList = rhs.m_pEntryList;
	m_nSectionId = rhs.m_nSectionId;
	m_pNext = rhs.m_pNext;
	m_pPrev = rhs.m_pPrev;
	m_nListSize = rhs.m_nListSize;
	m_eStatus = rhs.m_eStatus;

	m_nFileBeginOffset = rhs.m_nFileBeginOffset;
	m_nFileEndOffset = rhs.m_nFileEndOffset;
}

/***************************************************************************
 *
 * ZLexFileSection //destructor
 *
 ***************************************************************************/
ZLexFileSection::~ZLexFileSection() {
	m_nSectionId = 0;
	m_pNext = NULL;

	deleteEntries();

//  if(m_pEntryList)
//    delete m_pEntryList;

	if (m_szName)
		delete m_szName;
}

/***************************************************************************
 *
 * ZLexFileSection //overloaded assignment operator
 *
 ***************************************************************************/
ZLexFileSection& ZLexFileSection::operator=(const ZLexFileSection &rhs) {
	if (&rhs == this)
		return *this;

	m_nSectionId = rhs.m_nSectionId;

	if (m_szName)
		delete m_szName;

	if ((rhs.m_szName != NULL) && (rhs.m_szName[0] != '\0')) {
		m_szName = new char[strlen(rhs.m_szName) + 1];
		if (rhs.m_szName != NULL) {
			strcpy(m_szName, rhs.m_szName);
		}
	} else
		m_szName = NULL;

	m_pNext = rhs.m_pNext;
	m_pPrev = rhs.m_pPrev;
	m_pEntryList = rhs.m_pEntryList;
	m_nListSize = rhs.m_nListSize;
	m_eStatus = rhs.m_eStatus;
	return *this;
}
;

/***************************************************************************
 *
 * addEntry
 *
 ***************************************************************************/
StatusCode ZLexFileSection::addEntry(ZLexEntry *entry, bool begin) {
	if (entry == NULL)
		return E_BadParameter;

	ZLexEntry *pEntryList = m_pEntryList;
	ZLexEntry *pNew = entry;
	if (m_pEntryList == NULL) {
		m_pEntryList = entry;
		m_nListSize = 1;
		//m_pCurrentToken = pNew;
	} else {
		if (begin) {
			//insert entry at beginning of list;
			pNew->setNextEntryPtr(pEntryList);
			pEntryList->setPrevEntryPtr(pNew);
			//m_pCurrentToken = pNew;
		} else {
			while (pEntryList->getNextEntryPtr() != NULL) {
				pEntryList = pEntryList->getNextEntryPtr();
			}
			pNew->setPrevEntryPtr(pEntryList);
			pEntryList->setNextEntryPtr(pNew);
			//m_pCurrentToken = pNew;
			m_nListSize++;
		}
	}
	return E_NoError;
}

/***************************************************************************
 *
 * addEntry
 *
 * create an entry with a entry label if provided, not required
 *
 ***************************************************************************/
/*
 ZLexEntry* ZLexFileSection::addEntry(char* szEntry, bool begin)
 {
 //if (szEntry == NULL) return NULL;

 ZLexEntry* pThis = m_pEntryList;
 ZLexEntry* pNew = NULL;
 if (m_pEntryList == NULL)
 {
 m_pEntryList = new ZLexEntry(szEntry);
 if (m_pEntryList == NULL) return NULL;
 pNew = m_pEntryList;
 m_nListSize = 1;
 }
 else
 {
 pNew = new ZLexEntry(szEntry);
 if (pNew == NULL) return NULL;
 if(begin)
 {
 //insert entry at beginning of list;
 pNew->setNextEntryPtr(pThis);
 pThis->setPrevEntryPtr(pNew);
 }
 else
 {
 while (pThis->getNextEntryPtr() != NULL)
 {
 pThis = pThis->getNextEntryPtr();
 }
 pThis->setNextEntryPtr(pNew);
 m_nListSize++;
 }
 }
 m_pCurrentEntry = pNew;
 return pNew;
 }
 */

/***************************************************************************
 *
 * addEntryLine
 *
 ***************************************************************************/
/*
 StatusCode ZLexFileSection::addEntryLine(ZLexToken* lTok, bool begin)
 {
 if (lTok == NULL) return E_BadParameter;

 ZLexToken *tl = lTok;

 if(m_pEntryList == NULL)
 {
 m_pEntryList = new ZLexEntry();
 }

 if(begin)
 {
 while (tl != NULL)
 {
 ZLexElement* newElement = new ZLexElement(tl);
 m_pEntryList->addElement(newElement, true);
 tl = tl->next_;
 }
 }
 else
 {
 while (tl != NULL)
 {
 ZLexElement* newElement = new ZLexElement(tl);
 m_pEntryList->addElement(newElement, false);
 tl = tl->next_;
 }
 }
 return E_NoError;
 }
 */

/***************************************************************************
 *
 * addEntry
 *
 ***************************************************************************/
/*
 ZLexEntry* ZLexFileSection::addEntry(LexToken* lTok, bool begin)
 {
 if (lTok == NULL) return NULL;

 ZLexEntry* pEntryList = m_pEntryList;
 ZLexEntry* pNew = NULL;

 //ZLexElement *aTok = new ZLexElement(lTok);

 if (m_pEntryList == NULL)
 {
 m_pEntryList = new ZLexEntry(lTok);
 m_nListSize = 1;
 m_pCurrentEntry = m_pEntryList ;
 }
 else
 {
 if(begin)
 {
 //insert lTok at beginning of list;
 pNew = new ZLexEntry(lTok);
 pNew->setNextEntryPtr(pEntryList);
 pEntryList->setPrevEntryPtr(pNew);
 m_pCurrentEntry = pNew;
 }
 else
 {
 while (pEntryList->getNextEntryPtr() != NULL)
 {
 pEntryList = pEntryList->getNextEntryPtr();
 }
 pNew = new ZLexEntry(lTok);
 pNew->setPrevEntryPtr(pEntryList);
 pEntryList->setNextEntryPtr(pNew);
 m_pCurrentEntry= pNew;
 m_nListSize++;
 }
 }
 return pNew;
 }
 */

/***************************************************************************
 *
 * CreateMockEntry
 *
 ***************************************************************************/
ZLexEntry* ZLexFileSection::createMockEntry() {

	//create an entry on the heap
	ZLexEntry *entry = new ZLexEntry("createMockEntry()");

	//add three elements off the heap to the entry
	for (int i = 1; i < 4; i++) {
		//create an element from a test token on the heap
		ZLexElement *element = entry->createMockElement();

		//add newly allocated element to the end of the entry element list
		entry->addElement(element, false);
	}

	return entry;
}

/***************************************************************************
 *
 * deleteEntries
 *
 ***************************************************************************/
StatusCode ZLexFileSection::deleteEntries() {
	ZLexEntry *pLast = NULL;
	ZLexEntry *pThis = m_pEntryList;

	while (pThis != NULL) {
		pLast = pThis;
		pThis = pThis->getNextEntryPtr();
		delete (pLast);
		pLast = NULL;
	}

//	if (m_pEntryList != NULL)
//    delete (m_pEntryList);

	return E_NoError;
}

/***************************************************************************
 *
 * getName
 *
 ***************************************************************************/
StatusCode ZLexFileSection::getName(char szText[], int nBufferSize) {
	StatusCode eStatus = E_NoError;
	szText[0] = '\0';

	const char *temp = m_szName;
	if ((unsigned long int) strlen(temp) < nBufferSize) {
		strcpy(szText, m_szName);
	} else {
		eStatus = E_BufferOverflow;
	}

	return eStatus;
}

/***************************************************************************
 *
 * getFirstEntry
 *
 ***************************************************************************/

ZLexEntry* ZLexFileSection::getFirstEntry() {
	return getNextEntry(true);
}

/***************************************************************************
 *
 * getNextEntry
 *
 ***************************************************************************/
ZLexEntry* ZLexFileSection::getNextEntry(bool bStart) {
	StatusCode eStatus = E_NoError;

	if (bStart == true) {
		m_pCurrentEntry = m_pEntryList;
		eStatus = E_NoError;
		return m_pCurrentEntry;
	}

	if (m_pCurrentEntry != NULL) {
		m_pCurrentEntry = m_pCurrentEntry->getNextEntryPtr();
		eStatus = E_NoError;
	} else {
		eStatus = E_NoMoreEntries;
	}

	return m_pCurrentEntry;
}

/***************************************************************************
 *
 * GetEntry
 *
 ***************************************************************************/
ZLexEntry* ZLexFileSection::getEntry(ZLexElement *tok) {
	ZLexEntry *ent = m_pEntryList;
	ZLexElement *tst = (ZLexElement*) NULL;
	while (ent != NULL) {
		//get first element of entry
		tst = ent->getFirstElement();
		while (tst != NULL) {
			if (tst == tok) {
				return ent;
			}
			tst = tst->getNextPtr();
		}
		ent = ent->getNextEntryPtr();
	}
	return (ZLexEntry*) NULL;
}

/***************************************************************************
 *
 * insertEntry
 * add an entry before the referenced entry
 ***************************************************************************/
StatusCode ZLexFileSection::insertEntry(ZLexEntry *a_entry, ZLexEntry *a_pos) {
	if (a_pos == NULL)
		return E_BadParameter;
	if (a_entry == NULL)
		return E_BadParameter;

	ZLexEntry *pThis = m_pEntryList;
	ZLexEntry *pPrev = NULL;

	if (m_pEntryList == NULL) {
		// first entry
		m_pEntryList = a_entry;
		m_nListSize = 1;
	} else {

		pPrev = a_pos->getPrevEntryPtr();

		if (pPrev != NULL)
			pPrev->setNextEntryPtr(a_entry);

		a_entry->setPrevEntryPtr(pPrev);

		a_pos->setPrevEntryPtr(a_entry);

		a_entry->setNextEntryPtr(a_pos);

		m_nListSize++;

	}
	m_pCurrentEntry = a_entry;
	return E_NoError;
}

/***************************************************************************
 *
 * toXmlSchema()
 *
 ***************************************************************************/
const std::string ZLexFileSection::toXmlSchema() const {
	std::ostringstream streamOut;

	ZLexEntry *pListCursor = m_pEntryList;

	bool bReadNext = true;
	// traverse elements
	while (pListCursor != NULL) {

		streamOut << pListCursor->toXmlSchema();

		if (bReadNext) {
			pListCursor = pListCursor->getNextEntryPtr();
		}
		bReadNext = true;
	}

	return streamOut.str();
}

/***************************************************************************
 *
 * updateEntry
 *	//find the header field for each data field
 *	//and update attributes ==> length, datatype
 ***************************************************************************/
StatusCode ZLexFileSection::updateEntry(ZLexEntry *entry, bool begin) {
	if (entry == NULL)
		return E_BadParameter;

	char cName[1024];

	bool bReadNext = true;

	ZLexElement *elmnt = entry->getFirstElement();

	ZLexEntry *header = getFirstEntry();

	// traverse entry tokens
	while (elmnt != NULL) {
		// Update FieldName Tokens with FieldValue data statistics (i.e. max length) 
		if (elmnt->getTokenType() == LEXTOKEN_FIELDVALUE) {
			//if(updateFieldHeaders)
			//{
			ZLexElement *sElement = NULL;
			elmnt->getName(cName, sizeof(cName) - 1);
			//sElement = header->findElementByName(cName,true);
			sElement = header->findElementByIndex(elmnt->getId(), true);
			if (elmnt != NULL) {
				//set header field length to max found
				if ((elmnt->getLength() > 0) && (sElement->getLength() == 255))
					sElement->setLength(elmnt->getLength());
				else if (elmnt->getLength() > sElement->getLength())
					sElement->setLength(elmnt->getLength());
				/*
				 //set header field datatype to first determined field type of data field
				 //FOR THIS COLUMN!
				 if(elmnt->dataType != LEXTOKEN_DATATYPE_DEFAULT)
				 if(sElement->dataType == LEXTOKEN_DATATYPE_DEFAULT)
				 {
				 //sElement->dataType == LEXTOKEN_DATATYPE_UNDEFINED
				 //if(firstTimeSeenDataType == true)
				 //(
				 sElement->dataType = elmnt->dataType;

				 //firstSeenDataType = false;
				 //)
				 }
				 */
			}
		}
		elmnt = entry->getNextElement(false);
	}

	return E_NoError;
}

