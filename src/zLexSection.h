/**************************************************************************
 *
 * zLexSection.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/***************************************************************************
 *
 * ZLexFileSection
 *
 **************************************************************************/
/**
 *
 * A ZLexFileSection contains ZLexEntry (s).
 *
 */
#ifndef ___ZLEXFILESECTION_H___
#define ___ZLEXFILESECTION_H___


#include "zDwConst.h"

#include "zLexElement.h"
#include "zLexEntry.h"

#include <string>

/****************************************************************************
*
* ZLexFileSection
*
*****************************************************************************/
class ZLexFileSection
{
public:
	ZLexFileSection();                                //default constructor
	ZLexFileSection(char* szName);                    //construct using name
	ZLexFileSection(const ZLexFileSection& rhs);      //copy contructor
	~ZLexFileSection();

	StatusCode       addEntry(ZLexEntry* entry, bool begin);
	//ZLexEntry*     addEntry(char* szLabel, bool begin);
	//ZLexEntry*     addEntry(LexToken *lTok, bool begin);
	//StatusCode     addEntryLine(ZLexToken *lTok, bool begin);
	ZLexEntry*       createMockEntry();
	ZLexEntry*       getEntry(char *szText);
	ZLexEntry*       getEntry(ZLexElement *tok);
	ZLexEntry*       getFirstEntry();
	int              getListSize()   { return m_nListSize; }
	int              getSectionId()  { return m_nSectionId; }
	StatusCode       getName(char szText[], int nBufferSize1);
	ZLexEntry*       getNextEntry(bool bStart);
	ZLexFileSection* getNextPtr()    { return m_pNext; }
	ZLexFileSection* getPrevPtr()    { return m_pPrev; }
	StatusCode       getStatus()     { return m_eStatus; }
	StatusCode	     reInit();
	void             setNextPtr(ZLexFileSection *pNext) { m_pNext = pNext; }
	void             setPrevPtr(ZLexFileSection *pPrev) { m_pPrev = pPrev; }

	StatusCode    insertEntry(ZLexEntry* entry_pos, ZLexEntry* a_entry);
	StatusCode    updateEntry(ZLexEntry* entry, bool begin);

  const std::string toXmlSchema() const;
	
private:
	// list of sections to iterate
	ZLexEntry*       m_pEntryList;
	ZLexEntry*       m_pCurrentEntry;
	int              m_nSectionId;
	char*            m_szName;
	ZLexFileSection* m_pNext;
	ZLexFileSection* m_pPrev;		
	int              m_nListSize;
	StatusCode       m_eStatus;	
	int              m_nFileBeginOffset;
	int              m_nFileEndOffset;

public:
	StatusCode       deleteEntries();
	ZLexFileSection& operator= (const ZLexFileSection& rhs);  //overload assignment
	friend bool   operator== (const ZLexFileSection& lhs, const ZLexFileSection& rhs)
		{ return lhs.m_nSectionId == rhs.m_nSectionId; }
};

extern ZLexFileSection* gl_pZLexFileSection;

#endif //___ZLEXFILESECTION_H___
