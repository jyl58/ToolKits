/****************************************
*   wifi interface for app .app
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#include <cstring>
#include "AiBox_WiFiInterface.hpp"
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
	#include "AiBox_WiFiESP32.hpp"
#endif
#include "AiBox_SystemStatusData.hpp"
using namespace AiBox;

const std::string LOG_TAG="WiFiInterface";

WiFiInterface* WiFiInterface::_singleton_instance=nullptr;
WiFiInterface::~WiFiInterface()
{
	if(_wifi!=nullptr){
		delete _wifi;
		_wifi=nullptr;
	}
}
WiFiInterface* 
WiFiInterface::getWiFiInterfaceInstance()
{
	if(_singleton_instance==nullptr){
		_singleton_instance=new WiFiInterface();
	}
	return _singleton_instance;
}
WiFiInterface::WiFiInterface()
{
	_singleton_instance=this;
}
bool 
WiFiInterface::wiFiInit()
{
	wifi_connect_config_t wifi_connect_data;
	memset(&wifi_connect_data,0,sizeof(wifi_connect_config_t));
	wifi_connect_data._ssid=std::string();
	wifi_connect_data._passwd=std::string();
	wifi_connect_data._timeout_ms=5000; //5s
	SystemStatusData::getSystemStatusDataInstance()->getWifiConfig(wifi_connect_data);
	return wiFiInit(wifi_connect_data);
}
bool 
WiFiInterface::wiFiInit(const wifi_connect_config_t& connect_config)
{
	/*net the wifi instance*/
#ifdef ESP32_PLATFORM
	_wifi=new ESPWiFiManage();
#endif
	if(_wifi==nullptr){
		LOGI(LOG_TAG.c_str(),"Creat WIFI manage err!");
		return false;
	}
	if(!_wifi->init(connect_config)){
		LOGI(LOG_TAG.c_str(),"Init WIFI manage err!");
		return false;	
	}
	return true;
}
bool 
WiFiInterface::wiFiConnect(const wifi_connect_config_t& connect_config)
{
	if(_wifi!=nullptr){
		return _wifi->connect(connect_config);
	}
	return false;
}
void 
WiFiInterface::getWiFiStatus(wifi_status_t& wifi_status)
{
	wifi_status=_wifi->_wifi_status;
}
