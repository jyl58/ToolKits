/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.3.24
*****************************************/
#include <stdio.h>
#include <string>
#include <cstring>
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

void playerStatusCB(const AiBox::player_status_show_t& status){
	LOGI(LOG_TAG.c_str(),"Player CB: status=%d,media_src=%d\n",status._status,status._media_src);
}
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
	if(!VoiceASR::getVoiceASRInstance()->initVoiceASR()){
		err_msg="Init Voice err !!!";
		return false;
	}
	//step9
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
	while(!AiBox::SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		LOGI(LOG_TAG.c_str(),"waiting network!,%d\n",sizeof(int));
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
	while(1){
		AUDIO_MEM_SHOW(LOG_TAG.c_str());
		ESP_LOGW(LOG_TAG.c_str(), "Stack: %d", uxTaskGetStackHighWaterMark(NULL));
		ESP_LOGW(LOG_TAG.c_str(),"external free heaps size=%d Bytes",heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
		vTaskDelay(5000 / portTICK_PERIOD_MS);
		HttpClientInterface::getHttpClientInstance()->recycleHttpRequestMem();
	}
}