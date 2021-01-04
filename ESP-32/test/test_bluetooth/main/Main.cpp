/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include <stdio.h>
#include "HttpClientInterface.hpp"
#include "LogInterface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "AiBox_FlashInterface.hpp"
#include "AiBox_WiFiInterface.hpp"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_Console.hpp"
#include "AiBox_BleInterface.hpp"
const std::string LOG_TAG="main";

extern "C" void app_main(){
	AiBox::FlashInterface::getFlashInterfaceInstance()->init();
	//AiBox::SystemInterface::getSystemInterfaceInstance()->init();
	AiBox::wifi_connect_config_t test_wifi_config;
	test_wifi_config._ssid="ASUS-AITEST_001";
	test_wifi_config._passwd="test1234";
	//AiBox::WiFiInterface::getWiFiInterfaceInstance()->wiFiInit(test_wifi_config);
	LOGI(LOG_TAG.c_str(),"run the console");
	AiBox::BleInterface::getBleInterfaceInstance()->initBle();
	//AiBox::Console::getConsoleInstance()->runConsole();

	while(1){
		//LOGI(LOG_TAG.c_str(),"hello word\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}