/****************************************
*   ESP system .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#pragma once
#include "esp_event.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "AiBox_SystemBase.hpp"
#include "esp_heap_caps.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#define MAX_AES_MALLOC_LEN 4*1024
void registerSystemCMD();
namespace AiBox
{
    class ESPSystemManage:public AiBox::SystemBase
    {
        public:
            ESPSystemManage():AiBox::SystemBase(){}
            ~ESPSystemManage(){}
            bool init()override;
            unsigned long long getCurrentTimeUs()override;
            int generateUuid(std::string&  uuid_out)override;
            int base64Encode(std::string& encode_str,const std::string& src)override;
            int base64Decode(std::string& decode_str,const std::string& src)override;
            int aesEncrypt(const std::string& in_str, const std::string& key, std::string& out_str)override;
            int aesDecrypt(const std::string& in_str, const std::string& key, std::string& out_str)override;
            int SNTPInit();
			void SNTPStop();
    };
};