/**************************************************************************
 *
 * zLexBuffer.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 * Lex Buffer Class Definition
 **************************************************************************/
/*! \file zLexBuffer.h
*/

#ifndef ___ZLEXBUFFER_H___
#define ___ZLEXBUFFER_H___

#include <stdio.h>

/****************************************************************************
 * Flex Input Buffer  (reassignment of YY_INPUT to LexYYInput)
 * To understand what's going on here:
 * read page 157 of O'Reilly's "lex & yacc"
 * also see implementation of LexYYInput
 ****************************************************************************/
#undef YY_INPUT
 int LexYYInput(char* buf, int max_size);
#define YY_INPUT(b, r, ms) (r = LexYYInput(b, ms))

#ifdef TEST_DEBUG
	#define LEXFIRSTBUFSIZE		4096
#else
	#define LEXFIRSTBUFSIZE		8192
#endif	/* TEST_DEBUG */

/***************************************************************************
 *
 * ZLexBuffer struct
 *
 **************************************************************************/
struct ZLexBuffer {
	ZLexBuffer(int size);
	~ZLexBuffer();
	char*         buffer_;
	unsigned long offset_;
	unsigned long size_;
};

/***************************************************************************
 *
 * ZLexFileBuffer
 *
 **************************************************************************/
class ZLexFileBuffer {
public:
	ZLexFileBuffer(); 
	~ZLexFileBuffer();
	StatusCode countBufferLines();
	StatusCode fillFromBuffer(const char* inbuf);
	StatusCode fillFromFile(const char* filename);
	char* getBuffer() { return pZLexBuffer_->buffer_; };
	unsigned long getSize() { return pZLexBuffer_->size_; };
	StatusCode removeBufferLine(int n);	
	StatusCode putChar(char byte);
	StatusCode writeToFile(const char* fname,int n);
	StatusCode writeCharToFile(FILE* fp, int c);
	StatusCode prepareBufferFromBinary();
 	
private:
	StatusCode enlarge();
	int        line_;
	StatusCode init(int bufsize);
	StatusCode release();
	ZLexBuffer* pZLexBuffer_;

	FILE*      fileOpen(const char* fileName, const char* openMode);
	StatusCode fileClose(FILE* fp);
};

#endif //___ZLEXBUFFER_H___


