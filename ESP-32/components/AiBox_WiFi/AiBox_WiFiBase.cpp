/****************************************
*   wifi base for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#include <cstring>
#include "LogInterface.hpp"
#include "AiBox_WiFiBase.hpp"
#include "AiBox_FlashInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="WiFiBase";
int wiFiConfigCMD(int argc, char **argv)
{
    int ret=0;
    wifi_flash_data_t wifi_flash_data;
    if(FlashInterface::getFlashInterfaceInstance()->openFlash()){
        ret=FlashInterface::getFlashInterfaceInstance()->read("WIFI_CONFIG",(void*)&wifi_flash_data,sizeof( wifi_flash_data_t));
        if(ret==FLASH_ERR_NOT_FOUND){
			LOGI(LOG_TAG.c_str(),"WiFi config is empty in flash!!");
		}else if(ret!=FLASH_ERR_NONE){
            LOGI(LOG_TAG.c_str(),"Read WiFi config to flash err!!");
        }
        FlashInterface::getFlashInterfaceInstance()->closeFlash();
	}
    if(ret!=FLASH_ERR_NONE){
        return 0;
    }
    for(int i=0;i< sizeof(wifi_flash_data._wifi_config)/sizeof(wifi_flash_data._wifi_config[0]); i++){
        LOGI(LOG_TAG.c_str(),"Wifi condif index=%d, ssid=%s,passwd=%s",i,wifi_flash_data._wifi_config[i]._ssid,wifi_flash_data._wifi_config[i]._passwd);
    }
    return 0;
}
void* 
WiFiBase::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
WiFiBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}