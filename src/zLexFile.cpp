/**************************************************************************
 *
 * zLexFile.cpp  Lex File Class implementation
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexFile.cpp
 */

#pragma warning(disable:4786)

#include "zLexError.h"
#include "zLexFile.h"
#include "zLexLexer.h"
#include "zLexParser.h"
#include "zLexBuffer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../require.h"

std::string& replaceAll(std::string &context, const std::string &from,
		const std::string &to);

std::string& removeAll(std::string &context, const std::string &from,
		const std::string &to);

extern char *LexInput;
extern char *LexInputPtr;
extern char *LexInputLim;
//extern ZLexFileBuf* gl_pZLexFileBuf;
extern int gl_nQuietLevel;

ZLexFile *gl_pZLexFile = NULL;

//if the input is a .txt do a parseText
//if the input is a .zta do a parseHeader
//if the input is a .ztd do a parseData

/***************************************************************************
 *
 * contructor
 *
 ***************************************************************************/
ZLexFile::ZLexFile() {
	currentSection_ = NULL;
	//dataFileStream_  = NULL;
	dataStatus_ = E_NoError;
	dataError_ = E_LEX_OK;
	//dataParser_ = NULL;

	//fileBuffer_ = NULL;				// char* based file buffer

	fileLineBuffer_ = "";     // std:string based text storage for lexer

	//headerFileStream_;
	headerStatus_ = E_NoError;
	headerError_ = E_LEX_OK;
	;
	//headerParser_ = NULL;

	inputTxtFileName_ = "";       //file to split into header/data files
	inputTxtFileStatus_ = E_NoError;
	inputDataFileName_ = "";      //data file to parse
	inputDataFileStatus_ = E_NoError;
	inputHeaderFileName_ = "";       //header file to parse
	inputHeaderFileStatus_ = E_NoError;

	lineCount_ = 0;     //number of lines in input
	listSize_ = 0;      //number of file sections

	runContext_ = NULL;	//runtime variables - currently only need the filename - this will change
	sectionList_ = NULL; // list of file sections to iterate

	//txtFileStream_ = NULL;

	xmlOutputFile_ = NULL;
	xmlOutputFileOpen_ = false;
	xmlOutputFileName_ = "";
	xmlOutputFileSpec_ = "";

}

/***************************************************************************
 *
 * contructor
 *
 ***************************************************************************/
ZLexFile::ZLexFile(ZRuntime *run) {

	runContext_ = run; //runtime variables - currently only need the filename - this will change

	currentSection_ = NULL;
	//dataFileStream_  = NULL;
	dataStatus_ = E_NoError;
	dataError_ = E_LEX_OK;
	//dataParser_ = NULL;

	//fileBuffer_ = NULL;				// char* based file buffer
	fileLineBuffer_ = "";     // std:string based text storage for lexer

	//headerFileStream_;
	headerStatus_ = E_NoError;
	headerError_ = E_LEX_OK;
	;
	//headerParser_ = NULL;

	inputTxtFileName_ = runContext_->getDtFileInputSpec();  //file to process
	inputTxtFileStatus_ = E_NoError;
	inputDataFileName_ = "";      //split file to parse
	inputDataFileStatus_ = E_NoError;
	inputHeaderFileName_ = "";       //split file to parse
	inputHeaderFileStatus_ = E_NoError;

	lineCount_ = 0;     //number of lines in input
	listSize_ = 0;      //number of file sections

	sectionList_ = NULL; // list of file sections to iterate
	//txtFileStream_ = NULL;

	xmlOutputFile_ = NULL;
	xmlOutputFileOpen_ = false;
	xmlOutputFileName_ = "";
	xmlOutputFileSpec_ = "";
}
;

/***************************************************************************
 *
 * contructor
 *
 ***************************************************************************/
ZLexFile::ZLexFile(const char *fileName) {
	currentSection_ = NULL;
	//dataFileStream_  = NULL;
	dataStatus_ = E_NoError;
	dataError_ = E_LEX_OK;
	//dataParser_ = NULL;

	//fileBuffer_ = NULL;
	fileLineBuffer_ = "";     // text storage for lexer

	//headerFileStream_;
	headerStatus_ = E_NoError;
	headerError_ = E_LEX_OK;
	;
	//headerParser_ = NULL;

	inputTxtFileName_ = "";      //file to split
	inputTxtFileStatus_ = E_NoError;
	inputDataFileName_ = "";      //split file to parse
	inputDataFileStatus_ = E_NoError;
	inputHeaderFileName_ = "";       //split file to parse
	inputHeaderFileStatus_ = E_NoError;

	lineCount_ = 0;     //number of lines in input
	listSize_ = 0;      //number of file sections

	runContext_ = NULL;	//runtime variables - currently only need the filename - this will change
	sectionList_ = NULL; // list of file sections to iterate

	xmlOutputFile_ = NULL;
	xmlOutputFileOpen_ = false;
	xmlOutputFileName_ = "";
	xmlOutputFileSpec_ = "";

	if ((fileName != NULL) && (fileName[0] != '\0')) {
		//m_fileName = new char[strlen(fileName) + 1];
		inputTxtFileName_ = fileName;
	} else
		inputTxtFileName_ = "";
}

/***************************************************************************
 *
 * assignment operator
 * Note: INCOMPLETE
 ***************************************************************************/
ZLexFile& ZLexFile::operator=(const ZLexFile &rhs) {
	runContext_ = rhs.runContext_;
	return *this;
}
;

/***************************************************************************
 *
 * destructor
 *
 ***************************************************************************/
ZLexFile::~ZLexFile() {
	//delete the sections
	//which deletes the entries
	//which deletes the elements
	deleteSections();

	/*
	 if (m_szName != NULL)
	 {
	 delete m_szName;
	 }

	 m_szName = NULL;
	 */

}

/***************************************************************************
 *
 * initHeader
 *
 ***************************************************************************/
StatusCode ZLexFile::initHeader() {
	//open the input header file
	if (openInputHeader() != E_NoError)
		return E_FileError;

	std::vector<std::string> fileLines_;
	std::string line;
	std::string formatted_line = "";
	int num = 0;
	char numbuf[20];

	//add a section tag to the file
	formatted_line = "<zsection";
	//itoa(num,numbuf,10);
	snprintf(numbuf, sizeof(numbuf), "%d", 10);
	//formatted_line.append(1,numbuf);
	formatted_line += numbuf;
	//formatted_line.append(1," ");
	formatted_line += "> ";
	line.insert(0, formatted_line);
	fileLines_.push_back(line);
	fileLineBuffer_ += (line + "\n");
	num++;

	//mark the lines with entry tags
	while (std::getline(headerFileStream_, line)) {
		//@zl[0-9]+" "
		formatted_line = "<zentry";
		//itoa(num,numbuf,10);
		snprintf(numbuf, sizeof(numbuf), "%d", num);
		//formatted_line.append(1,numbuf);
		formatted_line += numbuf;
		//formatted_line.append(1," ");
		formatted_line += "> ";
		line.insert(0, formatted_line);
		fileLines_.push_back(line);
		fileLineBuffer_ += (line + "\n");
		num++;
	}
	lineCount_ = fileLines_.size();
	if (lineCount_ == 0)
		return E_BufferEmpty;

	//close the input file
	closeInputHeader();

	//ZLexElement* pElementList = NULL;
	//getHeaderElements(&pElementList);

	//ZLexElement* listElement = pElementList;

	//get a handle to the section list
	ZLexFileSection *aHeader = sectionList_;
	ZLexEntry *entry = aHeader->getFirstEntry();

	char cDesc[1024];
	char cName[1024];
	char cText[1024];

	ZLexElement *elmt = entry->getFirstElement();
	while (elmt != NULL) {
		//if(elmt->section == LEXFILESECTION_HEADING)
		//if(elmt->getSection() == sec)
		//{

		//create valid fieldnames
		if (elmt->getTokenType() == LEXTOKEN_FIELDNAME) {

			GetLexTokenDataTypeDesc(elmt->getDataType(), cDesc,
					sizeof(cDesc) - 1);
			elmt->getName(cName, sizeof(cName) - 1);
			elmt->getText(cText, sizeof(cText) - 1);
			LexDataType test = elmt->getDataType();

			std::string s(cText);

			std::transform(s.begin(), s.end(), s.begin(), ::toupper);

			rtrim(s);

			replaceIllegalFieldChars(s);

			fieldEntries_.push_back(s);
		}

		elmt = entry->getNextElement(false);
	}

	return E_NoError;

}

/***************************************************************************
 *
 * initData
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::initData()
 {
 //open the input file
 if(openInputData() != E_NoError)
 return E_FileError;

 std::vector<std::string> fileLines_ ;
 std::string line;
 std::string formatted_line = "";
 int num = 0;
 char numbuf[20];

 //add a section tag to the file
 formatted_line="<zsection";
 itoa(num,numbuf,10);
 //formatted_line.append(1,numbuf);
 formatted_line+=numbuf;
 //formatted_line.append(1," ");
 formatted_line+="> ";
 line.insert(0,formatted_line);
 fileLines_.push_back(line);
 fileLineBuffer_ += (line + "\n");
 num++;

 //mark the lines with entry tags
 while(std::getline(dataFileStream_,line)) {
 //@zl[0-9]+" "
 formatted_line="<zentry";
 itoa(num,numbuf,10);
 //formatted_line.append(1,numbuf);
 formatted_line+=numbuf;
 //formatted_line.append(1," ");
 formatted_line+="> ";
 line.insert(0,formatted_line);
 fileLines_.push_back(line);
 fileLineBuffer_ += (line + "\n");
 num++;
 }
 lineCount_ = fileLines_.size();
 if(lineCount_ == 0) return E_BufferEmpty;

 //close the input file
 closeInputData();

 return E_NoError;

 }
 */
