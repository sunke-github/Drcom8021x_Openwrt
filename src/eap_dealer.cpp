/*
 * eapdealer.cpp
 *
 *  Created on: May 14, 2015
 *      Author: hennsun
 *      www.shareideas.net
 *      代码参考EasyDrcom，进行了重构。
 */


#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "log.h"
#include "eap_dealer.h"
#include "pcap_dealer.h"
#include "utils.h"
#include "def.h"


using namespace std;



// the local_ip can be used to detect 'IP conflict!'
//method():value(x)  ; give x to value
eap_dealer::eap_dealer(string nic,vector<uint8_t> local_mac,std::string local_ip,std::string identity,std::string key,drcom_config conf)
: pcap(nic,local_mac,conf),resp_eap_id(0),resp_md5_eap_id(0),local_mac(local_mac),key(str_to_vec(key))
{

	begintime = time(0);
	std::vector<uint8_t> ip = str_ip_to_vec(local_ip);

	resp_id = str_to_vec(identity);

	resp_id.insert(resp_id.end(),0x00);   //0x00, 0x44, 0x61, 0x00, 0x00
	resp_id.insert(resp_id.end(),0x04);
	resp_id.insert(resp_id.end(),0x61);
	resp_id.insert(resp_id.end(),0x00);
	resp_id.insert(resp_id.end(),0x00);


	resp_id.insert(resp_id.end(), ip.begin(), ip.end());

	resp_md5_id = str_to_vec(identity);

	resp_md5_id.insert(resp_md5_id.end(),0x00);    //0x00 ,0x44, 0x61, 0x0a, 0x00 );
	resp_md5_id.insert(resp_md5_id.end(),0x44);
	resp_md5_id.insert(resp_md5_id.end(),0x61);
	resp_md5_id.insert(resp_md5_id.end(),0x0a);
	resp_md5_id.insert(resp_md5_id.end(),0x00);

	resp_md5_id.insert(resp_md5_id.end(), ip.begin(), ip.end());

}


struct ether_header eap_dealer::get_eth_header(std::vector<uint8_t> gateway_mac, std::vector<uint8_t> local_mac){
	struct ether_header eth_header;

	memcpy(eth_header.ether_dhost, &gateway_mac[0], 6);
	memcpy(eth_header.ether_shost, &local_mac[0], 6);
	eth_header.ether_type = htons(0x888e); // 802.1X Authentication (0x888e)

	return eth_header;

}


int eap_dealer::start(std::vector<uint8_t> gateway_mac){
   EAP_LOG_INFO("Start." << std::endl);
   std::vector<uint8_t> pkt_data(DRCOM_EAP_FRAME_SIZE, 0);
   uint8_t eapol_start[] = {
	   0x01,             // Version: 802.1X-2001
	   0x01,             // Type: Start
	   0x00, 0x00        // Length: 0
   };
   struct ether_header eth_header = get_eth_header(gateway_mac, local_mac);
   memcpy(&pkt_data[0], &eth_header, sizeof(eth_header));
   memcpy(&pkt_data[sizeof(eth_header)], eapol_start, 4);

   	std::vector<uint8_t> success;
   	std::string error;
    // EAP_AUTO_RETRY("Start");
	int retry_times = 0, ret;
	while ((ret = pcap.send(pkt_data, &success, &error)) == 0 && retry_times < MAX_RETRY_TIME)  //这里有问题。
	{
		retry_times++;
		EAP_LOG_ERR("Failed to perform " << "Start" << ", retry times = " << retry_times << std::endl);
		EAP_LOG_INFO("Try to perform " << "Start" << " after 2 seconds." << std::endl);
		sleep(2);
	}
	if (retry_times == MAX_RETRY_TIME)
	{
		EAP_LOG_ERR("Failed to perfrom " << "Start" << ", stopped." << std::endl);
		return 0;
	}
	EAP_HANDLE_ERROR("Start");
	if(ret){
		//struct ether_header *eth_header;
		struct eap_header *eap_header;

		//eth_header = (struct ether_header*) &success[0];
		eap_header = (struct eap_header*) (&success[0] + sizeof(struct ether_header));

		EAP_SHOW_PACKET_TYPE("Start");

		if (eap_header->eapol_type != 0x00) // EAP Packet
			return 0;
		// EAP Request                  // EAP Failure
		if (eap_header->eap_code != 0x01) //&& eap_header->eap_code != 0x04
			return 0;
		// Now, only eap_code = 0x01 packets, select eap_type = 0x01 packet
		if (eap_header->eap_type != 0x01) // Request, Identity
			return 0;
		EAP_LOG_INFO("Gateway returns: Request, Identity" << std::endl);
		resp_eap_id = eap_header->eap_id;
		return 1;
	}
	return ret;
}

