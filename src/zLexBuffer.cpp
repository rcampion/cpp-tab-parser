/**************************************************************************
 *
 * zLexBuffer.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexBuffer.cpp
*/

#include "zLexFile.h"
#include "zLexSection.h"
#include "zLexException.h"
#include "zLexLexer.h"
#include "zLexParser.h"

#include "zDwVersion.h"
#include "BisonTabHdrLineParser.hpp"
#include "zCtrlDwRuntime.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	#include <process.h>
	#include <direct.h>
#else	// Unix
	#include <stdarg.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/unistd.h>
	#include <sys/utsname.h>
	#include <sys/wait.h>
#endif


/****************************************************************************
 * Flex Input Buffer  (reassignment of YY_INPUT to LexYYInput)
 * to understand what's going on here:
 * read page 157 of O'Reilly's "lex & yacc"
 * also see function definition/reassignment in header
 * - the result is the number of chars actually read
 ****************************************************************************/
//global lexer/parser buffer specific pointers
char* gl_pLexInputBuf;
char* gl_pLexInputPtr = 0;
char* gl_pLexInputLim = 0;

/******************************************************************************
 *	ZLexBuffer constructor
 ******************************************************************************/
ZLexBuffer::ZLexBuffer(int size) {
	buffer_ = new char[size + 1];
	buffer_[0]='\0';
	size_ = size + 1;
	offset_ = 0;
}

/******************************************************************************
 *	ZLexBuffer destructor
 ******************************************************************************/
ZLexBuffer::~ZLexBuffer(){
	delete buffer_;
}

/******************************************************************************
 *	ZLexFileBuffer
 ******************************************************************************/
ZLexFileBuffer::ZLexFileBuffer() {
	pZLexBuffer_ = NULL;
	line_=0;
}

/******************************************************************************
 *	ZLexFileBuffer
 ******************************************************************************/
ZLexFileBuffer::~ZLexFileBuffer() {
	release();
}

/******************************************************************************
 *	fileBufferInit
 ******************************************************************************/
StatusCode ZLexFileBuffer::init(int bufsize) 
{
	StatusCode retval = E_NoError;

	if(pZLexBuffer_ != NULL) {
		release();
	}

	pZLexBuffer_ = new ZLexBuffer(DEFAULT_LEX_BUFSIZE);

	if(pZLexBuffer_ == NULL) return E_MemError;
	
	if (pZLexBuffer_->buffer_ == (char*)NULL) {
		return E_BufferError;
	}

	return E_NoError;
}

/******************************************************************************
 *	fileBufferFree
 ******************************************************************************/
StatusCode ZLexFileBuffer::release()
{
	StatusCode retval = E_NoError;
	if (pZLexBuffer_ != NULL) {
		delete(pZLexBuffer_);
	}
	pZLexBuffer_= NULL;
	return E_NoError;
}

/******************************************************************************
 * fileBufferEnlarge
 * double the buffer size.
 ******************************************************************************/
StatusCode ZLexFileBuffer::enlarge()
{
	char* newbuf;
	//increase the size of the buffer by shift operation
	//unsigned long testsize1 = (pZLexBuffer_->size_ << 1);
	unsigned long bufsize = ((pZLexBuffer_->size_ << 1) * sizeof(char));
	newbuf = new char[bufsize];
	if (newbuf != (char*)NULL)
	{
		memcpy(newbuf, pZLexBuffer_->buffer_, pZLexBuffer_->size_ );
		delete pZLexBuffer_->buffer_;
		//unsigned long testsize2 = pZLexBuffer_->size_ <<= 1;
		pZLexBuffer_->size_ <<= 1;
		pZLexBuffer_->buffer_ = newbuf;
	}
	else
		return E_BufferError;

	return E_NoError;
}

/******************************************************************************
 *	fill a buffer from a char array
 ******************************************************************************/
StatusCode ZLexFileBuffer::fillFromBuffer(const char* inbuf) {
	//fill the m_pLexFileBuf->buf
	unsigned long byteCount = 0;
	int			nextChar = 0;
	StatusCode retval = E_NoError;
	//while ((nextChar = fgetc(stream1)) != EOF) {
	while ((nextChar = inbuf[byteCount]) != NULL) {
		if (putChar((unsigned char)nextChar) == E_BufferError)
		{
			//fclose(stream1);
			retval = E_BufferError;
			break;
		}
		else {
		//	fputc( nextChar, stream2 );
			byteCount++;
		}
	}
	pZLexBuffer_->size_= byteCount++;
	return E_NoError;
}

