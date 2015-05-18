/*
 * pcapdealer.h
 *
 *  Created on: May 14, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_PCAPDEALER_H_
#define SRC_PCAPDEALER_H_

#include <vector>
#include <pcap.h>
#include <iostream>
#include "def.h"
using namespace std;

class pcap_dealer {
public:
	pcap_dealer(string nic,vector<uint8_t> mac,drcom_config conf);
	int send(std::vector<uint8_t> data,std::vector<uint8_t> *success, std::string *error);
	int send_alive(std::vector<uint8_t> data, std::vector<uint8_t> *success, std::string *error );

	virtual ~pcap_dealer();

private:
	pcap_t *handle;

};



#endif /* SRC_PCAPDEALER_H_ */
