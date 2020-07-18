/**************************************************************************
 *
 * zLexEntry.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexEntry.h
*/

#ifndef ___ZLEXENTRY_H___
#define ___ZLEXENTRY_H___

#include "zDwConst.h"
#include "zLexElement.h"

#include <string>

/****************************************************************************
 *
 * ZLexEntry
 *
 *****************************************************************************/
class ZLexEntry {
public:
	ZLexEntry();                                //default constructor
	ZLexEntry(char *str);                       //construct from text
	ZLexEntry(const ZLexEntry &rhs);            //copy contructor
	ZLexEntry& operator=(const ZLexEntry &rhs); //overload assignment
	~ZLexEntry();                               //destructor

	//entry member accessors
	int getEntryId() const {
		return m_nEntryId;
	}
	StatusCode getName(char szText[] /* out */,
			int nBufferSize1 /* in */) const;
	ZLexEntry* getNextEntryPtr() const {
		return m_pNextEntry;
	}
	ZLexEntry* getPrevEntryPtr() const {
		return m_pPrevEntry;
	}
	StatusCode getStatus() const {
		return m_eStatus;
	}
	int getListSize() const {
		return m_nListSize;
	}
	StatusCode reInit();
	void setNextEntryPtr(ZLexEntry *pNext) {
		m_pNextEntry = pNext;
	}
	void setPrevEntryPtr(ZLexEntry *pPrev) {
		m_pPrevEntry = pPrev;
	}

	//element list manipulators
	StatusCode addElement(const ZLexElement &elmnt, bool begin);
	StatusCode addElement(ZLexElement *elmnt, bool begin);
	ZLexElement* createMockElement();
	ZLexElement* findElementByName(const char *tokName, bool begin);
	ZLexElement* findElementByIndex(int nIdx, bool begin);
	ZLexElement* getNextElement(bool start);
	ZLexElement* getFirstElement();
	ZLexElement* getLastElement();
	StatusCode insertElement(ZLexElement *a_token, ZLexElement *a_pos);
	const std::string toXmlSchema() const;

private:
	StatusCode deleteElements();

private:
	StatusCode m_eStatus;
	int m_nListSize;
	int m_nEntryId;
	ZLexElement *m_pElementList;
	ZLexElement *m_pCurrentElement;
	ZLexEntry *m_pNextEntry;
	ZLexEntry *m_pPrevEntry;
	char *m_szName;
};

extern ZLexEntry* gl_pZLexEntry;

#endif //___ZLEXENTRY_H___
