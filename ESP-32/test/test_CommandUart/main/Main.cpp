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
#include "AiBox_UART.h"
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
	//step9:
	if(!SystemUart::getSystemUartInstance()->init()){
		err_msg="Command uart init err";
		return false;
	}
	//step10
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
	AiBox_uart_event_t uart_event;
	while(1){
		if(COMMAND_UART_QUEUE_SIZE== uxQueueSpacesAvailable(SystemUart::getSystemUartInstance()->getUartEventHandle())){
			//print the mem size
			vTaskDelay(20000 / portTICK_PERIOD_MS);
			AUDIO_MEM_SHOW(LOG_TAG.c_str());
			LOGI(LOG_TAG.c_str(), "Stack: %d", uxTaskGetStackHighWaterMark(NULL));
			LOGI(LOG_TAG.c_str(), "free spiram heap: %d",heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
			continue;
		}
		if(xQueueReceive(SystemUart::getSystemUartInstance()->getUartEventHandle(),&uart_event,0)== pdPASS){
			switch(uart_event.type){
				case UART_DATA:
				{
					std::string content;
					SystemUart::getSystemUartInstance()->read(content,uart_event.size);
					LOGI(LOG_TAG.c_str(), "get uart data:%s",content.c_str());
				}
					break;
				case UART_BREAK:             
					break;
				case UART_BUFFER_FULL:       
					break;
				case UART_FIFO_OVF:       
					break;
				case UART_FRAME_ERR:        
					break;
				case UART_PARITY_ERR:        
					break;
				case UART_DATA_BREAK:        
					break;
				case UART_PATTERN_DET:       
					break;
				case UART_EVENT_MAX:
					break;
				default:
					break;
			}
		}//end of if()
	}
}