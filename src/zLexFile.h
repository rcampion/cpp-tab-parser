/**************************************************************************
 *
 * zLexFile.h  Lex File Class definition
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexFile.h
 */
/*

 Overview:
 This code is based on GNU Flex and GNU Bison to parse an ASCII text file.
 - Flex  : C scanner generator.
 - Bison : C parser generator.

 A ZLexFile is an abstraction of an ASCII tab delimited file containing tokens arranged in a
 common data export format of one exactly one line/row of field names followed by 1+n rows of data.
 Each file line separates fields by the tab '\t' character, the delimiter.
 The first line of a file (the headerline) can be scanned into to field name tokens.
 The Header Line does not know what the primitives of the data is until the data is lexed.
 The remaining rows of the file (datalines) are matched to the headerline by their respective tab/column position.
 This pattern of headerline/dataline is described in a Flex/Bison specification used to update the header with what the
 data prsentsparser grammar and lexer tokenizer.

 The input data file is split into two files.
 The contents of each ASCII text file are read into a buffer.
 Each file line is read from the filesystem and tokenized creating a LEXTOKEN.
 Each line placed into sections, entries, and elements.
 Each linked list is provided to a section object for insertion.

 Preconditions:
 - A file exists in the ASCII character set in tab delimited form.
 - The file contains a field header line as the first line of the file.
 - The remaining line(s), data lines that exist should be in the same column(s) as the field header line.

 Objective:
 To create an XML Database Schema file from a tab delimited data file exported from 'some' application.
 The database schema file can be used to create SQL database table field descriptors, datatypes and sizes.

 Example:
 A third party application offers a data export option into a file as a tab delimited file format.
 Using a ZLexFile object, an XML database schema file, (based on Apache Turbine/Torque format), is created specifying the
 database name, table name, field names, datatypes, and field sizes. and indices.  Using this Torque based
 XML generated file, Torque, and the original data export file, a database table is created.
 A "bulk load" of the data portion of the parsed file can then exported populate a RDBMS table.
 */

#ifndef __ZLEXFILE_H___
#define __ZLEXFILE_H___

#include "zLexError.h"
#include "zLexSection.h"
#include "zLexUtil.h"
#include "zDwConst.h"
#include "zCtrlDwRuntime.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

/***************************************************************************
 *
 * ZLexFile
 *
 **************************************************************************/
/** A ZLexFile contains a name of a file to be parsed by a ZLexParser.  
 */
class ZLexFile {
public:
	ZLexFile();
	ZLexFile(const char *szName);
	ZLexFile(ZRuntime *run);
	~ZLexFile();
	ZLexFile& operator=(const ZLexFile &rhs);
	StatusCode addSection(ZLexFileSection *section, bool begin);
	StatusCode deleteSections();
	std::string getLineBuffer() {
		return fileLineBuffer_;
	}
	;
	LexError getDataError() {
		return dataError_;
	}
	;
	StatusCode getDataStatus() {
		return dataStatus_;
	}
	;
	void getHeaderElements(ZLexElement **ppList);
	StatusCode getHeaderStatus() {
		return headerStatus_;
	}
	;
	LexError getHeaderError() {
		return headerError_;
	}
	;
	std::string getInputTxtFileName() {
		return inputTxtFileName_;
	}
	;
	StatusCode getInputTxtFileStatus() {
		return inputTxtFileStatus_;
	}
	;
	int getLineCount() {
		return lineCount_;
	}
	;

	ZLexFileSection* getSectionList() {
		return sectionList_;
	}
	;
	ZLexFileSection* getNextSection(bool bStart);

	StatusCode lexData();
	StatusCode lexFile();
	StatusCode lexHeader();
	StatusCode parseData();
	StatusCode parseFile();
	StatusCode parseHeader();
	const std::string toString() const;
	const std::string toXmlSchema() const;

	void showHeader();

	void showData();

	StatusCode toXmlSchemaFile(const char *szDbName, const char *szDtName,
			const char *szOutPath);

	StatusCode toSqlSchemaFile(const char *szDbName, const char *szDtName,
			const char *szOutPath);

	StatusCode toSqlDataFile(const char *szDbName, const char *szDtName,
			const char *szOutPath);

	void setHeaderError(LexError e) {
		headerError_ = e;
	}
	;

	void rtrim(std::string &s);
	void replaceIllegalFieldChars(std::string &s);

private:
	StatusCode buildXmlSchemaFileSection(ZLexEntry *aHeader, FILE *fOutput,
			LexFileSection sec);

	StatusCode buildSqlSchemaFileSection(ZLexEntry *aHeader, FILE *fOutput,
			LexFileSection sec);

	StatusCode buildSqlDataFileSection(ZLexEntry *aData, FILE *fOutput,
			LexFileSection sec);

	std::string cleanHeader(std::string headerString);
	StatusCode closeInputData();
	StatusCode closeInputHeader();
	//StatusCode   closeInputTxt();
	StatusCode createHeaderFile();
	StatusCode createDataFile();
	//StatusCode   fillBufferFromFile(const char* fname);
	std::string getAllLines(std::ifstream &inStream);
	std::string getDataLine();
	std::string getLine(std::ifstream &inStream);
	//std::string  getFileLine();
	std::string getHeaderLine();

	StatusCode initHeader();
	StatusCode openInputData();
	StatusCode openInputHeader();

	StatusCode prepareForLex();
	StatusCode prepareForParse();
	bool prepareXmlOutputFile(const char *szDbName, const char *szDtName,
			const char *szPath);
	bool prepareSqlOutputFile(const char *szDbName, const char *szDtName,
			const char *szPath);
	bool prepareSqlDataOutputFile(const char *szDbName, const char *szDtName,
			const char *szPath);

	StatusCode removeLineFromBuffer(int lineNo);
	//StatusCode   writeBufferToFile(const char* fname, int n);

private:
	ZLexFileSection *currentSection_;

	std::ifstream dataFileStream_;
	StatusCode dataStatus_;
	LexError dataError_;

	std::string fileLineBuffer_;     // text storage for lexer

	//is this right?, vector initialized within a header file?
	std::vector<std::string> fieldEntries_;

	std::ifstream headerFileStream_;
	StatusCode headerStatus_;
	LexError headerError_;

	std::string inputTxtFileName_;      //file to split
	StatusCode inputTxtFileStatus_;
	std::string inputDataFileName_;     //split file to parse
	StatusCode inputDataFileStatus_;
	std::string inputHeaderFileName_;   //split file to parse
	StatusCode inputHeaderFileStatus_;

	int lineCount_;     //number of lines in input
	int listSize_;      //number of file sections

	ZRuntime *runContext_; //pointer to runtime variables - currently only need the filename - this will change
	ZLexFileSection *sectionList_;		//list of file sections to iterate
	std::ifstream txtFileStream_;

	FILE *xmlOutputFile_;
	bool xmlOutputFileOpen_;
	std::string xmlOutputFileName_;
	std::string xmlOutputFileSpec_;

	FILE *sqlOutputFile_;
	bool sqlOutputFileOpen_;
	std::string sqlOutputFileName_;
	std::string sqlOutputFileSpec_;

	FILE *sqlDataOutputFile_;
	bool sqlDataOutputFileOpen_;
	std::string sqlDataOutputFileName_;
	std::string sqlDataOutputFileSpec_;

};

extern ZLexFile *gl_pZLexFile;

#endif // __ZLEXFILE_H */
