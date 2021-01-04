/****************************************
*   wifi interface for app .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#pragma once
#include "AiBox_WiFiBase.hpp"
#include "AiBox_WiFiData.hpp"
namespace AiBox
{
	class WiFiBase;
	class WiFiInterface{
		public:
			WiFiInterface();
			~WiFiInterface();
			WiFiInterface(const WiFiInterface&)=delete;
			WiFiInterface& operator=(const WiFiInterface&)=delete;
			static WiFiInterface* getWiFiInterfaceInstance();
			bool wiFiInit();
			bool wiFiInit(const wifi_connect_config_t& connect_config);
			bool wiFiConnect(const wifi_connect_config_t& connect_config);
			void getWiFiStatus(wifi_status_t& wifi_status);
		private:
			WiFiBase* _wifi{nullptr};
			static WiFiInterface* _singleton_instance;
	};
};