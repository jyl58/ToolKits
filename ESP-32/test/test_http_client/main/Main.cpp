/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include <stdio.h>
#include "LogInterface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "audio_mem.h"
#include "AiBox_FlashInterface.hpp"
#include "AiBox_WiFiInterface.hpp"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_Console.hpp"
#include "AiBox_BleInterface.hpp"
#include "AiBox_BoardInterface.hpp"
#include "AiBox_VoiceASR.hpp"
#include "AiBox_HttpClientInterface.hpp"
#include "AiBox_SystemStatusData.hpp"
const std::string LOG_TAG="main";
void testHttpRequest(const std::string& response,int request_code)
{
	if(request_code==200){
		LOGI(LOG_TAG.c_str(),"### %s###",response.c_str());
	}else{
		LOGI(LOG_TAG.c_str(),"*** %s***",response.c_str());
	}
}
extern "C" void app_main(){
	AiBox::FlashInterface::getFlashInterfaceInstance()->init();
	AiBox::SystemInterface::getSystemInterfaceInstance()->init();
	AiBox::BoardInterface::getBoardInterfaceInstance()->initBoard();
	AiBox::wifi_connect_config_t test_wifi_config;
	test_wifi_config._ssid="JYL";
	test_wifi_config._passwd="jyl586616cc";
	AiBox::WiFiInterface::getWiFiInterfaceInstance()->wiFiInit(test_wifi_config);
	LOGI(LOG_TAG.c_str(),"run the console");
	AiBox::BleInterface::getBleInterfaceInstance()->initBle();
	//AiBox::VoiceASR::getVoiceASRInstance()->initVoiceASR();
	AiBox::HttpClientInterface::getHttpClientInstance()->init();
	AiBox::Console::getConsoleInstance()->runConsole();
	while(!AiBox::SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		LOGI(LOG_TAG.c_str(),"waiting network!\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	AiBox::http_request_data_t test_http_data;
	test_http_data._url="http://www.baidu.com";
	test_http_data._post_data=std::string();
	test_http_data._response_func=testHttpRequest;
	test_http_data._header="key1=111;key2=222;key3=333;key4=444;key5=555;key6=666;";
	test_http_data._method=AiBox::RequestMethod_GET;
	AiBox::HttpClientInterface::getHttpClientInstance()->httpRequest(test_http_data);
	while(1){
		//LOGI(LOG_TAG.c_str(),"hello word\n");
		//print the mem size
		AUDIO_MEM_SHOW(LOG_TAG.c_str());
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}