/***************************************************************************
 *
 * writeToFile
 * Note: DOS/Windows file format has 0x0D,0x0A as the EOL. 
 *			 Unix formatted files have simply 0x0A as the EOL. 
 ***************************************************************************/
StatusCode ZLexFileBuffer::writeToFile(const char* fname,int startLine)
{
	StatusCode retval = E_NoError;
	FILE		*fp = NULL;
	size_t	byteCount = 0;
	int			thisChar = 0;
	int			lineCount = 0;
	ZLexBuffer* thisBuffer = pZLexBuffer_;
	
	if(thisBuffer == NULL) return E_BufferError;
	
	if ((fp = fileOpen(fname,"wb")) == (FILE*)NULL) {
		retval = E_FileError;
		return retval;
	}
	else {
		byteCount = 0;
		thisChar = thisBuffer->buffer_[byteCount];
		size_t bufLimit = pZLexBuffer_->size_ -1;
		//end of file (EOF) = NULL char:
		//carriage-return followed by a new-line

		//get to the first line to begin outputting
		while((startLine > lineCount) && (byteCount < bufLimit)) {
			if(thisChar == 0x0A) {
				lineCount++;
			}
			thisChar = thisBuffer->buffer_[++byteCount];				
		}

		//now dump the rest of the chars to a file
		while(byteCount < bufLimit) {

			/* test */
			if((thisChar == 13) || (thisChar == 10) )
				std::cout << "Found CRLF" << std::endl;

			if((thisChar != 13)) {
				if(writeCharToFile(fp,thisChar) != E_NoError) {
					return E_FileError;
				}
			}
			thisChar = thisBuffer->buffer_[++byteCount];

		}

		//now add a new line char as the end of the file
		if(thisChar != 10 ) {
			if(writeCharToFile(fp,10) != E_NoError) {
				return E_FileError;
			}
		}
	}
	fileClose(fp);
	return retval;
}

/***************************************************************************
 *
 * writeCharToFile
 *
 ***************************************************************************/
StatusCode ZLexFileBuffer::writeCharToFile(FILE* fp,int c) {
	if(fputc(c, fp)==EOF) {
		return E_FileError;
	}
	return E_NoError;
}

/***************************************************************************
 *
 * fillFromFile
 *
 ***************************************************************************/
StatusCode ZLexFileBuffer::fillFromFile(const char* fname)
{
	StatusCode retval = E_NoError;
	FILE		*ifp = NULL;
	size_t	byteCount = 0;
	int			nextChar = 0;

	if ((ifp = fileOpen(fname,"rb")) == (FILE*)NULL)
	{
		retval = E_FileError;
		return retval;
	}
	else
	{
		// initialize the file buffer
		init(DEFAULT_LEX_BUFSIZE);

		if (pZLexBuffer_->buffer_ == (char*)NULL)
		{
			fclose(ifp);
			retval = E_MemError;
		}
		else
		{
			//fill the pZLexBuffer_->buffer_
			byteCount = 0;
			while ((nextChar = fgetc(ifp)) != EOF)
			{
				if (putChar((unsigned char)nextChar) == E_BufferError)
				{
					fclose(ifp);
					retval = E_BufferError;
					break;
				}
				else
				{
					byteCount++;
				}
			}
			pZLexBuffer_->buffer_[byteCount] = '\0';
			/*
			//"help" the lexer out by appending a newline ('\n') as the last char in line
			//so that a line of delimitted fields has, at minimum, a leading tab
			//and an ending new line.  This case would essentially be an empty field line
			//only add the new-line char if the last char is not a new-line
			
			int lastChar = pZLexBuffer_->buffer_[byteCount];
			if(lastChar != 10)
			{
				nextChar = 10; 
				if (putChar((unsigned char)nextChar) == E_BufferError)
				{
						fclose(ifp);
						retval = E_BufferError;
				}
				else
				{
					byteCount++;
				}
			}
			*/
			if (retval >= 0)
			{
				// reset the buffer offset and size
				pZLexBuffer_->offset_ = 0;
				pZLexBuffer_->size_ = byteCount+1;
				fclose(ifp);
			}
		}
	}
	return retval;
}

