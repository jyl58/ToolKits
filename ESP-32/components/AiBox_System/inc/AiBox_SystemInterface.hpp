/****************************************
*   system interface for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#pragma once
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "AiBox_SystemBase.hpp"
#include "AiBox_SystemStatusData.hpp"
#define WIFI_STATUS_LENGTH 1
#define BLUE_STATUS_LENGTH 1
#define PLAYER_STATUS_LENGTH 1
#define SYS_QUEUE_SET_LENGTH WIFI_STATUS_LENGTH + BLUE_STATUS_LENGTH + PLAYER_STATUS_LENGTH
namespace AiBox
{   class SystemStatusData;
    class SystemInterface
    {
        public:
            SystemInterface();
            SystemInterface(const SystemInterface&)=delete;
            SystemInterface& operator=(const SystemInterface&)=delete;
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            ~SystemInterface();
            static SystemInterface* getSystemInterfaceInstance();
            bool init();
            QueueHandle_t getSystemWiFiStatusHandle(){return _wifi_status_queue_handle;}
            QueueHandle_t getSystemBlueStatusHandle(){return _blue_status_queue_handle;}
            QueueHandle_t getSystemPlayerStatusHandle(){return _player_status_queue_handle;}
            unsigned long long getCurrentTimeUs();
            unsigned long long getCurrentTimeMs();
            int base64Encode(const std::string& src,std::string& encode_str);
            int base64Decode(const std::string& src,std::string& decode_str);
            int encodeURL(const std::string& str, std::string&  after_encode);
            int aesEncrypt(const std::string& in_str, const std::string& key, std::string& out_str);
            int aesDecrypt(const std::string& in_str, const std::string& key, std::string& out_str);
            int generateUuid(std::string&  uuid_out){return _system->generateUuid(uuid_out);}
            void* memMalloc(size_t size);
            void memFree(void *ptr);
        private:
            bool initSystemStatusHandle();
            static void systemStatusMaintainTask(void* param);
            void readWiFiConfig();
            void storeWiFiConfig(const wifi_link_config_t& wifi_connect_config);
            void exec();
            SystemBase* _system;
            SystemStatusData *_system_data;
            static SystemInterface*  _singleton_instance;
            QueueSetHandle_t _sys_status_queue_set_handle=nullptr;
            QueueHandle_t _wifi_status_queue_handle=nullptr;
            QueueHandle_t _blue_status_queue_handle=nullptr;
            QueueHandle_t _player_status_queue_handle=nullptr;
    };
};