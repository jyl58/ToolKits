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
	std::string src="dfedfdfddfdcdfe466879dgsgwe";
	std::string dsr;
	std::string dsr2;
	AiBox::SystemInterface::getSystemInterfaceInstance()->base64Encode(src,dsr);
	LOGI(LOG_TAG.c_str(),"Base64Encode: %s",dsr.c_str());
	AiBox::SystemInterface::getSystemInterfaceInstance()->base64Decode(dsr,dsr2);
	LOGI(LOG_TAG.c_str(),"Base64Decode: %s",dsr2.c_str());
	while(1){
		//LOGI(LOG_TAG.c_str(),"hello word\n");
		//print the mem size
		AUDIO_MEM_SHOW(LOG_TAG.c_str());
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}