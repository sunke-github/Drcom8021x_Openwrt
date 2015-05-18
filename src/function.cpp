/*
 * function.cpp
 *
 *  Created on: May 13, 2015
 *      Author: hennsun
 *      www.shareideas.net
 */

#include "drcomexception.h"
#include "def.h"
#include "function.h"
#include "config.h"
#include "eap_dealer.h"
#include "log.h"
#include <stdio.h>
#include <unistd.h>

function::function() {
	// TODO Auto-generated constructor stub
	alive_try = 0;
	config getconfig ;
     if(getconfig.read(conf_path,&conf)==0){
    	 readflag =true;
     }else{
    	 readflag =false;
     }
}

void function::online(){
	if(!readflag){
		return ;
		cout<<"read config file error, program stopped"<<endl;
	}
	std::vector<uint8_t> broadcast_mac = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	std::vector<uint8_t> nearest_mac = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x03 };

	ONLINE_STATE state = ONLINE_PROCESSING;
	bool flag = false;

	try{
		eap_dealer eap(conf.local.nic, conf.local.mac, conf.local.ip, conf.general.username, conf.general.password,conf);
		while (state != OFFLINE_PROCESSING) // auto redial
		{
			if (conf.remote.use_broadcast)
			{
				eap.logoff(nearest_mac);
				eap.logoff(nearest_mac);
				if (eap.start(broadcast_mac))
				if (eap.response_identity(broadcast_mac))
				if (eap.response_md5_challenge(broadcast_mac)) flag =true;
			}
			else
			{   eap.logoff(conf.remote.mac);
				eap.logoff(conf.remote.mac);
				if (eap.start(conf.remote.mac))
				if (eap.response_identity(conf.remote.mac))
				if (eap.response_md5_challenge(conf.remote.mac)) flag =true;
			}
			while (flag && state != OFFLINE_PROCESSING) // Keep Alive
			{
				//std::unique_lock<std::mutex> lock(mtx);
				if(eap.alive_identity(broadcast_mac)){
					state = ONLINE;
					alive_try =0;
				}else{
					alive_try ++;
				}
				if(alive_try>20){
					flag = false;
					alive_try = 0;
					SYS_LOG_ERR("Could not connect to server. Start to send connection requqest again!\n");
					break;
				}
				sleep(1);
				//cv.wait_for(lock, std::chrono::seconds(1));
			}
			if(!conf.general.auto_redial){
				break;             //如果设置不自动重播，则退出，否则自动重播。
			}
		} /*##############  while   ################*/

		//	std::unique_lock<std::mutex> lock(mtx);
		//	state = OFFLINE_NOTIFY;
		//	cv.notify_one();

	}catch(drcomexception& e ){
		SYS_LOG_ERR("Thread Online : " << e.what() << std::endl);
	}
}

void function::offline(){



}


function::~function() {
	// TODO Auto-generated destructor stub
}

