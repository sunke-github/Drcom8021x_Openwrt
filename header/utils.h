/*
 * utils.h
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef HEADER_UTILS_H_
#define HEADER_UTILS_H_

#include <iostream>
#include <stdint.h>
#include <vector>

using namespace std;

vector<uint8_t> get_md5_digest(std::vector<uint8_t>& data);
string hex_to_str(uint8_t *hex, size_t len, char separator);
void hexdump(std::vector<uint8_t> hex);
vector<std::string> split_string(string src, char delimiter = ' ', bool append_last = true);
vector<uint8_t> str_ip_to_vec(string ip);
vector<uint8_t> str_mac_to_vec(string mac);
vector<uint8_t> str_to_vec(string str);
vector<uint8_t> test(string str);

#endif /* HEADER_UTILS_H_ */
