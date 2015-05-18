/*
 * drcomexception.h
 *
 *  Created on: May 16, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_DRCOMEXCEPTION_H_
#define SRC_DRCOMEXCEPTION_H_
#include <iostream>
#include <sstream>
#include <string.h>
using namespace std;
class drcomexception{


public:
	drcomexception(const std::string& message) : message(message) { }
	drcomexception(const std::string& message, int err) {
        std::stringstream stream;
        stream << message << ", errno = " << err << ", desc: " << strerror(err);
        this->message = stream.str();
    }
	const char* what() const throw() { return message.c_str(); }

    ~drcomexception() throw() {}

private:
	std::string message;


};

#endif /* SRC_DRCOMEXCEPTION_H_ */
