/*****************************************************************************************************************
 *
 * zLexEceptions.h
 *
 ****************************************************************************************************************/
/***************************************************************************************************************
 *
 **************************************************************************************************************/
/*! \file zLexEceptions.h
 */

#ifndef ___ZLEXEXCEPTION_H___
#define ___ZLEXEXCEPTION_H___
#include <stdexcept>
#include <iostream>

class ZLexException: public std::runtime_error {
public:
	ZLexException(const std::string &msg = "") :
			std::runtime_error(msg) {
	}
};

#endif
