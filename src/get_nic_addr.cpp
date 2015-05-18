/**
 * Copyright (C) 2014 Shindo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <drcomexception.h>
#include <ifaddrs.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <string.h>


using namespace std;

std::vector<uint8_t> get_mac_address(std::string nic)
{
    int sock;
    struct ifreq dev;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        throw drcomexception("get_mac_address: socket failed");//  errno;  //##
    }
    
    strncpy(dev.ifr_name, nic.c_str(), sizeof(dev.ifr_name));
    dev.ifr_name[sizeof(dev.ifr_name)-1] = '\0';
    
    if (ioctl(sock, SIOCGIFHWADDR, &dev) < 0){
        throw drcomexception("get_mac_address: ioctl failed");//, errno);  //##
    }
    
    std::vector<uint8_t> ret(6, 0);
    memcpy(&ret[0], dev.ifr_hwaddr.sa_data, 6);
    return ret;
}


std::string get_ip_address(std::string nic)
{
    struct ifaddrs *ifaddr = NULL;
    std::string ip;
    
    if (getifaddrs(&ifaddr) < 0){
        throw drcomexception("get_ip_address: getifaddrs failed");//, errno);   //##
    }
    bool found = false;
    struct ifaddrs * ifa;
    for( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!strcmp(ifa->ifa_name, nic.c_str()))
            if (ifa->ifa_addr->sa_family == AF_INET) // only deal with IPv4
            {
                ip = inet_ntoa(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr);
                found = true; break;
            }
    }
    
    if (!found){
        throw drcomexception("get_ip_address: NIC '" + nic + "' not found.");    //##
    }
    return ip;
}

