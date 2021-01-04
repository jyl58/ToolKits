/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.14
*****************************************/
#include <stdio.h>
#include <string>
#include "AiBox_HttpClientInterface.hpp"
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
#include "AiBox_PlayerInterface.hpp"
#include "AiBox_PlayManage.hpp"
using namespace AiBox;
const std::string LOG_TAG="main";
static std::string err_msg=std::string();
bool systemSetup(){
	//step1 
	if(!FlashInterface::getFlashInterfaceInstance()->init()){
		err_msg="Flash init err";
		return false;
	}
	//step2
	if(!SystemInterface::getSystemInterfaceInstance()->init()){
		err_msg="System init err";
		return false;
	}
	//step3
	if(!BoardInterface::getBoardInterfaceInstance()->initBoard()){
		err_msg="Board init err";
		return false;
	}
	//step4
	if(!WiFiInterface::getWiFiInterfaceInstance()->wiFiInit()){
		err_msg="WiFi init err";
		return false;
	}
	//step5
	if(!BleInterface::getBleInterfaceInstance()->initBle()){
		err_msg="Bluetooth init err";
		return false;
	}
	//step6:
	if(!HttpClientInterface::getHttpClientInstance()->init()){
		err_msg="HttpClient init err";
		return false;
	}
	//step7:
	if(!PlayerInterface::getPlayerInterfaceInstance()->initPlayer()){
		err_msg="Player init err";
		return false;
	}
	//step8:
	if(!PlayManage::getPlayManageInstance()->initPlayManage()){
		err_msg="Player manage init err";
		return false;
	}
	//step7
	Console::getConsoleInstance()->runConsole();
	return true;
}
extern "C" void app_main(){
	//step1: system setup
	if(!systemSetup()){
		while(1){
			LOGI(LOG_TAG.c_str(),"system setup err: %s\n",err_msg.c_str());
			vTaskDelay(1000 / portTICK_PERIOD_MS);	
		}
	}
	//step2: service system setup

	//step3: voice system setup
	if(!VoiceASR::getVoiceASRInstance()->initVoiceASR()){
		while(1){
			LOGI(LOG_TAG.c_str(),"Voice setup err\n");
			vTaskDelay(1000 / portTICK_PERIOD_MS);	
		}
	}
	//main task
	while(1){
		vTaskDelay(20000 / portTICK_PERIOD_MS);
        AUDIO_MEM_SHOW(LOG_TAG.c_str());
        ESP_LOGW(LOG_TAG.c_str(), "Stack: %d", uxTaskGetStackHighWaterMark(NULL));
	}
}