int eap_dealer::response_identity(std::vector<uint8_t> gateway_mac){

	EAP_LOG_INFO("Response, Identity." << std::endl);
	std::vector<uint8_t> pkt_data(DRCOM_EAP_FRAME_SIZE, 0);
	std::vector<uint8_t> eap_resp_id = {
		0x01,           // Version: 802.1X-2001
		0x00,           // Type: EAP Packet
		0x00, 0x00,     // EAP Length
		0x02,           // Code: Reponse
		(uint8_t) resp_eap_id,    // Id
		0x00, 0x00,     // EAP Length
		0x01            // Type: Identity
	};   //-std=c++11

	uint16_t eap_length = htons(5 + resp_id.size());
	memcpy(&eap_resp_id[2], &eap_length, 2);
	memcpy(&eap_resp_id[6], &eap_length, 2);

	struct ether_header eth_header = get_eth_header(gateway_mac, local_mac);

	memcpy(&pkt_data[0], &eth_header, sizeof(eth_header));
	memcpy(&pkt_data[sizeof(eth_header)], &eap_resp_id[0], eap_resp_id.size());
	memcpy(&pkt_data[sizeof(eth_header) + eap_resp_id.size()], &resp_id[0], resp_id.size());
	alive_data=pkt_data;
	for(int j=0;j<96;j++){
		response[j]=alive_data[j];
	}

	/*##################################################################*/
   // EAP_AUTO_RETRY("Response, Identity");
	std::vector<uint8_t> success;
	std::string error;

	int retry_times = 0, ret;
	while ((ret = pcap.send(pkt_data, &success, &error)) == 0 && retry_times < MAX_RETRY_TIME)  //这里有问题。
	{
		retry_times++;
		EAP_LOG_ERR("Failed to perform " << "Response, Identity" << ", retry times = " << retry_times << std::endl);
		EAP_LOG_INFO("Try to perform " << "Response, Identity" << " after 2 seconds." << std::endl);
		sleep(2);
	}
	if (retry_times == MAX_RETRY_TIME)
	{
		EAP_LOG_ERR("Failed to perfrom " << "Response, Identity" << ", stopped." << std::endl);
		return 0;
	}
	EAP_HANDLE_ERROR("Response, Identity");
	if(ret){
		//struct ether_header *eth_header;
		struct eap_header *eap_header;

		//eth_header = (struct ether_header*) &success[0];
		eap_header = (struct eap_header*) (&success[0] + sizeof(struct ether_header));
		EAP_SHOW_PACKET_TYPE("Response, Identity");

		if (eap_header->eapol_type != 0x00) // EAP Packet
			return 0;
									// EAP Request                  // EAP Failure
		if (eap_header->eap_code != 0x01) //&& eap_header->eap_code != 0x04
			return 0;
		EAP_LOG_INFO("Gateway returns: Request, MD5-Challenge EAP" << std::endl);
		resp_md5_eap_id = eap_header->eap_id;
		resp_md5_attach_key = std::vector<uint8_t>(eap_header->eap_md5_value, eap_header->eap_md5_value + EAP_MD5_VALUE_SIZE);
	}
	resp_eap_id++;
	return ret;
}


