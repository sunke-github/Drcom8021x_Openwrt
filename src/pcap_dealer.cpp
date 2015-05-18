/*
 * pcapdealer.cpp
 *
 *  Created on: May 14, 2015
 *      Author: hennsun
 *      wwww.shareideas.net
 */

#include <drcomexception.h>
#include "pcap_dealer.h"
#include "def.h"
#include <iostream>
#include <string.h>
#include <pcap.h>
using namespace std;

pcap_dealer::pcap_dealer(string nic,vector<uint8_t> mac,drcom_config conf) {
	// TODO Auto-generated constructor stub

	const int    SNAP_LEN = 1518;
	char   errbuf[PCAP_ERRBUF_SIZE] = {0};
	char   filter[100];
	struct bpf_program  fp;
	handle = pcap_open_live(nic.c_str(), SNAP_LEN, 1, conf.local.eap_timeout, errbuf);

	if (NULL == (handle)){
		throw drcomexception("pcap_open_live: " + std::string(errbuf));
	}

	if (pcap_datalink(handle) != DLT_EN10MB){
		throw drcomexception("pcap_datalink: not an Ethernet device.");   //##
	}
	sprintf(filter, "ether dst %02x:%02x:%02x:%02x:%02x:%02x and ether proto 0x888e",
										mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	if (pcap_compile(handle, &fp, filter, 0, 0) == -1){
		throw drcomexception(std::string("pcap_compile: ") + pcap_geterr(handle));   //##
	}
	if (pcap_setfilter(handle, &fp) == -1){
		throw drcomexception(std::string("pcap_setfilter: ") + pcap_geterr(handle));  //##
	}
	pcap_freecode(&fp);
}

int pcap_dealer::send(std::vector<uint8_t> data,std::vector<uint8_t> *success, std::string *error)
{
    try
    {
        if (pcap_sendpacket(handle, &data[0], (int) data.size()) != 0){
            throw drcomexception("pcap_sendpacket: " + std::string(pcap_geterr(handle)));  //##
        }
        struct pcap_pkthdr *header;
        const uint8_t *pkt_data;
        int ret = pcap_next_ex(handle, &header, &pkt_data);
        switch (ret)
        {
            case 0:{ // Timeout
                throw drcomexception("pcap_next_ex: timeout.");   //##
            	}
            	break;
            case 1: // Success
            	(*success).resize(header->len);
                memcpy(&(*success)[0], pkt_data, header->len);
                break;
            default:{
                throw drcomexception(std::string("pcap_next_ex: ") + pcap_geterr(handle));  //##
            }
        }
    }
    catch (drcomexception& e)   //###  drcomexcetion
    {
    	*error = e.what();
        return 0;
    }
    return 1;
}


int pcap_dealer::send_alive(std::vector<uint8_t> data, std::vector<uint8_t> *success, std::string *error )
{
  try
  {
	  struct pcap_pkthdr *header;
	  const uint8_t *pkt_data;
	  int ret = pcap_next_ex(handle, &header, &pkt_data);  //当有request 时才返回 1， 才发送成功。
	  switch (ret)
	  {
		  case 0:{ // Timeout
			  throw drcomexception("pcap_next_ex: timeout.");            //##
			  break;
		  }
		  case 1: // Success
			  (*success).resize(header->len);
			  memcpy(&(*success)[0], pkt_data, header->len);
			  data[19]=pkt_data[19];
			  if (pcap_sendpacket(handle, &data[0], (int) data.size()) != 0){
				  throw drcomexception("pcap_sendpacket: " + std::string(pcap_geterr(handle)));   //##
				  break;
			  }
			  break;
		  default:{
			 throw drcomexception(std::string("pcap_next_ex: ") + pcap_geterr(handle));   //##
		  }
	  }
  }
  catch (drcomexception& e)    //drcomexception
  {
	  *error = e.what();
	  return 0;
  }
  return 1;
}
pcap_dealer::~pcap_dealer() {
	// TODO Auto-generated destructor stub
}





