/*
 * CppTabParser.cpp
 *
 *  Created on: Jul 17, 2020
 *      Author: rcampion
 */

#include "CppTabParser.h"
/*
CppTabParser::CppTabParser() {
	// TODO Auto-generated constructor stub

}

CppTabParser::~CppTabParser() {
	// TODO Auto-generated destructor stub
}
*/
// test.cpp : Defines the entry point for the console application.
//
#include "zLexFile.h"
#include "zLexSection.h"
#include "zLexException.h"
#include "zLexLexer.h"
#include "zLexParser.h"

#include "zDwVersion.h"
#include "BisonTabHdrLineParser.hpp"
#include "zCtrlDwRuntime.h"

// #include "stdafx.h"

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

void showResults(ZLexFile *zFile);

/***************************************************************************
*
* createFile
* 	//1.open the input text file,
*	//2.split it into two files, a header file and a data file
*	//3.load the header portion into this file objects' text buffer
*   //4 parse the file with a low level parser
*   //5 display the tokens/elements of the file
***************************************************************************/
StatusCode testCreateFile(ZRuntime* pContext, ZLexFile* zFile) {

	return E_NoError;

}

/***************************************************************************
*
* testLexer
*
***************************************************************************/
StatusCode testLexer(ZRuntime* pContext, ZLexFile *zFile) {

	//get the full file spec from the run context
	std::string fileName = pContext->getDtFileInputSpec().c_str();
	if(fileName.length() == 0) return E_FileNameError;

	//try lexing the file to get the header and data portions into tokens
	try {
		/////////////////////////////////////////////////////////
		//take the input text buffer and pass it through a Lexer
		//the result should be an abstraction of a delimitted file
		//this abstract file should now contain one or more file section(s)...
		//each section containing at least one entry of n.elements
		/////////////////////////////////////////////////////////
		std::cout << "Attempting to Lex file: " << fileName << std::endl;

		zFile->setHeaderError(E_LEX_OK);
		StatusCode stat;
		stat = zFile->lexFile();

		//how did it go? ....check the status
		if(stat != E_NoError) {
			std::cout << fileName << " Lex failed...StatusCode...:" << stat << std::endl;
		}

		//how did it really go? ....check the syntax error
		LexError lerr;
		lerr = zFile->getHeaderError();
		if(lerr != E_LEX_OK) {
			std::cout << fileName << " Lexer failed...LexError.....:" << lerr << std::endl;
			//display the line(s) and element(s) that gave us error(s)
			//get the first error token
			//LexToken* errorToken = zFile->getErrorTokenListPointer();
			/*
			while(errorToken != NULL) {
				printErrorMessage(errorToken);
				errorToken = zFile->getNextErrorToken();
				};
			*/
		}

		//test for errors
		if((lerr != E_LEX_OK) || (stat != E_NoError) ) {
			return E_LexerError;
		}

		//yippee!
		std::cout << fileName << " Header Lexed ok." << std::endl;
	}

	catch(...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	return E_NoError;
}

/***************************************************************************
*
* testParser
*
***************************************************************************/
StatusCode testParser(ZRuntime* pContext, ZLexFile *zFile) {

	//get the full file spec from the run context
	std::string fileName = pContext->getDtFileInputSpec().c_str();
	if(fileName.length() == 0) return E_FileNameError;

	//try parsing the file to get the header and data portions into tokens
	try {
		/////////////////////////////////////////////////////////
		//take the input text buffer and pass it through a parser
		//via the lexer and the result should be an abstraction of a delimitted file
		//This abstract file should now contain one or more file section(s)...
		//each section containing at least one entry of n.elements
		/////////////////////////////////////////////////////////
		std::cout << "Attempting to parse file: " << fileName << std::endl;
		StatusCode stat;

		//Parse the file
		//pass the lexed (yylex) tokens over to the parser to check
		//for token/element validity
		stat = zFile->parseFile();

		//how did it go? ....check the status
		if(stat != E_NoError) {
			std::cout << fileName << " Parse failed...StatusCode...:" << stat << std::endl;
		}

		//how did it really go? ....check the syntax error
		LexError lerr;
		lerr = zFile->getHeaderError();
		if(lerr != E_LEX_OK) {
			std::cout << fileName << " Parse failed...LexError.....:" << lerr << std::endl;
		}

		//test for errors
		if((lerr != E_LEX_OK) || (stat != E_NoError) ) {
			return E_ParserError;
		}

		//yippee!
		std::cout << fileName << " Header Parsed ok." << std::endl;

		zFile->showHeader();

		//how did it really go? ....check the syntax error
		lerr = zFile->getDataError();
		if(lerr != E_LEX_OK) {
			std::cout << fileName << " Parse failed...LexError.....:" << lerr << std::endl;
		}

		//test for errors
		if((lerr != E_LEX_OK) || (stat != E_NoError) ) {
			return E_ParserError;
		}

		//yippee!
		std::cout << fileName << " Data Parsed ok." << std::endl;

		zFile->showData();

	}

	catch(...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	return E_NoError;
}

/*****************************************************************************
*
* outputConsoleMsg
*
*****************************************************************************/
int outputConsoleMsg(int nLevel, char *msg, ...)
{
	// first check the global settings
	if (gl_nQuietLevel == MAX_QUIET)  return 0; // silent mode
	if (gl_nQuietLevel < nLevel)			return 0;

  char cMsgBuf[2048];    // the output message buffer
  int nCount = -1;
  va_list argp;
  int nResult = 1;

	va_start (argp, msg);
  nCount = vsprintf (cMsgBuf, msg, argp);
  va_end (argp);

  //strcat(cMsgBuf, "\n");

	return printf(cMsgBuf);
}


/***************************************************************************
*
* main
* 1. Creates a runtime object containint runtime info, default attibutes
* 2. Creates a LexFile object to be analyzed.
***************************************************************************/
int main(int argc, char* argv[])
{
	printf("Tab Parser Test...\n");

	/////////////////////////////////////////////////////////
	//construct the context of this run
	/////////////////////////////////////////////////////////
	ZRuntime* pContext = new ZRuntime(argc, argv);
	if (! pContext) {
		// constructor failed
		outputConsoleMsg(L_STATUS,"%s version %s \n%s\n",PROG_NAME, PROG_VERSION, GEN_DATE);
		exit(0);
	}

	/////////////////////////////////////////////////////////
	//set options not provided on command line
	/////////////////////////////////////////////////////////
	pContext->setDefaultOptions();

	/////////////////////////////////////////////////////////
	//get the name of the file to be processed (parsed)
	/////////////////////////////////////////////////////////
	std::string fileName = pContext->getDtFileInputSpec().c_str();
	if(fileName.length() == 0) return E_FileNameError;
	std::cout << "filename ...==> " << fileName << std::endl;

	/////////////////////////////////////////////////////////
	//build the full path to the file
	/////////////////////////////////////////////////////////
	if ( ! pContext->buildAndCheckInputFileSpec()) {
			std::cout << "Problem with the filename ...==> " << fileName << std::endl;
			return E_FileNameError;
	}

	/////////////////////////////////////////////////////////
	//display the file name and path
	/////////////////////////////////////////////////////////
	fileName = pContext->getDtFileInputSpec().c_str();
	std::cout << "filename ...==> " << fileName << std::endl;

	/////////////////////////////////////////////////////////
	// create the file object using the context info
	// comment: address gets lost during function call
	// don't know how to pass in a null pointer and create object outside
	// this function
	/////////////////////////////////////////////////////////
	ZLexFile* zFile = NULL;
	ZLexFile* newFile = NULL;
	StatusCode sc = E_NoError;

	try {
		//try constructing a ZLexFile object by filename only,no context object
		newFile = new ZLexFile(fileName.c_str());

		//was a file object created, memory allocated?
		if(newFile == NULL) {
			std::cout << "File Contructor Error: " << fileName << std::endl;
			return E_FileError;
		}
		std::cout << "ZLexFile Object Created: " << fileName << std::endl;

		//even though a file object was created, is it parseable?
		sc = newFile->getInputTxtFileStatus();
		if(sc != E_NoError) {
			delete newFile;
			newFile = NULL;
			std::cout << "Cannot proceed with this file ...==> " << fileName << std::endl;
			return sc;
		}
	}
	catch(...) {
		return E_Exception;
	}

	//assignment test
	if(NULL == newFile) {
		return E_FileError;
	}
	else {
		zFile = newFile;
	}

	/////////////////////////////////////////////////////////
	//test the lexing of a file
	/////////////////////////////////////////////////////////

/*
	if(E_NoError == sc) {
		sc = testLexer(pContext,zFile);
		//show lexer report
		showResults(zFile);
	}
*/
	/////////////////////////////////////////////////////////
	//test the parsing of a file
	/////////////////////////////////////////////////////////
	if(E_NoError == sc) {
		sc = testParser(pContext,zFile);
	}

	if(E_NoError == sc) {
		showResults(zFile);
	}

	if(zFile!=NULL)
		delete zFile;

	if(pContext!=NULL)
		delete pContext;

	return sc;
}

/*****************************************************************************
*
* showResults
*
*****************************************************************************/
void showResults(ZLexFile *zFile) {

	//show the results of the lexer and/or parser details

	ZLexFileSection* pFileSections = zFile->getSectionList();
	ZLexFileSection* thisSection = pFileSections;

	//iterate all file sections
	thisSection = zFile->getNextSection(true);
	while(thisSection	!= NULL) {
		char szName[40];
		char szText[100];
		thisSection->getName(szName,39);
		std::cout << szName << std::endl;

		//iterate all file section entries
		ZLexEntry* entry = thisSection->getNextEntry(true);
		while(entry	!= NULL) {
			entry->getName(szName,39);
			std::cout << szName << std::endl;

			//iterate all file section entry elements
			ZLexElement* element = entry->getNextElement(true);
			while(element	!= NULL) {
				element->getName(szName,39);
				element->getText(szText,99);
				std::cout << szName << "=" << szText << " columm = "<< element->getColumn() << std::endl;
				element = entry->getNextElement(false);
			}
			entry = thisSection->getNextEntry(false);
		}
		thisSection = zFile->getNextSection(false);
	}

	ZLexElement* pElementList = NULL;
	zFile->getHeaderElements(&pElementList);

	ZLexElement* listElement = pElementList;

	int count =0;
	char szName[40];
	char szText[40];
	while(listElement != NULL) {
		//addFieldItem(listElement);
		//listElement->getName(szName,(sizeof(szName) / sizeof(TCHAR)));
		listElement->getName(szName,(sizeof(szName) / sizeof(wchar_t)));
		std::cout << szName << std::endl;
		//listElement->getText(szText,(sizeof(szText) / sizeof(TCHAR)));
		listElement->getText(szText,(sizeof(szText) / sizeof(wchar_t)));
		std::cout << szText << std::endl;
		//std::cout << listElement->getText;
		listElement = listElement->getNextPtr();
		count++;
	}

	//write out the header elements as a torque db schema
	zFile->toXmlSchemaFile("office","contact","/_/data/dw/schema");

	zFile->toSqlSchemaFile("office","contact","/_/data/dw/schema");

	zFile->toSqlDataFile("office","contact","/_/data/dw/schema");

}

