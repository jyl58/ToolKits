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
#include "AiBox_FlashInterface.hpp"
#include "AiBox_WiFiInterface.hpp"
#include "AiBox_SystemInterface.hpp"
const std::string LOG_TAG="main";
extern "C" void app_main(){
	AiBox::FlashInterface::getFlashInterfaceInstance()->init();
	AiBox::SystemInterface::getSystemInterfaceInstance()->init();
	AiBox::wifi_connect_config_t test_wifi_config;
	test_wifi_config._ssid="JYL";
	test_wifi_config._passwd="jyl586616cc";
	AiBox::WiFiInterface::getWiFiInterfaceInstance()->wiFiInit(test_wifi_config);
	while(1){
		LOGI(LOG_TAG.c_str(),"hello word\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}