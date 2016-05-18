//
//  exceptions.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/14/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#ifndef phoneagent_exceptions_h
#define phoneagent_exceptions_h

#include <exception>
#include <stdexcept>

class MyException: public std::runtime_error {
public:
	MyException(const std::string& message, int code, int line, const std::string file) : std::runtime_error(message), errorCode(code),
    errorLine(line), fromFile(file) {};
	virtual ~MyException() throw() {}; // virtual class, può essere ridefinita in ogni classe derivata
	virtual int getErrorCode() { return errorCode; };
	virtual int getLine() { return errorLine; };
	virtual std::string getFile() { return fromFile; };
private:
	const int errorCode;
	const int errorLine;
	const std::string fromFile;
};

#endif
