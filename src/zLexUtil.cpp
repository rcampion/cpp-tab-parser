/**************************************************************************
 *
 * zLexUtil.cpp
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *
 *
 **************************************************************************/
/*! \file zLexUtil.cpp
*/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "zLexUtil.h"

/*=========================================================================
 * memory allocation test variables
 *========================================================================*/
//int gl_mallocs;
//int gl_strdups;
//int gl_frees;

/*=========================================================================
 * memory allocation test functions
 *========================================================================*/
/*
void* MyMalloc(int size, char* desc)
{
	gl_mallocs++;

	#ifdef _DEBUG_MEM
	printf("\nmalloc: %5d %5d %s", gl_mallocs, size, desc);
	#endif

	return malloc(size);
}
*/
/*=========================================================================
 * memory allocation test functions
 *========================================================================*/
/*
char* MyStrdup(const char* str)
{
  // Unix is very unforgiving in case of NULL strings used in printf(), 
  // strlen() etc.

    if (str == NULL) {
      return NULL;
    }

	int size = strlen(str);
	gl_strdups++;

	#ifdef _DEBUG_MEM
	printf("\nstrdup: %5d %5d %s", gl_strdups, size, str);
	#endif

	return strdup(str);
}
*/
/*=========================================================================
 * memory allocation test functions
 *========================================================================*/
/*
void MyFree(void* item, char* desc)
{
	gl_frees++;

	#ifdef _DEBUG_MEM
	printf("\nfree    %5d %s", gl_frees, desc);
	#endif

	free(item);
}
*/
/******************************************************************************
*	RightTrim
******************************************************************************/
char* RightTrim(char * s)
{

	if ( (s == NULL) || (s[0] == '\0') ) return NULL;

	char c[500];
	int nLen = strlen(s);
	if (nLen < sizeof(c))
	{
		strcpy(c, s);
		while ( (nLen > 0) && ( (isspace(c[nLen-1])) || (c[nLen-1] == '\n') ) )
		{
			c[nLen-1] = '\0';
			nLen = strlen(c);
		}
		// copy it back
		strcpy(s, c);
	}
	return s;
}


/*****************************************************************************
*	ucase
*****************************************************************************/
void ucase(char* s)
{
	while (*s != '\0')
	{
		switch (*s)
		{
			case 'a': *s = 'A'; break;
			case 'b': *s = 'B'; break;
			case 'c': *s = 'C'; break;
			case 'd': *s = 'D'; break;
			case 'e': *s = 'E'; break;
			case 'f': *s = 'F'; break;
			case 'g': *s = 'G'; break;
			case 'h': *s = 'H'; break;
			case 'i': *s = 'I'; break;
			case 'j': *s = 'J'; break;
			case 'k': *s = 'K'; break;
			case 'l': *s = 'L'; break;
			case 'm': *s = 'M'; break;
			case 'n': *s = 'N'; break;
			case 'o': *s = 'O'; break;
			case 'p': *s = 'P'; break;
			case 'q': *s = 'Q'; break;
			case 'r': *s = 'R'; break;
			case 's': *s = 'S'; break;
			case 't': *s = 'T'; break;
			case 'u': *s = 'U'; break;
			case 'v': *s = 'V'; break;
			case 'w': *s = 'W'; break;
			case 'x': *s = 'X'; break;
			case 'y': *s = 'Y'; break;
			case 'z': *s = 'Z'; break;
			case '�': *s = '�'; break;
			case '�': *s = '�'; break;
			case '�': *s = '�'; break;
			default: break;
		}
		s++;
	}
}

/*****************************************************************************
*	lcase
******************************************************************************/
void lcase(char* s)
{
	while (*s != '\0')
	{
		switch (*s)
		{
			case 'A': *s = 'a'; break;
			case 'B': *s = 'b'; break;
			case 'C': *s = 'c'; break;
			case 'D': *s = 'd'; break;
			case 'E': *s = 'e'; break;
			case 'F': *s = 'f'; break;
			case 'G': *s = 'g'; break;
			case 'H': *s = 'h'; break;
			case 'I': *s = 'i'; break;
			case 'J': *s = 'j'; break;
			case 'K': *s = 'k'; break;
			case 'L': *s = 'l'; break;
			case 'M': *s = 'm'; break;
			case 'N': *s = 'n'; break;
			case 'O': *s = 'o'; break;
			case 'P': *s = 'p'; break;
			case 'Q': *s = 'q'; break;
			case 'R': *s = 'r'; break;
			case 'S': *s = 's'; break;
			case 'T': *s = 't'; break;
			case 'U': *s = 'u'; break;
			case 'V': *s = 'v'; break;
			case 'W': *s = 'w'; break;
			case 'X': *s = 'x'; break;
			case 'Y': *s = 'y'; break;
			case 'Z': *s = 'z'; break;
			case '�': *s = '�'; break;
			case '�': *s = '�'; break;
			case '�': *s = '�'; break;
			default: break;
		}
		s++;
	}
}
