/*
 * config.cpp
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *       www.shareideas.net
 *      代码参考EasyDrcom，进行了重构。
 */

#include <errno.h>
#include <boost/property_tree/ini_parser.hpp>
#include "drcomexception.h"

#include "config.h"
#include "log.h"
#include "utils.h"
#include "get_nic_addr.h"

#include "def.h"


using namespace std;

config::config() {
	// TODO Auto-generated constructor stub

}


/**/
int config::read(string path, drcom_config* conf){

	boost::property_tree::ptree pt;
	try {
		boost::property_tree::ini_parser::read_ini(path, pt);
		(*conf).general.mode = pt.get<int>("General.Mode");
		(*conf).general.username = pt.get<std::string>("General.UserName");
		(*conf).general.password = pt.get<std::string>("General.PassWord");
		(*conf).local.nic = pt.get<std::string>("Local.NIC");
	}
	catch (std::exception& e) {
		SYS_LOG_ERR("Failed to read '" << path << "' - " << e.what() << std::endl);
		return EBADF;
	}

	(*conf).general.auto_online = pt.get("General.AutoOnline", true);
	(*conf).general.auto_redial = pt.get("General.AutoRedial", true);

	(*conf).remote.ip = pt.get("Remote.IP", "172.25.8.4");
	(*conf).remote.port = pt.get("Remote.Port", 61440);
	(*conf).remote.use_broadcast = pt.get("Remote.UseBroadcast", true);

	if (!(*conf).remote.use_broadcast){
		(*conf).remote.mac = str_mac_to_vec(pt.get("Remote.MAC", "00:1a:a9:c3:3a:59"));

		//(*conf).remote.mac = test("test");
	}

	(*conf).local.hostname = pt.get("Local.HostName", "Drcom8021X for HITwh");
	//#define MAJOR_VERSION "v0.8"

	(*conf).local.kernel_version = pt.get("Local.KernelVersion", "v0.8");

	(*conf).local.eap_timeout = pt.get("Local.EAPTimeout", 1000);
	(*conf).local.udp_timeout = pt.get("Local.UDPTimeout", 2000);

	(*conf).fake.enable = pt.get("Fake.Enable", 0);



	SYS_LOG_DBG("General.UserName = " << (*conf).general.username << ", General.PassWord = " << (*conf).general.password << ", General.Mode = " << (*conf).general.mode << endl);
	SYS_LOG_DBG("General.AutoOnline = " << ((*conf).general.auto_online ? "True" : "False") << ", General.AutoRedial = " << ((*conf).general.auto_redial ? "True" : "False" ) << endl);
	SYS_LOG_DBG("Remote.IP:Port = " << (*conf).remote.ip << ":" << (*conf).remote.port << ", Remote.UseBroadcast = " << ((*conf).remote.use_broadcast ? "True" : "False" ) << endl);

	if (!(*conf).remote.use_broadcast)
		SYS_LOG_DBG("Remote.MAC = " << hex_to_str((uint8_t *)&(*conf).remote.mac[0], 6, ':') << endl);

	SYS_LOG_DBG("Local.NIC = " << (*conf).local.nic << ", Local.HostName = " << (*conf).local.hostname << ", Local.KernelVersion = " << (*conf).local.kernel_version << endl);

	SYS_LOG_DBG("Local.EAPTimeout = " << (*conf).local.eap_timeout << ", Local.UDPTimeout = " << (*conf).local.udp_timeout << endl);

	try {
		(*conf).local.ip = get_ip_address((*conf).local.nic);
		(*conf).local.mac = get_mac_address((*conf).local.nic);

		SYS_LOG_INFO("Fetch NIC IP & MAC successfully." << std::endl);
		SYS_LOG_INFO("Local.IP = " << (*conf).local.ip << ", Local.MAC = " << hex_to_str((uint8_t *)&(*conf).local.mac[0], 6, ':') << endl);
	}
	catch (drcomexception& e) {     //drcomexcetion
		SYS_LOG_ERR("Failed to fetch NIC info - " << e.what() << std::endl);
		return EBADF;
	}

	if ((*conf).fake.enable) // fake user
	{
		try {
			(*conf).fake.mac = str_mac_to_vec(pt.get<std::string>("Fake.MAC"));
			(*conf).fake.username = pt.get<std::string>("Fake.UserName");
			(*conf).fake.password = pt.get<std::string>("Fake.PassWord");
		}
		catch (std::exception& e) {
			SYS_LOG_ERR("Failed to read fake settings - " << e.what() << std::endl);
			return EBADF;
		}

		SYS_LOG_INFO("Fetch fake settings successfully." << std::endl);
		SYS_LOG_INFO("Fake.MAC = " << hex_to_str((uint8_t *)&(*conf).fake.mac[0], 6, ':') << ", Fake.UserName = " << (*conf).fake.username << ", Fake.PassWord = " << (*conf).fake.password << endl);

	}
	SYS_LOG_INFO("Loaded config successfully." << std::endl);

	return 0;
}



config::~config() {
	// TODO Auto-generated destructor stub
}

