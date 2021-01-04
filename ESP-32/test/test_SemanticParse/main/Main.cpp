/****************************************
*   Main function
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.14
*****************************************/
#include <stdio.h>
#include "LogInterface.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
#include "AiBox_ProcessASR.hpp"
#include "AiBox_SemanticParse.hpp"
#include "AiBox_PlayManage.hpp"
using namespace AiBox;
const std::string LOG_TAG="main";
FlashInterface _flash;
SystemStatusData _sys_data;  //need before system
SystemInterface _system;
BoardInterface _bord;
WiFiInterface	_wifi;
BleInterface	_ble;
HttpClientInterface _http;
PlayerInterface _player;
Console _console;
ASRProcess _asr_process;
void testHttpRequest(const std::string& response,int request_code)
{
	if(request_code==200){
		LOGI(LOG_TAG.c_str(),"### %s###",response.c_str());
	}else{
		LOGI(LOG_TAG.c_str(),"*** %s***",response.c_str());
	}
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
	//step2 check wifi is ok
	while(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		LOGI(LOG_TAG.c_str(),"waiting network!\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
	
	//main task
	directive_queue_t directive_data;
	while(1){
		if(DIRECTIVE_MAX_LEN== uxQueueSpacesAvailable(AiBox::SemanticParse::getSemanticParseInstance()->getSemanticDirectiveQueueHandle())){
			//print the mem size
			vTaskDelay(20000 / portTICK_PERIOD_MS);
			AUDIO_MEM_SHOW(LOG_TAG.c_str());
			ESP_LOGW(LOG_TAG.c_str(), "Stack: %d", uxTaskGetStackHighWaterMark(NULL));
			continue;
		}
		if(xQueueReceive(AiBox::SemanticParse::getSemanticParseInstance()->getSemanticDirectiveQueueHandle(),&directive_data,0)== pdPASS){
			LOGI(LOG_TAG.c_str(),"Receive derective type=%d",directive_data._directive_type);
			switch(directive_data._directive_type){
				case AiBox::Directive_Type_PLAYCONTROL:
				{
					msg_control_t control_data;
					MsgControlClass::getSemanticDataByptr(directive_data._directive_ptr,control_data);
					LOGI(LOG_TAG.c_str(),"Control Data action=%s,param=%d,session=%s",control_data.action.c_str(),control_data.param,control_data.session_id.c_str());
					break;
				}
				default :
					break;
			}
		}
	}
}