/******************************************************************************
 *	Write a char to memory.
 ******************************************************************************/
StatusCode ZLexFileBuffer::putChar(char byte)
{
	StatusCode retval = E_NoError;
	
	if (pZLexBuffer_->offset_ == pZLexBuffer_->size_)
	{
		if (enlarge() == E_BufferError)
		{
			return E_BufferError;
		}
	}
	pZLexBuffer_->buffer_[pZLexBuffer_->offset_++] = byte;

	return retval;
}

/******************************************************************************
 * countBufferLines
 * Note: DOS/Windows file format has 0x0D,0x0A as the EOL. 
 *				Unix formatted files have simply 0x0A as the EOL. 
 ******************************************************************************/
StatusCode ZLexFileBuffer::countBufferLines() {
	char* origBuf = pZLexBuffer_->buffer_;
	//char* newBuf = '\0';
	char* newBuf;
	int indx = 0;
	int nLine = 1;
	int c = origBuf[indx];
	
	//get to the line requested
	//while not at end of buffer
	while(c != '\0') {

		//count the carrage return/ line feeds
		if (c == 0x0D) {
			c = origBuf[indx++];
			if (c == 0x0A) {
				nLine++;
			}
		}
	}
	line_ = nLine;
	return E_NoError;
}

/******************************************************************************
 * removeBufferLine(int n)
 * Note: DOS/Windows file format has 0x0D,0x0A as the EOL. 
 *				Unix formatted files have simply 0x0A as the EOL. 
 ******************************************************************************/
StatusCode ZLexFileBuffer::removeBufferLine(int lineNo) {
	char* origBuf = pZLexBuffer_->buffer_;
	//char* newBuf = '\0';
	char* newBuf;
	int indx = 0;
	int nLine = 1;
	int c = origBuf[indx];
	
	//get to the line requested
	//while not at end of buffer
	while(c != '\0') {

		//count the carrage return/ line feeds
		if (c == 0x0D) {
			c = origBuf[indx++];
			if (c == 0x0A) {
				nLine++;
			}
		}
	}
	line_ = nLine;
	return E_NoError;
}

/***************************************************************************
 * fileOpen
 ***************************************************************************/
FILE* ZLexFileBuffer::fileOpen(const char* fileName, const char* openMode)
{
	FILE* fp;

	if (fileName != (char*)NULL)
	{
		fp = fopen(fileName, openMode);
	}
	else
	{
		fp = (FILE*)NULL;
	}
	return (fp);
}

/***************************************************************************
 * fileClose
 ***************************************************************************/
StatusCode ZLexFileBuffer::fileClose(FILE* fp)
{
	if (fclose(fp) != 0) {
		return E_FileError;
	}
	return E_NoError;
}

/***************************************************************************
 *
 * prepareBufferFromBinary
 *
 ***************************************************************************/

//StatusCode ZLexFileBuffer::prepareBufferFromBinary(ZLexFileBuffer *zbuf) 
StatusCode ZLexFileBuffer::prepareBufferFromBinary() 
{
	StatusCode sc = E_NoError;
	//assign the buffer to global flex variables

	// assign the buffer to Flex
	gl_pLexInputBuf = getBuffer();
	gl_pLexInputPtr = getBuffer();
	gl_pLexInputLim = getBuffer() + (getSize() -1);

	return E_NoError;
}

/****************************************************************************
 * Flex Input Buffer  (reassignment of YY_INPUT to LexYYInput)
 * to understand what's going on here:
 * read page 157 of O'Reilly's "lex & yacc"
 * also see function definition/reassignment in header
 * - the result is the number of chars actually read
 ****************************************************************************/
int LexYYInput(char* buf, int max_size)
{
/*
 	unsigned int n = (unsigned int)max_size;

	if ((unsigned int)max_size > ((unsigned int)gl_pLexInputLim - (unsigned int)gl_pLexInputPtr)) {
		n = (unsigned int)gl_pLexInputLim - (unsigned int)gl_pLexInputPtr;
	}
*/
	int n = max_size;
	if (max_size > (gl_pLexInputLim - gl_pLexInputPtr)) {
		n = gl_pLexInputLim - gl_pLexInputPtr;
	}

	if (n > 0) {
		memcpy(buf, gl_pLexInputPtr, n);
		gl_pLexInputPtr += n;
	}
	return (n);
}