int eap_dealer::response_md5_challenge(std::vector<uint8_t> gateway_mac){

	EAP_LOG_INFO("Response, MD5-Challenge EAP." << std::endl);
	std::vector<uint8_t> pkt_data(DRCOM_EAP_FRAME_SIZE, 0);

	std::vector<uint8_t> eap_resp_md5_ch = {
		0x01,               // Version: 802.1X-2001
		0x00,               // Type: EAP Packet
		0x00, 0x00,         // EAP Length
		0x02,               // Code: Reponse
		(uint8_t) resp_md5_eap_id,    // Id
		0x00, 0x00,         // EAP Length
		0x04,               // Type: MD5-Challenge EAP
		EAP_MD5_VALUE_SIZE  // EAP-MD5 Value-Size = 16
	};     //-std=c++11

	uint16_t eap_length = htons(6 + EAP_MD5_VALUE_SIZE + resp_md5_id.size());

	memcpy(&eap_resp_md5_ch[2], &eap_length, 2);
	memcpy(&eap_resp_md5_ch[6], &eap_length, 2);

	struct ether_header eth_header = get_eth_header(gateway_mac, local_mac);

	memcpy(&pkt_data[0], &eth_header, sizeof(eth_header));
	memcpy(&pkt_data[sizeof(eth_header)], &eap_resp_md5_ch[0], eap_resp_md5_ch.size());

	std::vector<uint8_t> eap_key(1 +                                // EAP Id
								 key.size() + EAP_MD5_VALUE_SIZE);
	eap_key[0] = resp_md5_eap_id;
	memcpy(&eap_key[1], &key[0], key.size());
	memcpy(&eap_key[1 + key.size()], &resp_md5_attach_key[0], EAP_MD5_VALUE_SIZE);

	std::vector<uint8_t> md5_value = get_md5_digest(eap_key);
	memcpy(&pkt_data[sizeof(eth_header) + eap_resp_md5_ch.size()], &md5_value[0], md5_value.size());

	memcpy(&pkt_data[sizeof(eth_header) + eap_resp_md5_ch.size() + EAP_MD5_VALUE_SIZE], &resp_md5_id[0], resp_md5_id.size());

    //EAP_AUTO_RETRY("Response, MD5-Challenge EAP");
	std::vector<uint8_t> success;
	std::string error;
	int retry_times = 0, ret;
	while ((ret = pcap.send(pkt_data, &success, &error)) == 0 && retry_times < MAX_RETRY_TIME)
	{
		retry_times++;
		EAP_LOG_ERR("Failed to perform " << "Response, MD5-Challenge EAP" << ", retry times = " << retry_times << std::endl);
		EAP_LOG_INFO("Try to perform " << "Response, MD5-Challenge EAP" << " after 2 seconds." << std::endl);
		//std::this_thread::sleep_for(std::chrono::seconds(2));
		sleep(2);
	}
	if (retry_times == MAX_RETRY_TIME)
	{
		EAP_LOG_ERR("Failed to perfrom " << "Response, MD5-Challenge EAP" << ", stopped." << std::endl);
		return 0;
	}
	EAP_HANDLE_ERROR("Response, MD5-Challenge EAP");
	if(ret){
		//struct ether_header *eth_header;
		struct eap_header *eap_header;

		//eth_header = (struct ether_header*) &success[0];
		eap_header = (struct eap_header*) (&success[0] + sizeof(struct ether_header));
		EAP_SHOW_PACKET_TYPE("Response, MD5-Challenge EAP");
		if (eap_header->eapol_type != 0x00) // EAP Packet
			return 0;
									// Request                      // Success
		if (eap_header->eap_code != 0x01 && eap_header->eap_code != 0x03)
			return 0;
		if (eap_header->eap_code == 0x01) // Request
		{
			if (eap_header->eap_type != 0x02) // Notification
				return 0;

			std::string noti(ntohs(eap_header->eap_length) - 5, 0); // 1 for NULL Terminator
			memcpy(&noti[0], ((uint8_t*)eap_header + 4 + 5), // 4 - EAPol Header, 5 - EAP Header
				   ntohs(eap_header->eap_length) - 5);

			EAP_LOG_INFO("Gateway returns: Request, Notification: " << noti << std::endl);

			if (!noti.compare("userid error1"))
				EAP_LOG_INFO("Tips: Account or password authentication fails, the system does not exist in this account." << std::endl);

			if (!noti.compare("userid error3"))
				EAP_LOG_INFO("Tips: Account or password authentication fails, the system does not exist in this account or your account has arrears down." << std::endl);
			logoff(gateway_mac); // Need to send a logoff, or the gateway will always send notification
			return 1; // Don't retry when notification
		}

		// In fact, this condition is always true
		if (eap_header->eap_code == 0x03) // Success
			EAP_LOG_INFO("Gateway returns: Success" << std::endl);
		return 1;
	}
	return ret;
}

int eap_dealer::alive_identity(std::vector<uint8_t> gateway_mac){

	resp_eap_id++;
	std::vector<uint8_t> success;
	std::string error;
	int ret = pcap.send_alive(alive_data, &success,&error);   //change here
	//just for debugging.
	//EAP_HANDLE_ERROR("Active, Identity. Could ignore this error!");

	if(ret){
		char ctime[20];
		sprintf(ctime, "%d", (int)(time(0)-begintime));
		SYS_LOG_INFO("online time is "+std::string(ctime)+"s\n");
	}
	return ret;
}

int eap_dealer::logoff(std::vector<uint8_t> gateway_mac){

//	EAP_LOG_INFO("Logoff." << std::endl);
//
//	std::vector<uint8_t> pkt_data(DRCOM_EAP_FRAME_SIZE, 0);
//
//	uint8_t eapol_logoff[] = {
//		0x01,             // Version: 802.1X-2001
//		0x02,             // Type: Logoff
//		0x00, 0x00        // Length: 0
//	};
//
//	struct ether_header eth_header = get_eth_header(gateway_mac, local_mac);
//	memcpy(&pkt_data[0], &eth_header, sizeof(eth_header));
//	memcpy(&pkt_data[sizeof(eth_header)], eapol_logoff, 4);
//
//	auto handler_success = [&](std::vector<uint8_t> recv) -> int {
//		struct ether_header *eth_header;
//		struct eap_header *eap_header;
//
//		eth_header = (struct ether_header*) &recv[0];
//		eap_header = (struct eap_header*) (&recv[0] + sizeof(struct ether_header));
//
//		EAP_SHOW_PACKET_TYPE("Logoff");
//
//		// We needn't to deal with the packet back
//		return 0;
//	};
//
//	auto handler_error = [&](std::string) {
//		// We needn't to deal with the packet back
//	};
//
//	try
//	{
//		pcap.send(pkt_data, handler_success, handler_error); //###############################
//	}
//	catch (std::exception &e)
//	{
//		EAP_LOG_ERR("Logoff :" << e.what() << std::endl);
//	}

	return 1;
}

eap_dealer::~eap_dealer() {
	// TODO Auto-generated destructor stub
}

