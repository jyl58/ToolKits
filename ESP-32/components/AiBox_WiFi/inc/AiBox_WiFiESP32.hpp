/****************************************
*   esp wifi  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#pragma once
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_ping.h"
#include "ping/ping.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "AiBox_WiFiBase.hpp"
#define CONNECTED_BIT 1
#define NETWORK_HEALTH_CHECK_PERIOD 10*1000 //10s
namespace AiBox
{
	class ESPWiFiManage :public AiBox::WiFiBase
	{
		public:
			ESPWiFiManage():AiBox::WiFiBase(){_singleton_instance=this;}
			~ESPWiFiManage(){}
			bool init(const wifi_connect_config_t& connect_config)override;
			bool connect(const wifi_connect_config_t& connect_config)override;
			bool reconnect();
			//ping
			void pingByHostName(const std::string& host_name,esp_ping_found_fn ping_call_back);
			void pingConfig(const ip_addr_t *ipaddr,int ping_time,esp_ping_found_fn ping_call_back);
			static void pingCallback(ping_target_id_t found_id, esp_ping_found *found_val);
			static void dnsCallBack(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
			static void networkTimerCallBack( TimerHandle_t xTimer );
			static EventGroupHandle_t _wifi_event_group;
			static ESPWiFiManage* _singleton_instance;
		private:
			void registerWiFiCMD();
			bool setWiFiConfig(const wifi_connect_config_t& connect_config);
	};
};