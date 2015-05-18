/*
 * eapdealer.h
 *
 *  Created on: May 14, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#ifndef SRC_EAPDEALER_H_
#define SRC_EAPDEALER_H_


#include "def.h"
#include "pcap_dealer.h"
#include <vector>
#include <iostream>


#define DRCOM_EAP_FRAME_SIZE    (0x60)
#define EAP_MD5_VALUE_SIZE      (0x10)
#define MAX_RETRY_TIME 2

#define EAP_SHOW_PACKET_TYPE(step)                                                                      \
    EAP_LOG_DBG("Recevied after " << step << ", "                                                       \
                 << "eapol_type = 0x" << std::hex << (int) eap_header->eapol_type                       \
                 << ", eap_id = 0x" << std::hex << (int) eap_header->eap_id                             \
                 << ", eap_type = 0x" << std::hex << (int) eap_header->eap_type                         \
                 << ", eap_length = " << (int) eap_header->eap_length << std::endl);


#define EAP_HANDLE_ERROR(step)  EAP_LOG_ERR(step << ": " << error << std::endl)


class eap_dealer {
public:
	eap_dealer(string nic,vector<uint8_t> local_mac,std::string local_ip,std::string identity,std::string key,drcom_config conf);

	struct ether_header get_eth_header(std::vector<uint8_t> gateway_mac, std::vector<uint8_t> local_mac);
	int start(std::vector<uint8_t> gateway_mac);
	int logoff(std::vector<uint8_t> gateway_mac);
	int response_identity(std::vector<uint8_t> gateway_mac);
	int alive_identity(std::vector<uint8_t> gateway_mac);
	int response_md5_challenge(std::vector<uint8_t> gateway_mac);


	virtual ~eap_dealer();



private:
	pcap_dealer pcap;
	//int rec;
	int resp_eap_id;
	int resp_md5_eap_id;// Recved from Request, MD5-Challenge EAP
	uint8_t response[128];//数据包
	int begintime;

	std::vector<uint8_t> local_mac;           // Const
	std::vector<uint8_t> resp_id, resp_md5_id, key;
	std::vector<uint8_t> alive_data;
	std::vector<uint8_t> resp_md5_attach_key;// Recved from Request, Identity
};

#endif /* SRC_EAPDEALER_H_ */
