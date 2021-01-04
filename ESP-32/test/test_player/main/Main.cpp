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
#include "freertos/ringbuf.h"
using namespace AiBox;
const std::string LOG_TAG="main";
extern char adf_music_mp3_start[]    asm("_binary_adf_music_mp3_start");
extern char adf_music_mp3_end[]      asm("_binary_adf_music_mp3_end");
extern char linked_mp3_start[]       asm("_binary_linked_mp3_start");
extern char linked_mp3_end[]         asm("_binary_linked_mp3_end");
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
	//step9
	Console::getConsoleInstance()->runConsole();
	return true;
}
int send_count=0;
int last_len=adf_music_mp3_end-adf_music_mp3_start;
char* start_pos=adf_music_mp3_start;
int segment_len=1024*50;

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
	AiBox::button_data_t  _button_data;
	int vol=0;
	while(1){
		if(BUTTON_MAX_QUEUE_LEN== uxQueueSpacesAvailable(AiBox::BoardInterface::getBoardInterfaceInstance()->getButtonQueueHandle())){
			//print the mem size
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			AUDIO_MEM_SHOW(LOG_TAG.c_str());
			ESP_LOGW(LOG_TAG.c_str(), "Stack: %d", uxTaskGetStackHighWaterMark(NULL));
			continue;
		}
		
		if(xQueueReceive(AiBox::BoardInterface::getBoardInterfaceInstance()->getButtonQueueHandle(),&_button_data, 0)== pdPASS){
			LOGI(LOG_TAG.c_str(),"get button event ");
			switch(_button_data._button_id){
				case AiBox::BUTTON_ID_VOLUP:{
					if(_button_data._type==AiBox::BUTTON_TYPE_UP){
						AiBox::PlayerInterface::getPlayerInterfaceInstance()->getVol(vol);
						vol+=5;
						if(vol>100){
							vol=100;
						}
						AiBox::PlayerInterface::getPlayerInterfaceInstance()->setVol(vol);
						LOGI(LOG_TAG.c_str(),"volup=%d\n",vol);
					}
					break;
				}
				case AiBox::BUTTON_ID_VOLDOWN:{
					send_count=0;
					last_len=adf_music_mp3_end-adf_music_mp3_start;
					start_pos=adf_music_mp3_start;
					if(_button_data._type==AiBox::BUTTON_TYPE_UP){
						AiBox::PlayerInterface::getPlayerInterfaceInstance()->getVol(vol);
						vol-=5;
						if(vol<0){
							vol=0;
						}
						AiBox::PlayerInterface::getPlayerInterfaceInstance()->setVol(vol);
						LOGI(LOG_TAG.c_str(),"volDown=%d\n",vol);
					}
					break;
				}//"https://dingdangcdn.qq.com/news/20200424/20200424A0HZZE.mp3";
				case AiBox::BUTTON_ID_PLAY:{
					if(_button_data._type==AiBox::BUTTON_TYPE_UP){
						if(AiBox::SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){ //waiting the wifi connect
							LOGI(LOG_TAG.c_str(),"Play http media!\n");
							AiBox::media_node_t media_node;
							media_node._media_info._media_type=AiBox::MEDIA_TYPE_MUSIC_HTTP;
							media_node._media_info._uri="http://isure.stream.qqmusic.qq.com/C600002SuEwu3l2bsf.m4a?guid=2000001682&vkey=57BBF679870BB00A7AC5E2DBE754AE74D5D8C2AAE0DD8C748CE6F89D17B7B653335F24CBE220CD5C9E7280C6DB37290F067D797C36C67EF7&uin=&fromtag=234";
							media_node._media_info._start_pos=0;
							media_node._media_info._start_time=0;
							media_node._func=playerStatusCB;
							MediaVehicle::getMediaVehicleInstance()->setMediaData(media_node);
							media_vehicle_data_t media_vehicle;
							media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
							auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
							if(media_queue_handle!=nullptr){
								xQueueSend(media_queue_handle,&media_vehicle,0);
							}else{
								ESP_LOGI(LOG_TAG.c_str(), "Media handle is nullptr.");
							}
						}
					}
					break;
				}
				case AiBox::BUTTON_ID_MUTE:
					if(_button_data._type==AiBox::BUTTON_TYPE_UP){
						LOGI(LOG_TAG.c_str(),"Play raw media!\n");
						int send_len=0;
						if(last_len>segment_len){
							send_len=segment_len;
						}else if(last_len>0){
							send_len=last_len;
						}else{
							LOGI(LOG_TAG.c_str(),"Play done!\n");
							send_len=0;
						}
						//write the raw data to ring buffer
						PlayerInterface::getPlayerInterfaceInstance()->writeRawMediaData(start_pos,send_len);
						if(send_count==0){
							AiBox::media_node_t media_node;
							media_node._media_info._media_type=AiBox::MEDIA_TYPE_TONE_FLASH;
							media_node._media_info._uri="test1";
							media_node._media_info._start_pos=0;
							media_node._media_info._start_time=0;
							media_node._func=playerStatusCB;
							media_node._business_type=Business_Type_NONE;
							MediaVehicle::getMediaVehicleInstance()->setMediaData(media_node);
							media_vehicle_data_t media_vehicle;
							media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
							auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
							if(media_queue_handle!=nullptr){
								xQueueSend(media_queue_handle,&media_vehicle,0);
							}else{
								ESP_LOGI(LOG_TAG.c_str(), "Media handle is nullptr.");
							}
						}
						send_count++;
						start_pos+=send_len;
						last_len-=send_len;
					}
					break;
				default:
					break;
			}
		}
	}
}