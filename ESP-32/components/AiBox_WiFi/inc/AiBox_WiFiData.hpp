/****************************************
*    wifi data struct .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#pragma once
#include <string>
namespace AiBox
{
	typedef struct WiFiLinkConfig
	{
		char _ssid[40];
		char _passwd[30];
	}wifi_link_config_t;
	typedef struct WiFiFlashData
	{
		wifi_link_config_t _wifi_config[3];	
	}wifi_flash_data_t;

	typedef struct WiFiConnectConfig{
		std::string _ssid;
		std::string _passwd;
		int _timeout_ms;
	}wifi_connect_config_t;
	
	typedef struct WiFiStatus{
		wifi_connect_config_t _connect_config;
		bool _connect_status;
		std::string _ip;
		uint8_t _mac[6];
	}wifi_status_t;
};