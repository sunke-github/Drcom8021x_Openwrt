/*
 * config.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_


#include "def.h"
#include <iostream>
#include <string.h>
using namespace std;


class config {
public:
	config();

	/*
	 * read Drcom.conf file
	 */
	int read(std::string path ,drcom_config * conf);

	virtual ~config();
};

#endif /* SRC_CONFIG_H_ */
