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
#include "AiBox_Console.hpp"
#include "AiBox_BleInterface.hpp"
#include "AiBox_BoardInterface.hpp"
const std::string LOG_TAG="main";

extern "C" void app_main(){
	AiBox::FlashInterface::getFlashInterfaceInstance()->init();
	AiBox::SystemInterface::getSystemInterfaceInstance()->init();
	AiBox::BoardInterface::getBoardInterfaceInstance()->initBoard();
	AiBox::wifi_connect_config_t test_wifi_config;
	test_wifi_config._ssid="ASUS-AITEST_001";
	test_wifi_config._passwd="test1234";
	AiBox::WiFiInterface::getWiFiInterfaceInstance()->wiFiInit(test_wifi_config);
	LOGI(LOG_TAG.c_str(),"run the console");
	AiBox::BleInterface::getBleInterfaceInstance()->initBle();
	AiBox::Console::getConsoleInstance()->runConsole();
	//test button
	AiBox::button_data_t test_button;
	QueueHandle_t test_button_queue_handle=AiBox::BoardInterface::getBoardInterfaceInstance()->getButtonQueueHandle();
	while(1){
		if(test_button_queue_handle==nullptr){
			LOGI(LOG_TAG.c_str(),"the queue handle is nullptr");
			continue;
		}
		if(xQueueReceive(test_button_queue_handle,&test_button,200 / portTICK_PERIOD_MS)==pdTRUE){
			switch(test_button._button_id){
				case AiBox::BUTTON_ID_VOLDOWN:
					LOGI(LOG_TAG.c_str(),"VOLDOWN type: %d",test_button._type);
				break;
				case AiBox::BUTTON_ID_VOLUP:
					LOGI(LOG_TAG.c_str(),"UP type: %d",test_button._type);
				break;
				case AiBox::BUTTON_ID_PLAY:
					LOGI(LOG_TAG.c_str(),"PLAY type: %d",test_button._type);
				break;
				case AiBox::BUTTON_ID_MUTE:
					LOGI(LOG_TAG.c_str(),"MUTE type: %d",test_button._type);
				break;
				default:
				break;
			}
		}
	}
}