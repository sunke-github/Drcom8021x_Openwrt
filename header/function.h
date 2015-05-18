/*
 * function.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_FUNCTION_H_
#define SRC_FUNCTION_H_

#include "def.h"
#include <iostream>

class function {
public:
	function();
	void online();
	void offline();
	virtual ~function();


private:
	drcom_config conf;
	std::string conf_path = configpath;
	int alive_try;
    bool readflag;

};

#endif /* SRC_FUNCTION_H_ */
