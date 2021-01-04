/****************************************
*   wifi base for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#pragma once
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "AiBox_WiFiData.hpp"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_WiFiInterface.hpp"
#define MAX_CONNECT_TRY_TIMES 5
extern "C" int wiFiConfigCMD(int argc, char **argv);
namespace AiBox
{
	enum ServerType{
		ServerType_NONE=10,
		ServerType_IP,
		ServerType_HOSTNAME
	};
	typedef struct PingServer{
		enum ServerType _type;
		std::string _host_name;
		uint32_t    _ip;
	}ping_server_t;
	class WiFiInterface;
	class WiFiBase
	{
		public:
			friend WiFiInterface;
			WiFiBase(){}
			virtual bool init(const wifi_connect_config_t& connect_config)=0;
			virtual bool connect(const wifi_connect_config_t& connect_config)=0;
			virtual ~WiFiBase(){}
			void* operator new(std::size_t size);
            void  operator delete(void *ptr);
			wifi_status_t _wifi_status;
			const ping_server_t _ping_server_list[5]=
			{
				{ServerType_IP,std::string(),IPV4(114,114,114,114)},
				{ServerType_IP,std::string(),IPV4(119,29,29,29)},
				{ServerType_IP,std::string(),IPV4(1,2,4,8)},
				{ServerType_IP,std::string(),IPV4(180,76,76,76)},
				{ServerType_HOSTNAME,"touchdetect.hismarttv.com",0}
			};
			int _ping_server_index=1;
			//used to log show 
			std::string _ping_ip=std::string();
			//used to check network healthy 
			TimerHandle_t _network_xTimers=nullptr;
			uint8_t _ping_times=1;
			bool _ping_success=false;
			/*ping fail count*/
			uint8_t _ping_fail_count=0;
			/*connect fail count*/
			uint8_t _ap_connect_fail_count=0;
			uint8_t _ap_wifi_config_index=0;
		protected:
			bool _wifi_driver_initialized{false};
	};
};