/***************************************************************************
 *
 * createHeaderFile
 *
 ***************************************************************************/
StatusCode ZLexFile::createHeaderFile() {
	enum bufs {
		BASE, TXT, HEADER, DATA, HLINE1, DLINE1, BUFNUM
	};
	std::string part[BUFNUM];

	part[BASE] = inputTxtFileName_.c_str();

	// Find any '.' in the std::string:
	size_t loc = part[BASE].find('.');
	if (loc != std::string::npos)
		part[BASE].erase(loc); // Strip extension
	/*
	 // Force to upper case:
	 for(int i = 0; i < part[BASE].size(); i++)
	 part[BASE][i] = toupper(part[BASE][i]);
	 */
	// Create file names and internal tag lines:
	part[TXT] = part[BASE] + ".txt";		//text file in
	part[HEADER] = part[BASE] + ".zta"; //header("T"able "A"ttributes) file out
	part[DATA] = part[BASE] + ".ztd";   //data("T"able "D"ata) file out
	part[HLINE1] = "#<zheader>: " + part[HEADER];
	part[DLINE1] = std::string("#<zdata>") + ": " + part[DATA];

	inputHeaderFileName_ = part[HEADER].c_str();
	// First, try to open input text file:
	std::ifstream existtxt(part[TXT].c_str());
	if (!existtxt) { // Input text file doesn't exist; abort for now, maybe create a sample?
		std::cout << "Input File Error: File " << part[TXT].c_str()
				<< " does not exist. ";
		return E_FileNotFound;
	}

	// read the first line of input file into a buffer (std::string)
	// assuming/expecting this to be the the header
	// also assuming that there are no embedded new lines
	std::string inString;
	existtxt.seekg(0);
	std::getline(existtxt, inString);
	existtxt.close();

	// Second, try to open previously created header file:
	// create the file
	std::ifstream existheader(part[HEADER].c_str());
	if (!existheader) { // Create header file.
		//existheader.close();
		std::ofstream newheader(part[HEADER].c_str());
		//assure(newheader, part[HEADER].c_str());
		//newheader << part[HLINE1];
		//newheader << "\tdate: xx-xx-xxxx "  << endl; // Date created marker
		std::string cleanedString = cleanHeader(inString);
		newheader << cleanedString; // write the header fields
		newheader << std::endl; // write a new line char
		newheader.close();
	} else { // Overwrite header file for now
			 // ToDo: Already exists;
			 // Verify it, perform a diff, name/number/datatype/len of fields.
		// Check that new header fields = old header
		//existheader.close();
		std::ofstream newheader(part[HEADER].c_str());
		//assure(newheader, part[HEADER].c_str());
		//newheader << part[HLINE1];
		//newheader << "\tdate: xx-xx-xxxx "  << endl; // Date created marker
		std::string cleanedString = cleanHeader(inString);
		newheader << cleanedString; // write the header fields

		newheader << '\t';

		newheader << std::endl; // write a new line char and flush stream

		newheader.close();
	}
	return E_NoError;
}

/***************************************************************************
 *
 * createDataFile
 -open the input.txt file
 -skip over the header
 -write out the rest
 -close the input file
 *
 ***************************************************************************/
StatusCode ZLexFile::createDataFile() {

//------------- file name bullshit
	enum bufs {
		BASE, TXT, HEADER, DATA, HLINE1, DLINE1, BUFNUM
	};
	std::string part[BUFNUM];
	part[BASE] = inputTxtFileName_.c_str();

	// Find any '.' in the std::string:
	size_t loc = part[BASE].find('.');
	if (loc != std::string::npos)
		part[BASE].erase(loc); // Strip extension

	// Create file names and internal tag lines:
	part[TXT] = part[BASE] + ".txt";		//text file in
	part[HEADER] = part[BASE] + ".zta"; //header("T"able "A"ttributes) file out
	part[DATA] = part[BASE] + ".ztd";   //data("T"able "D"ata) file out
	part[HLINE1] = "#<zheader>: " + part[HEADER];
	part[DLINE1] = std::string("#<zdata>") + ": " + part[DATA];
//---------------

	//fill the char* based buffer
	ZLexFileBuffer *fileBuffer_ = new ZLexFileBuffer();
	StatusCode fs = fileBuffer_->fillFromFile(inputTxtFileName_.c_str());
	inputDataFileName_ = part[DATA].c_str();

	int start = 1; //number of lines to skip over

	//write out a data only file (no header / skip first line)
	StatusCode retval = fileBuffer_->writeToFile(inputDataFileName_.c_str(),
			start);

	delete fileBuffer_;
	return E_NoError;
}

/***************************************************************************
 * prepareForParse()
 * split the text file into two files, a header file and a data file
 * - createHeaderFile()
 * - createDataFile()
 *
 ***************************************************************************/
StatusCode ZLexFile::prepareForParse() {
	StatusCode sc = E_NoError;

	////////////////////////////////////////////////////////
	//create the table attribute file (header = "input.zta")
	////////////////////////////////////////////////////////
	try {
		sc = createHeaderFile();
		if (sc != E_NoError)
			return sc;
	} catch (...) {
		return E_Exception;
	}

	////////////////////////////////////////////////////////
	//create the table data file (data = "input.ztd")
	////////////////////////////////////////////////////////
	try {
		sc = createDataFile();
		if (sc != E_NoError)
			return sc;
	} catch (...) {
		return E_Exception;
	}

	return E_NoError;
}

/***************************************************************************
 * prepareForLex()
 * split the text file into two files, a header file and a data file
 * - createHeaderFile()
 * - createDataFile()
 *
 ***************************************************************************/
StatusCode ZLexFile::prepareForLex() {
	StatusCode sc = E_NoError;

	////////////////////////////////////////////////////////
	//create the table attribute file (header = "input.zta")
	////////////////////////////////////////////////////////
	try {
		sc = createHeaderFile();
		if (sc != E_NoError)
			return sc;
	} catch (...) {
		return E_Exception;
	}

	////////////////////////////////////////////////////////
	//create the table data file (data = "input.ztd")
	////////////////////////////////////////////////////////
	try {
		sc = createDataFile();
		if (sc != E_NoError)
			return sc;
	} catch (...) {
		return E_Exception;
	}

	return E_NoError;
}

/***************************************************************************
 * lexFile
 *	1. prepare for a lex run,
 *			- create two files from input text file,
 *				a header file and a data file
 *	2. lex the header file
 *			- open and load the header file into this file-objects' text buffer
 *			- call a low level lexer (flex) and place the tokens
 *				into a header section
 *	3. lex the data file
 *			- open and load the data file into this file-objects' text buffer
 *			- call a low level lexr (flex) and place the tokens
 *				into a data section
 ***************************************************************************/
