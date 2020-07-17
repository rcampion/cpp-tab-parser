/**************************************************************************
 *
 * zLexUtil.h
 *
 * author: Richard Campion
 *
 **************************************************************************/
/**************************************************************************
 *  Common C utility routine interface for lexer/parser
 *
 **************************************************************************/
/*! \file zLexUtil.h
*/

#ifndef ___ZLEXUTIL_H___
#define ___ZLEXUTIL_H___

/*========================================================================*/
extern int gl_mallocs;
extern int gl_strdups;
extern int gl_frees;

//void* MyMalloc(int size, char* desc);
//char* MyStrdup(const char* src);
//void  MyFree(void* item, char* desc);
//void	PrintMemUsage(FILE *fOutput);
//void	PrintMemUsage();
/*========================================================================*/

//char array string manipulation routines
char* RightTrim(char * s);
void ucase(char* s);
void lcase(char* s);

#endif