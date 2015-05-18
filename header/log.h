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

#ifndef __INCLUDE_LOG__
#define __INCLUDE_LOG__

#include <stddef.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <stdio.h>

#define DRCOM8021x_DEBUG

static inline std::string log_now()
{
    time_t now = time(NULL);

    struct tm* time = localtime(&now);
    
    char buf[128];
    sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d",
    					1900 + time->tm_year,
						1 + time->tm_mon,
						time->tm_mday,
						time->tm_hour,
						time->tm_min,
						time->tm_sec);
    
    std::string str(buf);
    return str;
}

#define LOG {                                                        \
    std::stringstream for_log_use_stream;                            \
    for_log_use_stream

#define PRINT_INFO                                                   \
    std::cout << log_now() << " " << for_log_use_stream.str();       /*\*/
  //  std::cout << for_log_use_stream.str();
#define PRINT_ERR                                                    \
    std::cout << log_now() << " " << for_log_use_stream.str();       /*\*/
  //  std::cerr << for_log_use_stream.str();
#ifdef DRCOM8021x_PRINT_DBG_ON_SCREEN
    #define PRINT_DBG                                                \
        std::clog << log_now() << " " << for_log_use_stream.str();   \
        std::cout << for_log_use_stream.str();
#else
    #define PRINT_DBG                                                \
        std::cout << log_now() << " " << for_log_use_stream.str();
#endif

#define LOG_INFO(section, info)                                      \
    LOG << "[" << section << " Info] " << info; PRINT_INFO }
#define LOG_ERR(section, err)                                        \
    LOG << "[" << section << " Error] " << err; PRINT_ERR }
#ifdef DRCOM8021x_DEBUG
    #define LOG_DBG(section, db)                                     \
        LOG << "[" << section << " Debug] " << db; PRINT_DBG }
#else
    #define LOG_DBG(db)
#endif

#define U31_LOG_INFO(info)       LOG_INFO("U31", info)
#define U31_LOG_ERR(err)         LOG_ERR("U31", err)
#define U31_LOG_DBG(db)          LOG_DBG("U31", db)

#define U62_LOG_INFO(info)       LOG_INFO("U62", info)
#define U62_LOG_ERR(err)         LOG_ERR("U62", err)
#define U62_LOG_DBG(db)          LOG_DBG("U62", db)

#define EAP_LOG_INFO(info)  LOG_INFO("EAP", info)
#define EAP_LOG_ERR(err)    LOG_ERR("EAP", err)
#define EAP_LOG_DBG(db)     LOG_DBG("EAP", db)

#define SYS_LOG_INFO(info)  LOG_INFO("Drcom8021x", info)
#define SYS_LOG_ERR(err)    LOG_ERR("Drcom8021x", err)
#define SYS_LOG_DBG(db)     LOG_DBG("Drcom8021x", db)

#endif // __INCLUDE_LOG__