StatusCode ZLexFile::lexFile() {
	StatusCode sc = E_NoError;
	headerError_ = E_LEX_OK;

	std::string fileName = inputTxtFileName_;

	////////////////////////////////////////////////////////
	//1.prepare for a Lex run,
	//  create two files from the one input text file,
	//  - split the input text file into two files 
	//    - header file
	//    - data file 	
	////////////////////////////////////////////////////////
	try {
		sc = prepareForLex();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	////////////////////////////////////////////////////////
	//2.Lex the header file
	////////////////////////////////////////////////////////
	try {
		sc = lexHeader();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	////////////////////////////////////////////////////////
	//3.Lex the data file
	////////////////////////////////////////////////////////
	try {
		sc = lexData();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}

	return E_NoError;
}

/***************************************************************************
 * parseFile
 *	1. prepare for a parse run,
 *			- create two files from input text file,
 *				a header file and a data file
 *	2. parse the header file
 *			- open and load the header file into this file-objects' text buffer
 *			- call a low level parser (flex/bison) and place the tokens
 *				into a header section
 *	3. parse the data file
 *			- open and load the data file into this file-objects' text buffer
 *			- call a low level parser (flex/bison) and place the tokens
 *				into a header section
 ***************************************************************************/
StatusCode ZLexFile::parseFile() {
	StatusCode sc = E_NoError;

	std::string fileName = inputTxtFileName_;

	////////////////////////////////////////////////////////
	//1.prepare for a parse run,
	//  create two files from the one input text file,
	//  - split the input text file into two files 
	//    - header file
	//    - data file 	
	////////////////////////////////////////////////////////
	try {
		sc = prepareForParse();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	////////////////////////////////////////////////////////
	//2.parse the header file
	////////////////////////////////////////////////////////
	try {
		sc = parseHeader();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}
	////////////////////////////////////////////////////////
	//3.parse the data file
	////////////////////////////////////////////////////////
	try {
		sc = parseData();
		if (E_NoError != sc) {
			return sc;
		}
	} catch (...) {
		std::cout << fileName << " threw an exception!" << std::endl;
		return E_Exception;
	}

	return E_NoError;
}

/***************************************************************************
 *
 * LexHeader
 *
 ***************************************************************************/
StatusCode ZLexFile::lexHeader() {

	//initialize the header
	StatusCode sc = initHeader();

	if (E_NoError != sc)
		return sc;

	//create a new buffer with some chars
	ZLexFileBuffer *fileBuffer_ = new ZLexFileBuffer();
	StatusCode fs = fileBuffer_->fillFromFile(inputHeaderFileName_.c_str());

	//create a new Lexer with the header text line buffer
	//probably need a way to Lex the file directly for extra large files
	ZLexLexer *headerLexer_ = new ZLexLexer(this, fileBuffer_);

	//test if object was created
	if (headerLexer_ == NULL)
		return E_LexerError;

	//test for object status
	sc = headerLexer_->getLexerStatus();
	if (sc != E_NoError)
		return sc;

	//Lex the header file
	gl_pZLexFile = this;

	headerStatus_ = headerLexer_->doLexHeader();

	//headerError_ = headerLexer_->getLexerError();

	delete headerLexer_;
	headerLexer_ = NULL;

	delete fileBuffer_;
	fileBuffer_ = NULL;

	if (headerStatus_ != E_NoError)
		return E_LexerError;

	return E_NoError;
}

/***************************************************************************
 *
 * LexData
 *
 ***************************************************************************/
StatusCode ZLexFile::lexData() {

	//initialize the header
	//StatusCode sc = initData();
	StatusCode sc = E_NoError;

	//if(E_NoError != sc)
	//	return sc;

	//create a new buffer with some chars
	ZLexFileBuffer *fileBuffer_ = new ZLexFileBuffer();
	StatusCode fs = fileBuffer_->fillFromFile(inputDataFileName_.c_str());

	//create a new Lexer with the header text line buffer
	//probably need a way to Lex the file directly for extra large files
	ZLexLexer *dataLexer_ = new ZLexLexer(this, fileBuffer_);

	//test if object was created
	if (dataLexer_ == NULL)
		return E_LexerError;

	//test for object status
	sc = dataLexer_->getLexerStatus();
	if (sc != E_NoError)
		return sc;

	//Lex the header file
	gl_pZLexFile = this;

	dataStatus_ = dataLexer_->doLexData();
	dataError_ = dataLexer_->getLexerError();

	delete dataLexer_;
	dataLexer_ = NULL;

	delete fileBuffer_;
	fileBuffer_ = NULL;

	if (dataStatus_ != E_NoError)
		return E_LexerError;

	return E_NoError;
}

/***************************************************************************
 *
 * parseHeader
 *
 ***************************************************************************/
StatusCode ZLexFile::parseHeader() {

	//create a new buffer and fill the char* based buffer
	ZLexFileBuffer *fileBuffer = new ZLexFileBuffer();
	StatusCode fs = fileBuffer->fillFromFile(inputHeaderFileName_.c_str());

	//create a new parser with the header file buffer
	//todo: need a way to parse the file directly for extra large files
	ZLexParser *headerParser = new ZLexParser(this, fileBuffer);

	//test if object was created
	if (headerParser == NULL) {
		delete fileBuffer;
		return E_ParserError;
	}

	//test for object status
	StatusCode sc = headerParser->getParseStatus();
	if (sc != E_NoError) {
		delete fileBuffer;
		return sc;
	}

	//parse the header file
	gl_pZLexFile = this;

	headerStatus_ = headerParser->doParseHeader();
	headerError_ = headerParser->getParseError();

	delete fileBuffer;
	fileBuffer = NULL;

	delete headerParser;
	headerParser = NULL;

	if (headerStatus_ != E_NoError)
		return E_ParserError;

	sc = initHeader();
	if (E_NoError != sc)
		return sc;

	return E_NoError;
}

/***************************************************************************
 *
 * parseData
 *
 ***************************************************************************/
StatusCode ZLexFile::parseData() {
	/*
	 StatusCode sc = initData();
	 if(E_NoError != sc)
	 return sc;
	 */
	//create a new buffer and fill the char* based buffer
	ZLexFileBuffer *fileBuffer = new ZLexFileBuffer();
	StatusCode fs = fileBuffer->fillFromFile(inputDataFileName_.c_str());

	//create a new parser with the data file buffer
	//todo: need a way to parse the file directly for extra large files
	ZLexParser *dataParser = new ZLexParser(this, fileBuffer);

	//test if object was created
	if (dataParser == NULL) {
		delete fileBuffer;
		return E_ParserError;
	}

	//test for object status
	StatusCode sc = dataParser->getParseStatus();
	if (sc != E_NoError) {
		delete fileBuffer;
		return sc;
	}

	//parse the data file
	gl_pZLexFile = this;

	dataStatus_ = dataParser->doParseData();
	dataError_ = dataParser->getParseError();

	delete fileBuffer;
	fileBuffer = NULL;

	delete dataParser;
	dataParser = NULL;

	if (dataStatus_ != E_NoError)
		return E_ParserError;

	return E_NoError;
}

/***************************************************************************
 *
 * ZLexFile removeLineFromBuffer
 *
 ***************************************************************************/
StatusCode ZLexFile::removeLineFromBuffer(int lineNo) {
	return E_NoError;
}

/***************************************************************************
 *
 * ZLexFile parseHeaderLine
 * create a root section containing a generic file of line entries
 *
 ***************************************************************************/
/*
 int ZLexFile::parseFileLine(std::string& s)
 {
 int nRetVal = RET_NO_ERR;

 //create a parser with the header line
 m_pFileParser = new ZLexParser(s);

 //parse the file
 try {
 LexError sc = m_pFileParser->doFileLineParse();
 //the parser should now have a linked list of parsed tokens
 //insert the parser tokens into the zfile object header section
 sectionList_->addEntryLine(m_pFileParser->getTokenList(),false);
 }
 catch (...) {
 int oops = -1;
 }

 //the file now has a section containing an entry of elements
 //parser is no longer needed
 delete m_pFileParser;
 m_pFileParser = 0;

 return nRetVal;
 }
 */

/***************************************************************************
 *
 * ZLexFile parseHeaderLine
 * create a header section containing a header entry of field elements
 *
 ***************************************************************************/
//create a header section containing a header entry of field elements
/*
 int ZLexFile::parseHeaderLine(std::string& s)
 {
 int nRetVal = RET_NO_ERR;

 //create a parser with the header line
 m_pHeaderParser = new ZLexParser(s);
 //m_pHeaderParser = new ZLexParser(zfile->getHeaderLine());

 //parse the header
 m_pHeaderParser->doHeaderLineParse();

 //create a header section
 ZLexFileSection* newSection = new ZLexFileSection("header");

 //the parser should now have a linked list of parsed tokens

 //insert the parser tokens into the zfile object header section
 newSection->addEntryLine(m_pHeaderParser->getTokenList(),false);

 addSection(newSection, false);

 //the file now has a section containing a header entry of elements
 //parser is no longer needed
 delete m_pHeaderParser;
 m_pHeaderParser = 0;

 return nRetVal;
 }
 */

/***************************************************************************
 *
 * parseDataLine
 * create a data section containing a header entry of field elements
 *
 ***************************************************************************/
/*
 int ZLexFile::parseDataLine(std::string& s)
 {
 int nRetVal = RET_NO_ERR;

 //create a parser with the header line
 m_pDataParser = new ZLexParser(s);
 //m_pHeaderParser = new ZLexParser(zfile->getHeaderLine());

 //parse the header
 m_pDataParser->doDataLineParse();

 //create a header section
 ZLexFileSection* newSection = new ZLexFileSection("data");

 //the parser should now have a linked list of parsed tokens

 //insert the parser tokens into the zfile data section
 newSection->addEntryLine(m_pDataParser->getTokenList(),false);

 addSection(newSection, false);

 //the file now has a section containing a header entry of elements
 //parser is no longer needed
 delete m_pDataParser;
 m_pDataParser = 0;

 return nRetVal;
 }
 */

/***************************************************************************
 *
 * GetSection
 *
 ***************************************************************************/
/*
 ZLexFileSection* ZLexFile::getSection(int id)
 {
 ZLexFileSection* sec = sectionList_;

 while (sec != NULL)
 {
 if(sec->getSectionId() == id)
 {
 return sec;
 }
 sec = sec->getNextPtr();
 }
 return (ZLexFileSection*) NULL;
 }
 */

//---------------------------------------------------------------------------
/*
 LexError ZLexFile::parseBuffer(char* text)
 {
 LexError rc       = E_LEX_OK;
 LexError rc_parse = E_LEX_OK;
 LexError rc_load  = E_LEX_OK;

 char* newtext = NULL;

 if(text == NULL)
 return E_LEX_FILE_ERROR;

 int textLen = strlen(text)+1;
 newtext = (char*) malloc(textLen * sizeof(char));
 memset(newtext, '\0', textLen);
 strcpy(newtext, text);

 try{
 //    rc = rc_parse = (LexError) ParseLexFromBuffer(newtext);
 }
 catch(...)
 {
 return E_LEX_EXCEPTION;
 }

 try{
 rc = rc_load = Load();
 }
 catch(...)
 {
 return E_LEX_EXCEPTION;
 }

 free(newtext);

 //destroy all parser memory
 // todo: copy global error info
 //LexDestroy();

 if(rc_parse != E_LEX_OK)
 return rc_parse;

 return rc;
 }

 //---------------------------------------------------------------------------
 LexError ZLexFile::parseFile()
 {
 LexError rc       = E_LEX_OK;
 LexError rc_parse = E_LEX_OK;
 LexError rc_load  = E_LEX_OK;
 StatusCode sc;
 //FILE *fXmlOutput;

 if(m_szName == NULL)
 return E_LEX_FILE_ERROR;

 try{
 //test: find the first data line within this file
 //rc = rc_parse = (LexError) ParseLexLinesFromFile(m_szName);
 //		rc = rc_parse = (LexError) ParseLexFromFile(m_szName);
 if(rc == RET_PARSE_HEADER_OK)
 {
 sc = CreateXmlSchemaFilePass1("office","contact","d:\\dw\\schema",m_pCurrentSection);
 //sc = BuildXmlSchemaFile(m_pCurrentSection->getFirstEntry(),
 //	xmlOutputFile_, LEXFILESECTION_HEADING);
 }
 }
 catch(...)
 {
 return E_LEX_EXCEPTION;
 }

 try{
 rc = rc_load = Load();
 }
 catch(...)
 {
 return E_LEX_EXCEPTION;
 }
 //destroy all parser memory
 // todo: copy global error info
 //LexDestroy();

 if(rc_parse != E_LEX_OK)
 return rc_parse;

 return rc;
 }

 //-------------------------------------------------------------------------
 LexError ZLexFile::load()
 {

 return E_LEX_OK;
 }
 */

/***************************************************************************
 *
 * addSection
 *
 ***************************************************************************/
StatusCode ZLexFile::addSection(ZLexFileSection *sect, bool begin) {
	if (sect == NULL)
		return E_BadParameter;

	ZLexFileSection *pNew = sect;
	if (sectionList_ == NULL) {
		sectionList_ = pNew;
		listSize_ = 1;
	} else {
		if (begin) {
			//insert section at beginning of list;
			pNew->setNextPtr(sectionList_);
			sectionList_->setPrevPtr(pNew);
			sectionList_ = pNew;
			listSize_++;
		} else {
			//traverse the list to the end
			ZLexFileSection *pList = sectionList_;
			while (pList->getNextPtr() != NULL) {
				pList = pList->getNextPtr();
			}
			pNew->setPrevPtr(pList);
			pList->setNextPtr(pNew);
			listSize_++;
		}
	}

	//m_pCurrentSection = pNew;

	return E_NoError;
}

/***************************************************************************
 *
 * AddSection
 *
 ***************************************************************************/
/*
 ZLexFileSection* ZLexFile::addSection(char* szEntry, ZLexElement* aTok, bool begin)
 {
 if (szEntry == NULL) return NULL;

 ZLexFileSection *pThis = sectionList_;
 ZLexFileSection *pNew = NULL;
 if (sectionList_ == NULL)
 {
 sectionList_ = new ZLexFileSection(szEntry, aTok);
 if (sectionList_ == NULL) return NULL;
 pNew = sectionList_;
 listSize_ = 1;
 }
 else
 {
 pNew = new ZLexFileSection(szEntry, aTok);
 if (pNew == NULL) return NULL;

 if(begin)
 {
 sectionList_->setPrevPtr(pNew);
 pNew->setNextPtr(sectionList_);
 sectionList_ = pNew;
 }
 else
 {
 while (pThis->getNextPtr() != NULL)
 {
 pThis = pThis->getNextPtr();
 }
 pThis->setNextPtr(pNew);
 }
 listSize_++;
 }
 m_pCurrentSection = pNew;
 return pNew;
 }
 */

/***************************************************************************
 *
 * AddSection
 *
 ***************************************************************************/
/*
 ZLexFileSection* ZLexFile::addSection(char* szEntry, LexFileSection i, bool begin)
 {
 if (szEntry == NULL) return NULL;

 ZLexFileSection* pThis = sectionList_;
 ZLexFileSection* pNew = NULL;
 if (sectionList_ == NULL)
 {
 sectionList_ = new ZLexFileSection(szEntry, i);
 if (sectionList_ == NULL) return NULL;
 pNew = sectionList_;
 listSize_ = 1;

 }
 else
 {
 pNew = new ZLexFileSection(szEntry,i);
 if (pNew == NULL) return NULL;

 if(begin)
 {
 sectionList_->setPrevPtr(pNew);
 pNew->setNextPtr(sectionList_);
 sectionList_ = pNew;
 }
 else
 {
 while (pThis->getNextPtr() != NULL)
 {
 pThis = pThis->getNextPtr();
 }
 pThis->setNextPtr(pNew);
 }
 listSize_++;
 }
 m_pCurrentSection = pNew;
 return pNew;
 }
 */
/***************************************************************************
 *
 * GetNextSection
 *
 ***************************************************************************/

ZLexFileSection* ZLexFile::getNextSection(bool bStart) {
	StatusCode eStatus = E_NoError;

	if (bStart) {
		currentSection_ = sectionList_;
		eStatus = E_NoError;
		return currentSection_;
	}

	if (currentSection_ != NULL) {
		currentSection_ = currentSection_->getNextPtr();
		eStatus = E_NoError;
	} else {
		eStatus = E_NoMoreEntries;
	}

	return currentSection_;
}

/***************************************************************************
 *
 * DeleteSections
 *
 ***************************************************************************/
StatusCode ZLexFile::deleteSections() {
	ZLexFileSection *pLast;
	ZLexFileSection *pThis;
	if (sectionList_ != NULL) {
		pThis = sectionList_;
		while (pThis != NULL) {
			pLast = pThis;
			pThis = pThis->getNextPtr();
			delete (pLast);
			pLast = NULL;
		}
	}

	return E_NoError;
}

/***************************************************************************
 *
 * DeleteSection
 *
 ***************************************************************************/
/*
 StatusCode  ZLexFile::deleteSection(LEXFILESECTION section)
 {
 ZLexFileSection* pThis = findSection(section,true);
 ZLexFileSection* pPrev = pThis->getPrevPtr();
 ZLexFileSection* pNext = pThis->getNextPtr();

 if (pThis != NULL)
 {
 if(sectionList_ == pThis)
 sectionList_ = pNext;

 if(pPrev != NULL)
 //if(pNext != NULL)
 pPrev->setNextPtr(pNext);

 if(pNext != NULL)
 //if(pNext != NULL)
 pNext->setPrevPtr(pPrev);

 delete pThis;
 listSize_--;
 }
 else
 return E_BadParameter;

 return E_NoError;

 }
 */

//if//(szEntry//==//NULL)//return//NULL;
//ZLexFileSection*//pThis//=//sectionList_;
//ZLexFileSection*//pNew//=//NULL;
//if//(sectionList_//==//NULL)
//{
////sectionList_//=//new//ZLexFileSection(szEntry,//i);
////if//(sectionList_//==//NULL)//return//NULL;
////pNew//=//sectionList_;
////listSize_//=//1;
//}
//else
//{
////pNew//=//new//ZLexFileSection(szEntry,i);
////if//(pNew//==//NULL)//return//NULL;
////while//(pThis->getNext()//!=//NULL)
////{
//////pThis//=//pThis->getNext();
////}
////pThis->setNext(pNew);
////listSize_++;
//}
//m_pCurrentSection//=//pNew;
//return//pNew;
/***************************************************************************
 *
 * FindSection
 *
 ***************************************************************************/
/*
 ZLexFileSection* ZLexFile::findSection(LexFileSection section, bool begin)
 {

 static ZLexFileSection* sec;

 if(begin)
 sec = sectionList_;

 while (sec != NULL)
 {
 if(sec->getSectionId() == section)
 {
 return (ZLexFileSection*) sec;
 }
 sec = sec->getNextPtr();
 }

 return ((ZLexFileSection*) NULL);
 }
 */

/***************************************************************************
 *
 * FindEntry
 *
 * return the entry that contains a token of type desc with a name of srch 
 ***************************************************************************/
/*
 ZLexEntry* ZLexFile::findEntry(ZLexFileSection* section, TokenDesc desc, char* srch, bool begin)
 {
 //static ZLexFileSection *section = (ZLexFileSection*) NULL;
 static ZLexEntry* entry   = (ZLexEntry*) NULL;
 static ZLexElement* token   = (ZLexElement*) NULL;

 char srch1[1024], srch2[1024];

 if (section == NULL)
 return ((ZLexEntry*) NULL);

 //get the first or next entry based on passed bool "begin"
 //if(begin)
 entry = section->getNextEntry(begin);

 while(entry != NULL)
 {
 //get the first token of this entry
 token = entry->getNextElement(true);
 while(token != NULL)
 {
 if(token->getDesc() == desc)
 {
 if(srch == (char*) NULL)
 { // found first match
 return (ZLexEntry*) entry;
 }
 else
 {
 token->getText(srch1,sizeof(srch1)-1);
 if(srch1 != NULL)
 { //convert to same case
 strcpy(srch2,srch);
 if(stricmp(srch1,srch2) == 0)
 {
 return((ZLexEntry*) entry);
 }
 }
 }
 }
 token = entry->getNextElement(false);
 }
 entry = section->getNextEntry(false);
 }

 return ((ZLexEntry*) NULL);
 }
 */

/***************************************************************************
 *
 * findEntry
 *
 ***************************************************************************/
/*
 ZLexEntry* ZLexFile::findEntry(FileSection sec, TokenDesc desc, char* srch, bool begin)
 {
 static ZLexFileSection* section = (ZLexFileSection*) NULL;
 static ZLexEntry*   entry   = (ZLexEntry*) NULL;
 static ZLexElement*   token   = (ZLexElement*) NULL;

 char srch1[100], srch2[100];

 //get the section object
 section = GetSection(sec);
 if (section == NULL)
 return ((ZLexEntry*) NULL);

 //get the first or next entry based on passed bool "begin"
 if(begin)
 entry = section->getNextEntry(true);

 while(entry != NULL)
 {
 //get the first token of this entry
 token = entry->getNextElement(true);
 while(token != NULL)
 {
 if(token->getDesc() == desc)
 {
 if(srch == (char*) NULL)
 { // found first match
 return (ZLexEntry*) entry;
 }
 else
 if(token->getText() != NULL)
 { //convert to same case
 strcpy(srch1,srch);
 lcase(srch1);
 strcpy(srch2,token->getText());
 lcase(srch2);
 if(strcmp(srch1,srch2) == 0)
 {
 return((ZLexEntry*) entry);
 }
 }
 }
 token = entry->getNextElement(false);
 }
 entry = section->getNextEntry(false);
 }

 return ((ZLexEntry*) NULL);
 }
 */

/***************************************************************************
 *
 * FindToken
 *
 ***************************************************************************/
/*
 ZLexElement* ZLexFile::findToken(ZLexEntry* entry, TokenDesc desc, char* srch, bool begin)
 {
 static ZLexElement* token;

 char srch1[1024], srch2[1024];

 if(begin)
 {
 while(entry != NULL)
 {
 token = entry->getNextElement(true);
 while(token != NULL)
 {
 if(token->getDesc() == desc)
 {
 if(srch == (char*) NULL)
 { // found first match
 return (ZLexElement*) token;
 }
 else
 {
 token->getText(srch1,1024);
 if(srch1 != NULL)
 { //convert to same case
 strcpy(srch2,srch);
 if(stricmp(srch1,srch2) == 0)
 {
 return((ZLexElement*) token);
 }
 }
 }
 }
 token = entry->getNextElement(false);
 }
 entry = entry->getNextPtr();
 }
 }
 return ((ZLexElement*) NULL);
 }
 */

/***************************************************************************
 *
 * FindElement
 *
 ***************************************************************************/
/* 
 ZLexElement* ZLexFile::findElement(FileSection section, TokenDesc desc, char* srch, bool begin)
 {
 static ZLexFileSection* sec  = (ZLexFileSection *) NULL;
 static ZLexEntry*   ent  = (ZLexEntry *) NULL;
 static ZLexElement* elmt = (ZLexElement*) NULL;

 char srch1[100], srch2[100];

 if(begin)
 {
 //sec = sectionList_;
 sec = GetSection(section);
 if(sec != NULL)
 {
 //if(sec->getSectionId() == section)
 //{
 ent = sec->getNextEntry(true);
 while(ent != NULL)
 {
 elmt = ent->getNextElement(true);
 while(elmt != NULL)
 {
 if(elmt->getDesc() == desc)
 {
 if(srch == (char*) NULL)
 { // found first match
 return (ZLexElement*) elmt;
 }
 else
 if(elmt->getText() != NULL)
 { //convert to same case
 strcpy(srch1,srch);
 lcase(srch1);
 strcpy(srch2,elmt->getText());
 lcase(srch2);
 if(strcmp(srch1,srch2) == 0)
 {
 return((ZLexElement*) elmt);
 }
 }
 }
 elmt = ent->getNextElement(false);
 }
 ent = sec->getNextEntry(false);
 }
 //}
 //sec = sec->getNext();
 }
 }
 return ((ZLexElement*) NULL);
 }
 */

/***************************************************************************
 *
 * ZLexFile toString
 *
 ***************************************************************************/
const std::string ZLexFile::toString() const {
	std::ostringstream streamOut;

	//get a handle to the section list
	ZLexFileSection *zsection = sectionList_;

	//test
	std::string s = zsection->toXmlSchema();

	streamOut << zsection->toXmlSchema();

	return streamOut.str();
}

/***************************************************************************
 *
 * toXmlSchema
 *
 ***************************************************************************/
const std::string ZLexFile::toXmlSchema() const {
	std::ostringstream streamOut;

	//get a handle to the section list
	ZLexFileSection *zsection = sectionList_;

	//test
	std::string s = zsection->toXmlSchema();

	streamOut << zsection->toXmlSchema();

	return streamOut.str();
}

/***************************************************************************
 *
 * toXmlSchemaFile
 *
 ***************************************************************************/
StatusCode ZLexFile::toXmlSchemaFile(const char *szDbName, const char *szDtName,
		const char *szOutPath) {
	prepareXmlOutputFile(szDbName, szDtName, szOutPath);
	if (!xmlOutputFileOpen_)
		return E_FileError;

	FILE *fOutput = xmlOutputFile_;
	fclose(fOutput);

	//write preamble
	fOutput = fopen(xmlOutputFileSpec_.c_str(), "wb");

	fclose(fOutput);
	fOutput = fopen(xmlOutputFileSpec_.c_str(), "ab");
	fprintf(fOutput,
			"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\" ?>\n");
	fprintf(fOutput,
			"<!DOCTYPE database SYSTEM \"http://db.apache.org/torque/dtd/database_3_1.dtd\">\n");

	/*
	 <database
	 name="office"
	 defaultIdMethod="@DATABASE_ID_METHOD@">

	 <!-- =================================================== -->
	 <!-- C O N T A C T   T A B L E                           -->
	 <!-- =================================================== -->

	 <table name="contact" description="Contact Table" idMethod="native">

	 */
	std::string sDbRootDir = szDbName;
	std::string sDtRootDir = szDtName;

	fprintf(fOutput,
			"<database \n\tname=\"%s\"\n\t\tdefaultIdMethod=\"@DATABASE_ID_METHOD@\">\n",
			sDbRootDir.c_str());
	fprintf(fOutput,
			"\t<table name=\"%s\" description=\"Contact Table\" idMethod=\"native\">\n",
			sDtRootDir.c_str());
	fprintf(fOutput,
			"\t<column name=\"id\" required=\"true\" primaryKey=\"true\" type=\"INTEGER\" description=\"id\" autoIncrement=\"true\"/>\n");

	//get a handle to the section list
	ZLexFileSection *aHeader = sectionList_;
	ZLexEntry *entry = aHeader->getFirstEntry();

	bool bReadNext = true;
	// traverse entries
	while (entry != NULL) {
		buildXmlSchemaFileSection(entry, fOutput, LEXFILESECTION_HEADING);
		entry = aHeader->getNextEntry(false);
	}

	/*
	 </table>
	 </database>
	 */

	fprintf(fOutput, "\t</table>\n");
	fprintf(fOutput, "</database>\n");

	fclose(fOutput);
	xmlOutputFileOpen_ = false;

	return E_NoError;
}

/***************************************************************************
 *
 * BuildXmlSchemaFileSection
 *
 ***************************************************************************/
StatusCode ZLexFile::buildXmlSchemaFileSection(ZLexEntry *aHeader /*in*/,
		FILE *fOutput, LexFileSection sec /*out*/) {

	/*
	 // write the data to the file
	 // a 16 bit value  and a String
	 printf("Writing uint16_t value of %d\n", intValue);
	 udata_write16(pData, intValue);
	 printf("Writing string value of %s\n", stringValue);
	 udata_writeString(pData, stringValue, sizeof(stringValue));
	 */

	/* Torque column attributes
	 name=
	 javaName=
	 primaryKey=
	 required=
	 type=
	 javaType=
	 size=
	 default=
	 autoIncrement=
	 inheritance=
	 inputValidator=
	 javaNamingMethod=
	 description=
	 */

	char cDesc[1024];
	char cName[1024];
	char cText[1024];
	bool bReadNext = true;

	ZLexElement *elmt = aHeader->getFirstElement();

// traverse parser tokens looking for sections
	while (elmt != NULL) {
		//if(elmt->section == LEXFILESECTION_HEADING)
		//if(elmt->getSection() == sec)
		//{

		//write widestring
		if (elmt->getTokenType() == LEXTOKEN_FIELDNAME) {

			GetLexTokenDataTypeDesc(elmt->getDataType(), cDesc,
					sizeof(cDesc) - 1);
			elmt->getName(cName, sizeof(cName) - 1);
			elmt->getText(cText, sizeof(cText) - 1);
			LexDataType test = elmt->getDataType();
			//to print size or not print size, that is the question,...whether tis nobler...
			if ((elmt->getDataType() == LEXTOKEN_DATATYPE_BLOB)
					|| (elmt->getDataType() == LEXTOKEN_DATATYPE_INTEGER)) {
				//print the line without the size attribute
				//<column name="Number" required="false" type="INTEGER" description="Number" />
				fprintf(fOutput,
						"\t\t\t<column name=\"%s\" required=\"%s\" type=\"%s\" description=\"%s\" />\n",
						cName, // fieldname
						"false", // rkc: add to token???
						cDesc, // datatype representation
						cText       // field description text
						);
			} else {
				//print the line with the size attribute
				//<column name="Title" required="false" type="VARCHAR" size="255" description="Title" />

				//char cTextTemp[1024] = toupper(cText);

				std::string s(cText);

				std::transform(s.begin(), s.end(), s.begin(), ::toupper);

				rtrim(s);

				replaceIllegalFieldChars(s);

				fprintf(fOutput,
						"\t\t\t<column name=\"%s\" required=\"%s\" type=\"%s\" size=\"%d\" description=\"%s\" />\n",
						s.c_str(), // fieldname
						"false", // rkc: add to token???
						cDesc,            // datatype representation
						elmt->getLength(), // length of scanned text
						cText             // field description text
						);
			}
		}
		//}
		elmt = aHeader->getNextElement(false);
	}

	return E_NoError;
}

void ZLexFile::showHeader() {

	char cText[1024];

	//get a handle to the section list
	ZLexFileSection *aHeader = sectionList_;
	ZLexEntry *entry = aHeader->getFirstEntry();

	ZLexElement *elmt = NULL;

	// traverse entries
	while (entry != NULL) {

		elmt = entry->getFirstElement();

		while (elmt != NULL) {

			if (elmt->getTokenType() == LEXTOKEN_FIELDNAME) {

				elmt->getText(cText, sizeof(cText) - 1);

				std::string s(cText);

				std::cout << s << std::endl;
			}
			elmt = entry->getNextElement(false);
		}

		entry = aHeader->getNextEntry(false);
	}
}

void ZLexFile::showData() {

	char cText[1024];

	//get a handle to the section list
	//get a handle to the section list
	ZLexFileSection *aData = sectionList_->getNextPtr();
	ZLexEntry *entry = aData->getFirstEntry();

	ZLexElement *elmt = NULL;

	// traverse entries
	while (entry != NULL) {

		elmt = entry->getFirstElement();

		while (elmt != NULL) {

			if (elmt->getTokenType() == LEXTOKEN_FIELDVALUE) {

				elmt->getText(cText, sizeof(cText) - 1);

				std::string s(cText);

				std::cout << "value =" << s << std::endl;

				std::cout << "column =" << elmt->getColumn() << std::endl;

				std::string test1 = fieldEntries_[elmt->getColumn()];

				int x = test1.compare("NOTES");

				int y = s.compare("Untenberg\t");

				if(x==0){
					std::cout << "columnName =" << fieldEntries_[elmt->getColumn()] << std::endl;
				}

				if(y==0){
					std::cout << "Found Untenberg" << std::endl;
				}

				std::cout << "columnName =" << fieldEntries_[elmt->getColumn()] << std::endl;

			}
			elmt = entry->getNextElement(false);
		}

		entry = aData->getNextEntry(false);
	}
}

/***************************************************************************
 *
 * toSqlSchemaFile
 *
 ***************************************************************************/
StatusCode ZLexFile::toSqlSchemaFile(const char *szDbName, const char *szDtName,
		const char *szOutPath) {
	prepareSqlOutputFile(szDbName, szDtName, szOutPath);
	if (!sqlOutputFileOpen_)
		return E_FileError;

	FILE *fOutput = sqlOutputFile_;
	fclose(fOutput);

	fOutput = fopen(sqlOutputFileSpec_.c_str(), "wb");

	fclose(fOutput);
	fOutput = fopen(sqlOutputFileSpec_.c_str(), "ab");

	fprintf(fOutput, "drop table if exists contact;\n");

	fprintf(fOutput, "CREATE TABLE contact( \n");

	std::string sDbRootDir = szDbName;
	std::string sDtRootDir = szDtName;

	//get a handle to the section list
	ZLexFileSection *aHeader = sectionList_;
	ZLexEntry *entry = aHeader->getFirstEntry();

	// traverse entries
	while (entry != NULL) {
		buildSqlSchemaFileSection(entry, fOutput, LEXFILESECTION_HEADING);
		entry = aHeader->getNextEntry(false);
	}

	fprintf(fOutput, ");\n");

	fclose(fOutput);
	sqlOutputFileOpen_ = false;

	return E_NoError;
}

/***************************************************************************
 *
 * BuildSqlSchemaFileSection
 *
 ***************************************************************************/
StatusCode ZLexFile::buildSqlSchemaFileSection(ZLexEntry *aEntry /*in*/,
		FILE *fOutput, LexFileSection sec /*out*/) {

	char cDesc[1024];
	char cName[1024];
	char cText[1024];

	ZLexElement *elmt = aEntry->getFirstElement();

	fprintf(fOutput, "\t\t\tid INTEGER NOT NULL AUTO_INCREMENT,\n");

	// traverse parser tokens looking for fields
	while (elmt != NULL) {

		if (elmt->getTokenType() == LEXTOKEN_FIELDNAME) {

			GetLexTokenDataTypeDesc(elmt->getDataType(), cDesc,
					sizeof(cDesc) - 1);
			elmt->getName(cName, sizeof(cName) - 1);
			elmt->getText(cText, sizeof(cText) - 1);

			std::string s(cText);

			std::transform(s.begin(), s.end(), s.begin(), ::toupper);

			rtrim(s);

			replaceIllegalFieldChars(s);

			fprintf(fOutput, "\t\t\t%s  %s, \n", s.c_str(), // fieldname
					"TEXT"     // datatype representation
					);

		}
		elmt = aEntry->getNextElement(false);
	}

	fprintf(fOutput, "\t\t\t PRIMARY KEY ( %s ) \n",

	"id");

	return E_NoError;
}

/***************************************************************************
 *
 * toSqlDataFile
 *
 ***************************************************************************/
StatusCode ZLexFile::toSqlDataFile(const char *szDbName, const char *szDtName,
		const char *szOutPath) {
	prepareSqlDataOutputFile(szDbName, szDtName, szOutPath);
	if (!sqlDataOutputFileOpen_)
		return E_FileError;

	FILE *fOutput = sqlDataOutputFile_;
	fclose(fOutput);

	fOutput = fopen(sqlDataOutputFileSpec_.c_str(), "wb");

	fclose(fOutput);
	fOutput = fopen(sqlDataOutputFileSpec_.c_str(), "ab");

	std::string sDbRootDir = szDbName;
	std::string sDtRootDir = szDtName;

	//get a handle to the section list
	ZLexFileSection *aData = sectionList_->getNextPtr();
	ZLexEntry *entry = aData->getFirstEntry();
	ZLexElement *elmt = NULL;

	// traverse entries
	while (entry != NULL) {

		elmt = entry->getFirstElement();

		if (elmt != NULL) {

			fprintf(fOutput, "INSERT INTO %s ",

			"contact");

			buildSqlDataFileSection(entry, fOutput, LEXFILESECTION_DATA);

			fprintf(fOutput, "; \n"

			);
		}

		entry = aData->getNextEntry(false);
	}

	//fprintf(fOutput, ";\n");

	fclose(fOutput);
	sqlOutputFileOpen_ = false;

	return E_NoError;
}

/***************************************************************************
 *
 * BuildSqlDataFileSection
 *
 ***************************************************************************/
StatusCode ZLexFile::buildSqlDataFileSection(ZLexEntry *aData /*in*/,
		FILE *fOutput, LexFileSection sec /*out*/) {

	char cDesc[1024];
	char cName[1024];
	char cText[1024];

	ZLexElement *elmt = aData->getFirstElement();

	std::string fields;
	fields = "(";
	std::string values;
	values = "VALUES(";

	// traverse parser tokens looking for field values
	while (elmt != NULL) {

		if (elmt->getTokenType() == LEXTOKEN_FIELDVALUE) {

			GetLexTokenDataTypeDesc(elmt->getDataType(), cDesc,
					sizeof(cDesc) - 1);
			elmt->getName(cName, sizeof(cName) - 1);
			elmt->getText(cText, sizeof(cText) - 1);

			std::string s(cText);

			//all values need to be escaped with quotes if not already escaped

			if (s.length() > 0) {

				fields += fieldEntries_[elmt->getColumn()];
				fields = fields + ",";

				char test = s.at(0);

				if (test != 34) {

					s = "\"" + s + "\"";

				}

				values += s;
				values = values + ',';
			}



		}
		elmt = aData->getNextElement(false);
	}
	//remove last comma
	fields.pop_back();
	//remove last comma
	values.pop_back();

	fields = fields + ")";
	values = values +  ")";
	fprintf(fOutput, "%s %s", fields.c_str(), // fieldnames
			values.c_str() // fieldvalues
			);

	return E_NoError;
}

// trim from end (in place)
void ZLexFile::rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// replace
void ZLexFile::replaceIllegalFieldChars(std::string &s) {
// different member versions of find in the same order as above:
	std::size_t found = s.find("/");
	if (found != std::string::npos)
		s.replace(found, 1, "_");

	found = s.find("-");
	if (found != std::string::npos)
		s.replace(found, 1, "_");
}

/*****************************************************************************
 *
 * PrepareXmlOutputFile
 *
 *****************************************************************************/
bool ZLexFile::prepareXmlOutputFile(const char *szDbName, const char *szDtName,
		const char *szPath) {
//set the status flag
	xmlOutputFileOpen_ = false;

//build the file name
	xmlOutputFileName_ = szDtName;
	xmlOutputFileName_ += "-schema.xml";

//build the file spec
	xmlOutputFileSpec_ = szPath;
	xmlOutputFileSpec_ += SEPARATOR;
	xmlOutputFileSpec_ += szDbName;
	xmlOutputFileSpec_ += SEPARATOR;
	xmlOutputFileSpec_ += xmlOutputFileName_;

//test the file spec

	if (xmlOutputFileSpec_ == "") {
		if (xmlOutputFile_ == NULL) {
			xmlOutputFile_ = stdout;
		}
		// else, a stream was specified using -o
		xmlOutputFileOpen_ = false;
	} else {
		// try to open the file
		xmlOutputFile_ = fopen(xmlOutputFileSpec_.c_str(), "w");
		if (xmlOutputFile_ == NULL) {
			return xmlOutputFileOpen_ = false;
			//gl_pContext->OutputConsoleMsg(L_ERR,"\nError, file %s could not be opened/created for writing.\n",xmlOutputFile_.get());
			//gl_pContext->OutputConsoleMsg(L_ERR," restart with the \"-o\" option set.\n\n");
			//ShowHelp(false);
			//Exit(RET_FILE_ERR);
		}
		xmlOutputFileOpen_ = true;
	}

	return xmlOutputFileOpen_;
}

/*****************************************************************************
 *
 * PrepareSqlOutputFile
 *
 *****************************************************************************/
bool ZLexFile::prepareSqlOutputFile(const char *szDbName, const char *szDtName,
		const char *szPath) {
//set the status flag
	sqlOutputFileOpen_ = false;

//build the file name
	sqlOutputFileName_ = szDtName;
	sqlOutputFileName_ += "-schema.sql";

//build the file spec
	sqlOutputFileSpec_ = szPath;
	sqlOutputFileSpec_ += SEPARATOR;
	sqlOutputFileSpec_ += szDbName;
	sqlOutputFileSpec_ += SEPARATOR;
	sqlOutputFileSpec_ += sqlOutputFileName_;

//test the file spec

	if (sqlOutputFileSpec_ == "") {
		if (sqlOutputFile_ == NULL) {
			sqlOutputFile_ = stdout;
		}
		// else, a stream was specified using -o
		sqlOutputFileOpen_ = false;
	} else {
		// try to open the file
		sqlOutputFile_ = fopen(sqlOutputFileSpec_.c_str(), "w");
		if (sqlOutputFile_ == NULL) {
			return sqlOutputFileOpen_ = false;
			//gl_pContext->OutputConsoleMsg(L_ERR,"\nError, file %s could not be opened/created for writing.\n",sqlOutputFile_.get());
			//gl_pContext->OutputConsoleMsg(L_ERR," restart with the \"-o\" option set.\n\n");
			//ShowHelp(false);
			//Exit(RET_FILE_ERR);
		}
		sqlOutputFileOpen_ = true;
	}

	return sqlOutputFileOpen_;
}

/*****************************************************************************
 *
 * PrepareSqlDataOutputFile
 *
 *****************************************************************************/
bool ZLexFile::prepareSqlDataOutputFile(const char *szDbName,
		const char *szDtName, const char *szPath) {
//set the status flag
	sqlDataOutputFileOpen_ = false;

//build the file name
	sqlDataOutputFileName_ = szDtName;
	sqlDataOutputFileName_ += "-data.sql";

//build the file spec
	sqlDataOutputFileSpec_ = szPath;
	sqlDataOutputFileSpec_ += SEPARATOR;
	sqlDataOutputFileSpec_ += szDbName;
	sqlDataOutputFileSpec_ += SEPARATOR;
	sqlDataOutputFileSpec_ += sqlDataOutputFileName_;

//test the file spec

	if (sqlDataOutputFileSpec_ == "") {
		if (sqlDataOutputFile_ == NULL) {
			sqlDataOutputFile_ = stdout;
		}
		// else, a stream was specified using -o
		sqlDataOutputFileOpen_ = false;
	} else {
		// try to open the file
		sqlDataOutputFile_ = fopen(sqlDataOutputFileSpec_.c_str(), "w");
		if (sqlDataOutputFile_ == NULL) {
			return sqlDataOutputFileOpen_ = false;
			//gl_pContext->OutputConsoleMsg(L_ERR,"\nError, file %s could not be opened/created for writing.\n",sqlOutputFile_.get());
			//gl_pContext->OutputConsoleMsg(L_ERR," restart with the \"-o\" option set.\n\n");
			//ShowHelp(false);
			//Exit(RET_FILE_ERR);
		}
		sqlDataOutputFileOpen_ = true;
	}

	return sqlDataOutputFileOpen_;
}

/*
 int ZLexFile::getFieldCount() {
 //change this to the parser field count result
 return listSize_;
 }
 */

void ZLexFile::getHeaderElements(ZLexElement **ppList) {
//parse the first line of the file
//StatusCode sc = parseHeader();
//ZLexFileSection* zsection = getNextSection(true);
//ZLexEntry* zentry = zsection->getFirstEntry();
//*ppList = zentry->getFirstElement();

	ZLexFileSection *aHeader = sectionList_;
	ZLexEntry *aEntry = aHeader->getFirstEntry();
	ZLexElement *aElement = aEntry->getFirstElement();
	*ppList = aElement;
}

/*
 ZLexElement* ZLexFile::getHeaderElements() {
 ZLexFileSection* zsection = getNextSection(true);
 ZLexEntry* zentry = zsection->getFirstEntry();
 return zentry->getFirstElement();
 }

 ZLexElement* ZLexFile::getDataElements() {
 //parse the first line of the file
 //StatusCode sc = parseHeader();
 ZLexFileSection* zsection = getNextSection(true);
 ZLexEntry* zentry = zsection->getFirstEntry();
 return zentry->getFirstElement();
 }
 */

/***************************************************************************
 *
 * ZLexFile getFileLine
 *
 ***************************************************************************/
/*
 std::string ZLexFile::getFileLine()
 {
 std::string z = getLine(txtStream_);
 return z;
 }
 */

/***************************************************************************
 *
 * ZLexFile getHeaderLine
 *
 ***************************************************************************/
std::string ZLexFile::getHeaderLine() {
	std::string z = getLine(headerFileStream_);
	return z;
}

/***************************************************************************
 *
 * ZLexFile getDataLine
 *
 ***************************************************************************/
std::string ZLexFile::getDataLine() {
	std::string z = getLine(dataFileStream_);
	return z;
}

/***************************************************************************
 *
 * ZLexFile getLine
 *
 ***************************************************************************/
std::string ZLexFile::getLine(std::ifstream &inStream) {
	char c;
	std::string tempString;
	int i = 0;

//"help" the parser out by sticking a tab before expected field;
//tempString = '\t';

	while (!inStream.eof()) {
		inStream.get(c);
		tempString += (c);
		if (c == 0x0A) //line feed
			break;
		i++;
	}
	return tempString;
}

/***************************************************************************
 *
 * ZLexFile getAllLines
 *
 ***************************************************************************/
std::string ZLexFile::getAllLines(std::ifstream &inStream) {
	char c;
	std::string tempString;
	int i = 0;

//"help" the parser out by sticking a tab before expected field;
//tempString = '\t';

	while (!inStream.eof()) {
		inStream.get(c);
		tempString += (c);
		//if(c == 0x0A) //line feed
		//	break;
		i++;
	}
	return tempString;
}

/***************************************************************************
 *
 * open
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::openInputText()
 {
 txtFileStream_.open(inputTxtFileName_.c_str());

 if(!txtFileStream_.is_open())
 return E_FileError;

 return E_NoError;
 }
 */

/***************************************************************************
 *
 * openInputData
 *
 ***************************************************************************/
StatusCode ZLexFile::openInputData() {
	enum bufs {
		BASE, TXT, HEADER, DATA, HLINE1, DLINE1, BUFNUM
	};
	std::string part[BUFNUM];
//part[BASE] = fileName;
	part[BASE] = inputTxtFileName_.c_str();
// Find any '.' in the std::string:
	size_t loc = part[BASE].find('.');
	if (loc != std::string::npos)
		part[BASE].erase(loc); // Strip extension
// Force to upper case:
	for (int i = 0; i < part[BASE].size(); i++)
		part[BASE][i] = toupper(part[BASE][i]);
// Create file names and internal lines:
	part[TXT] = part[BASE] + ".txt";		//text file in
	part[HEADER] = part[BASE] + ".zta"; //header("T"able "A"ttributes) file out
	part[DATA] = part[BASE] + ".ztd";   //data("T"able "D"ata) file out

	dataFileStream_.open(part[HEADER].c_str());

	if (!dataFileStream_.is_open())
		return E_FileError;

	return E_NoError;
}

/***************************************************************************
 *
 * openInputHeader
 *
 ***************************************************************************/
StatusCode ZLexFile::openInputHeader() {

	enum bufs {
		BASE, TXT, HEADER, DATA, HLINE1, DLINE1, BUFNUM
	};
	std::string part[BUFNUM];
//part[BASE] = fileName;
	part[BASE] = inputTxtFileName_.c_str();
// Find any '.' in the std::string:
	size_t loc = part[BASE].find('.');
	if (loc != std::string::npos)
		part[BASE].erase(loc); // Strip extension
	/*
	 // Force to upper case:
	 for(int i = 0; i < part[BASE].size(); i++)
	 part[BASE][i] = toupper(part[BASE][i]);

	 */
// Create file names and internal lines:
	part[TXT] = part[BASE] + ".txt";		//text file in
	part[HEADER] = part[BASE] + ".zta"; //header("T"able "A"ttributes) file out
	part[DATA] = part[BASE] + ".ztd";   //data("T"able "D"ata) file out

//headerFileStream_.open(part[HEADER].c_str());
	std::string s = part[HEADER].c_str();
	headerFileStream_.open(part[HEADER].c_str());
	if (!headerFileStream_.is_open())
		return E_FileError;

	return E_NoError;
}

/***************************************************************************
 *
 * close
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::closeInputTxt()
 {
 txtStream_.close();

 if(txtStream_.is_open())
 return E_FileError;

 return E_NoError;
 }
 */
/***************************************************************************
 *
 * closeInputHeader
 *
 ***************************************************************************/
StatusCode ZLexFile::closeInputHeader() {
	headerFileStream_.close();

	if (headerFileStream_.is_open())
		return E_FileError;

	return E_NoError;
}

/***************************************************************************
 *
 * closeInputData
 *
 ***************************************************************************/
StatusCode ZLexFile::closeInputData() {
	dataFileStream_.close();

	if (dataFileStream_.is_open())
		return E_FileError;

	return E_NoError;
}

/***************************************************************************
 *
 * replaceAll
 *
 ***************************************************************************/
std::string& replaceAll(std::string &context, const std::string &from,
		const std::string &to) {
	size_t lookHere = 0;
	size_t foundHere;
	while ((foundHere = context.find(from, lookHere)) != std::string::npos) {
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
	return context;
} ///:~

/***************************************************************************
 *
 * removeAll
 *
 ***************************************************************************/
std::string removeAll(std::string &context, const std::string &srch) {
	size_t lookHere = 0;
	size_t foundHere;
	std::string aCopy(context);

	while ((foundHere = aCopy.find(srch, lookHere)) != std::string::npos) {
		//aCopy.replace(foundHere, srch.size(), to);
		int l1 = aCopy.length();
		//for(size_t i=foundHere; i < (foundHere + srch.size()); i++)
		aCopy.erase(foundHere, srch.size());
		int l2 = aCopy.length();
		lookHere = foundHere + srch.size();
	}

	return aCopy;
//return string(s,;
} ///:~

/***************************************************************************
 *
 * trim
 *
 ***************************************************************************/
inline std::string trim(const std::string &s) {
	if (s.length() == 0)
		return s;
	size_t beg = s.find_first_not_of(" \'\"");
	if (beg == std::string::npos) // No non-spaces
		return "";
	size_t end = s.find_last_not_of(" \'\"");
	return std::string(s, beg, end - beg + 1);
}

/***************************************************************************
 *
 * clean
 *
 ***************************************************************************/
inline std::string clean(const std::string &s) {
	if (s.length() == 0)
		return s;
	std::string trimmed = trim(s);

	replaceAll(trimmed, "\'", " ");

	replaceAll(trimmed, "\r", " ");

	std::string cleaned = removeAll(trimmed, " ");
	return cleaned;
}

/***************************************************************************
 *
 * cleanHeader
 *
 ***************************************************************************/
std::string ZLexFile::cleanHeader(std::string headerString) {
// vector of strings to store the header tokens
	std::vector<std::string> strings;
// The '\t' character will be delimiters
//string s("now.;sense;make;to;going;is;This");
	std::string s(headerString);
// The last element of the string:
	int last = s.size();
// The beginning of the current token:
	size_t current = s.rfind('\t');
// Walk backward through the string:
	while (current != std::string::npos) {
		// Push each token into the vector.
		// Current is incremented before copying
		// to avoid copying the delimiter:
		++current;
		strings.push_back(s.substr(current, last - current));
		//strings.push_front(s.substr(current, last - current));
		// Back over the delimiter we just found,
		// and set last to the end of the next token:
		current -= 2;
		last = current + 1;
		// Find the next delimiter:
		current = s.rfind('\t', current);
	}
// Pick up the first token -- it's not
// preceded by a delimiter:
	strings.push_back(s.substr(0, last));
//strings.push_front(s.substr(0, last));

	std::string sentence;
//for(size_t i = 0; i < strings.size() - 1; i++)
	for (int i = strings.size() - 1; i >= 0; i--)
		//clean each token, strip quotes, space, apostrophies
		sentence += clean(strings[i]) += "\t";
// Manually put last token in to avoid an extra space:
//sentence += strings[strings.size() - 1];

	return sentence;
}

/***************************************************************************
 *
 * joinFiles
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::joinFiles(const char* fname) {
 StatusCode retval = E_NoError;
 // now join the files
 if (fOutDetail != NULL)
 {
 rewind(fOutDetail);
 fprintf(gl_pContext->getOutputLogPtr(),"\n\n** Package detail **\n");
 c = fgetc(fOutDetail);
 while (!feof(fOutDetail))
 {
 fputc(c,gl_pContext->getOutputLogPtr());
 c = fgetc(fOutDetail);
 }
 fclose(fOutDetail);
 remove(cFileSpecDetail);
 }

 return retval;
 }
 */
/***************************************************************************
 *
 * fillBufferFromBuf(output,input,outputsize)
 *
 * preconditions: 
 * the input buffer has a delimitter preceeding any text that is to be scanned
 *
 * postconditions:
 * the output buffer will have a trailing new-line char to ensure proper lexer
 * termination
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::fillBufferFromBuf(ZLexFileBuffer* fileBuffer_, const char* inputBuffer, int& byteCounter)
 {
 StatusCode retval = E_NoError;
 size_t byteCount = 0;
 int	nextChar = 0;
 //bool    b_unix = false;

 // initialize the file inputBuffer
 fileBuffer_->buffer_ = (char*)NULL;
 //b_unix = IsFileUnixFormat(fname);

 if (inputBuffer == NULL) {
 retval = E_BufferEmpty;
 }
 else {
 fileBufferFree(fileBuffer_);
 //init the fileBuffer_ buffer
 //fileBuffer_->size_ = FIRSTBUFSIZE;
 //fileBuffer_->size_ = sizeof(inputBuffer + 1); //make room for "helper" '\n' char
 fileBuffer_->size_ = sizeof(inputBuffer); //make room for "helper" '\n' char

 //allocate buffer memory
 fileBuffer_->buffer_ = (char*)malloc(fileBuffer_->size_ * sizeof(char));
 //fileBuffer_->buffer_ = (char*)malloc(fileBuffer_->size_,"fileBuffer_->buffer_");
 //memset
 int chk = sizeof(fileBuffer_->buffer_);
 memset(fileBuffer_->buffer_,'\0',fileBuffer_->size_);
 fileBuffer_->offset_ = 0;
 if (fileBuffer_->buffer_ == (char*)NULL) {
 retval = E_BufferError;
 }
 else {
 //copy all the chars from input buffer to parser buffer
 while ((nextChar = inputBuffer[byteCount]) != NULL) {
 if (fileBufferPutChar(fileBuffer_,(unsigned char)nextChar) == E_BufferError) {
 retval = E_BufferError;
 break;
 }
 else {
 byteCount++;
 }
 }
 //"help" the lexer out by appending a newline ('\n') as the last char in line
 //so that a line of delimitted fields has, at minimum, a leading tab
 //and an ending new line.  This case would essentially be an empty field line
 //only add the new-line char if the last char is not a new-line
 int lastChar = fileBuffer_->buffer_[byteCount-1];
 if(lastChar != 10) {
 nextChar = 10;
 if (fileBufferPutChar(fileBuffer_,(unsigned char)nextChar) == E_BufferError) {
 retval = E_BufferError;
 }
 else {
 byteCount++;
 }
 }

 if (retval >= 0) {
 //reset the buffer offset and size
 fileBuffer_->offset_ = 0;
 fileBuffer_->size_ = byteCount;
 }
 }
 }
 byteCounter = byteCount;
 return retval;
 }
 */

/******************************************************************************
 *	fileBufferInit
 ******************************************************************************/
/*
 StatusCode ZLexFile::fileBufferInit(ZLexFileBuffer* fb, int size)
 {
 StatusCode retval = E_NoError;

 fileBufferFree(fb);

 fb->size_ = size;
 fb->buffer_ = (char*)malloc(size * sizeof(unsigned char));
 //fb->buffer_ = (char*)malloc(size * sizeof(unsigned char), "filebuffer");
 fb->offset_ = 0;
 if (fb->buffer_ != (char*)NULL)
 {
 return (fb->size_);
 }
 else
 {
 return 0;
 }
 }
 */
/******************************************************************************
 *	fileBufferFree
 ******************************************************************************/
/*
 StatusCode ZLexFile::fileBufferFree(ZLexFileBuffer* fb)
 {
 StatusCode retval = E_NoError;
 if (fb->buffer_ != (char*)NULL)
 {
 free(fb->buffer_);
 //free(fb->buffer_, "filebuffer");
 fb->buffer_ = (char*)NULL;
 }
 return E_NoError;
 }
 */

/******************************************************************************
 *	Write a char to memory.
 ******************************************************************************/
/*
 StatusCode ZLexFile::fileBufferPutChar(ZLexFileBuffer* fb, char byte)
 {
 StatusCode retval = E_NoError;

 if (fb->offset_ == fb->size_)
 {
 if (fileBufferEnlarge(fb) == E_BufferError)
 {
 return E_BufferError;
 }
 }
 fb->buffer_[fb->offset_++] = byte;

 return retval;
 }
 */

/******************************************************************************
 *	fileBufferEnlarge
 * double the output memory size.
 ******************************************************************************/
/*
 StatusCode ZLexFile::fileBufferEnlarge(ZLexFileBuffer* fb)
 {
 char* newbuf;

 newbuf = (char*)calloc((fb->size_ << 1) * sizeof(char), sizeof(char));
 if (newbuf != (char*)NULL)
 {
 memcpy(newbuf, fb->buffer_, fb->size_);
 free(fb->buffer_);
 //free(fb->buffer_,"fb->buffer_");
 fb->size_ <<= 1;
 fb->buffer_ = newbuf;
 }
 else
 return E_BufferError;

 return E_NoError;
 }
 */

/******************************************************************************
 *	return the validity of reading the next char in the buffer.
 ******************************************************************************/
/*
 bool ZLexFile::fileBufferCanRead(ZLexFileBuffer* fb)
 {
 bool	retval;

 if (fb->offset_ < fb->size_)
 {
 retval = true;
 }
 else
 {
 retval = false;
 }
 return retval;
 }
 */

/***************************************************************************
 * fileOpen
 ***************************************************************************/
/*
 FILE* ZLexFile::fileOpen(const char* fileName)
 {
 FILE* fp;

 if (fileName != (char*)NULL)
 {
 fp = fopen(fileName, "rb");
 }
 else
 {
 fp = (FILE*)NULL;
 }
 return (fp);
 }
 */

/******************************************************************************
 *	fileOpenExt
 ******************************************************************************/
/*
 FILE* ZLexFile::fileOpenExt(const char* name)
 {
 FILE* fp;

 if (name != (char*)NULL)
 {
 fp = fopen(name, "rb");
 }
 else
 {
 fp = (FILE*)NULL;
 }
 return (fp);
 }
 */
/***************************************************************************
 *
 * parseFile
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::parseFile()
 {
 //create a root section
 sectionList_ = new ZLexFileSection("root");

 //open the file
 if(openInput() != E_NoError)
 return E_FileError;

 int nColumns, nRows;
 //parse the file into rows and columns of cells
 //m_inStream
 //this is a bogus check
 while(!m_inStream.eof()) {
 nColumns = parseFileLine(getFileLine());
 nRows++;
 }

 if(nRows < 1)
 return E_ParserError;

 return E_NoError;

 //close the file
 closeInput();

 }
 */

/***************************************************************************
 *
 * fillBufferFromFile
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::fillBufferFromFile(const char* fname) {
 StatusCode retval = E_NoError;
 fileBuffer_ = new ZLexFileBuffer();
 retval = fileBuffer_->fillFromFile(fname);
 return retval;
 }
 */
/***************************************************************************
 *
 * writeBufferToFile
 *
 ***************************************************************************/
/*
 StatusCode ZLexFile::writeBufferToFile(const char* fname, int start) {
 StatusCode retval = fileBuffer_->writeToFile(fname,start);
 return retval;
 }
 */

