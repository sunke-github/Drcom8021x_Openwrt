/*
 * get_nic_addr.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef HEADER_GET_NIC_ADDR_H_
#define HEADER_GET_NIC_ADDR_H_
#include <vector>
#include <iostream>
using namespace std;

std::vector<uint8_t> get_mac_address(std::string nic);

std::string get_ip_address(std::string nic);




#endif /* HEADER_GET_NIC_ADDR_H_ */
