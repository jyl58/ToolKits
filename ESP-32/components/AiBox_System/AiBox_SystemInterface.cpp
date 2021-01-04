/****************************************
*   system interface for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
    #include "AiBox_ESPSystem.hpp"
#endif
#include "AiBox_Console.hpp"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_FlashInterface.hpp"
#include "AiBox_ESPPlayer.hpp"
using namespace AiBox;
const static std::string LOG_TAG="SystemInterface";

SystemInterface*  SystemInterface::_singleton_instance=nullptr;
SystemInterface::SystemInterface():_system_data(SystemStatusData::getSystemStatusDataInstance())
{
    _singleton_instance=this;
}
SystemInterface::~SystemInterface()
{
    if( _system!=nullptr){
        delete _system;
        _system=nullptr;
    }
}
SystemInterface* 
SystemInterface::getSystemInterfaceInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new SystemInterface();
    }
    return _singleton_instance;
}
void* 
SystemInterface::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
SystemInterface::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
}
bool 
SystemInterface::init()
{
#ifdef AIBOX_VERSION
    LOGI(LOG_TAG.c_str(),"*******************AiBox******************");
    LOGI(LOG_TAG.c_str(),"*   AiBox Version: %s   *",AIBOX_VERSION);
    LOGI(LOG_TAG.c_str(),"*                                        *");
    LOGI(LOG_TAG.c_str(),"*                                        *");
    LOGI(LOG_TAG.c_str(),"******************************************");
#endif
#ifdef ESP32_PLATFORM
    _system=new ESPSystemManage();
#endif
   if( _system==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat System manage err!");
        return false;
    }
    if( !_system->init()){
        LOGI(LOG_TAG.c_str(),"System manage init err!");
        return false;
    }
    //step2:init the console 
    if(!Console::getConsoleInstance()->consoleInit("Hisense>")){
        LOGI(LOG_TAG.c_str(),"Console init err!");
        return false;
    }
    //step3:creat system status queue handle
    if(!initSystemStatusHandle()){
        LOGI(LOG_TAG.c_str(),"System status handle init err!");
        return false;
    }
    //step4: read the wifi config from flash
    readWiFiConfig();
    return true;
}
void 
SystemInterface::readWiFiConfig()
{
    if(FlashInterface::getFlashInterfaceInstance()->openFlash()){
    	int ret=FlashInterface::getFlashInterfaceInstance()->read("WIFI_CONFIG",(void*)&_system_data->_wifi_flash_data,sizeof( wifi_flash_data_t));
        if(ret==FLASH_ERR_NOT_FOUND){
			LOGI(LOG_TAG.c_str(),"WiFi config is empty in flash!!");
		}else if(ret!=FLASH_ERR_NONE){
            LOGI(LOG_TAG.c_str(),"Read WiFi config to flash err!!");
        }
        FlashInterface::getFlashInterfaceInstance()->closeFlash();
	}
}
void 
SystemInterface::storeWiFiConfig(const wifi_link_config_t& wifi_connect_config)
{
    //find the empty postion
    int max_config_num=sizeof(_system_data->_wifi_flash_data._wifi_config)/sizeof(_system_data->_wifi_flash_data._wifi_config[0]);
    //push front
    for(int i=max_config_num-1; i>0; i--){
       _system_data->_wifi_flash_data._wifi_config[i]=_system_data->_wifi_flash_data._wifi_config[i-1];
    }
    _system_data->_wifi_flash_data._wifi_config[0]=wifi_connect_config;
    LOGI(LOG_TAG.c_str(),"store the new wifi config");
    //write to flash
    if(FlashInterface::getFlashInterfaceInstance()->openFlash()){
        if(FlashInterface::getFlashInterfaceInstance()->write("WIFI_CONFIG",(void*)& _system_data->_wifi_flash_data,(std::size_t )sizeof( wifi_flash_data_t))!=FLASH_ERR_NONE){
            LOGI(LOG_TAG.c_str(),"write wifi config to flash err!!");
        }
        FlashInterface::getFlashInterfaceInstance()->closeFlash();
    }
}

bool 
SystemInterface::initSystemStatusHandle()
{
    _sys_status_queue_set_handle=xQueueCreateSet(SYS_QUEUE_SET_LENGTH);
    if(_sys_status_queue_set_handle==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat queue set handle err!");
        return false;
    }
    _wifi_status_queue_handle=xQueueCreate(WIFI_STATUS_LENGTH,sizeof(wifi_status_t));
    if(_wifi_status_queue_handle==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat wifi queue handle err!");
        return false;  
    }
    //add to queue set
    xQueueAddToSet(_wifi_status_queue_handle,_sys_status_queue_set_handle);

    _blue_status_queue_handle=xQueueCreate(BLUE_STATUS_LENGTH,sizeof(ble_status_t));
    if(_blue_status_queue_handle==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat Ble queue handle err!");
        return false;
    }
    //add to queue set
    xQueueAddToSet(_blue_status_queue_handle,_sys_status_queue_set_handle);

    //creat a player status handle
    _player_status_queue_handle=xQueueCreate(PLAYER_STATUS_LENGTH,sizeof(player_status_show_t));
    if(_player_status_queue_handle==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat player status handle err!");
        return false; 
    }
    //add to queue set
    xQueueAddToSet(_player_status_queue_handle,_sys_status_queue_set_handle);

    //creat a sys status main tain task,use cpu core 1, must use the internal memory
    if(xTaskCreatePinnedToCore(&SystemInterface::systemStatusMaintainTask, "sys_maintain", 1024*4, this,10,NULL,1)==NULL){
        LOGI(LOG_TAG.c_str(),"Creat system handle task err!!!");
        return false;
    }
    return true;
}

void 
SystemInterface::systemStatusMaintainTask(void* param)
{
    ((SystemInterface*)param)->exec();
    vTaskDelete(NULL);
}
void 
SystemInterface::exec()
{
    QueueSetMemberHandle_t xActivatedMember;
    wifi_status_t temp_wifi_status;
    while(1){
        xActivatedMember = xQueueSelectFromSet( _sys_status_queue_set_handle,200 / portTICK_PERIOD_MS ); //200ms
        if(xActivatedMember==_wifi_status_queue_handle){
            //step1:recevice the wifi status to _wifi
            memset(&temp_wifi_status,0,sizeof(wifi_status_t));
            xQueueReceive(xActivatedMember,&temp_wifi_status,0);
            //step2: flash ssid and passwd, if success connected to a new ap
            if( !temp_wifi_status._connect_config._ssid.empty()&&
                temp_wifi_status._connect_status
              )
            {
                //need store the ssid an passwd to flash?
                if( _system_data->_wifi._connect_config._ssid.empty()||
                    _system_data->_wifi._connect_config._ssid.compare(temp_wifi_status._connect_config._ssid)!=0 ||
                    _system_data->_wifi._connect_config._passwd.compare(temp_wifi_status._connect_config._passwd)!=0
                  )
                {
                    //step1: store the new ssid and passwd
                    wifi_link_config_t new_wifi_connect;
                    memset(&new_wifi_connect,0,sizeof(wifi_link_config_t));
                    memcpy(new_wifi_connect._ssid,temp_wifi_status._connect_config._ssid.c_str(),temp_wifi_status._connect_config._ssid.size()+1);
                    memcpy(new_wifi_connect._passwd,temp_wifi_status._connect_config._passwd.c_str(),temp_wifi_status._connect_config._passwd.size()+1);
                    storeWiFiConfig(new_wifi_connect);
                    //step2: record the ssid and passwd
                    _system_data->_wifi._connect_config._ssid=temp_wifi_status._connect_config._ssid;
                    _system_data->_wifi._connect_config._passwd=temp_wifi_status._connect_config._passwd;
                }
            }
            //step3: record the new wifi status and ip and mac
            _system_data->_wifi._connect_status=temp_wifi_status._connect_status;
            _system_data->_wifi._ip=temp_wifi_status._ip;
            memcpy(_system_data->_wifi._mac,temp_wifi_status._mac,6);
            LOGI(LOG_TAG.c_str(),"Get WiFi status update: connect=%d,ip=%s",_system_data->_wifi._connect_status,_system_data->_wifi._ip.c_str());
            //step4: first setup creat feature code
            if(_system_data->_JHKDeviceID.empty()){
                char mac[16];
                sprintf(mac,"%X%X%X%X",_system_data->_wifi._mac[2],_system_data->_wifi._mac[3],_system_data->_wifi._mac[4],_system_data->_wifi._mac[5]);
                _system_data->_JHKDeviceID=JHK_FEATURE_CODE+std::string(mac);
                _system_data->_wifiID=WIFI_ID+std::string(mac);
                LOGI(LOG_TAG.c_str(),"Device ID=%s;WiFi ID=%s",_system_data->_JHKDeviceID.c_str(),
                                                               _system_data->_wifiID.c_str());
            }
        }else if(xActivatedMember==_blue_status_queue_handle){
            //recevice the blue status so,write to _ble
            xQueueReceive(xActivatedMember,&_system_data->_ble,0);
            LOGI(LOG_TAG.c_str(),"Get blue update message: connect=%d",_system_data->_ble._connect_status);
        }else if(xActivatedMember==_player_status_queue_handle){
            //recevice the player status so,write _player
            xQueueReceive(xActivatedMember,&_system_data->_player,0);
            LOGI(LOG_TAG.c_str(), "Get Player current status: status=%d,meida=%d,vol=%d,time=%d\n",
                                    _system_data->_player._status,
                                    _system_data->_player._media_src,
                                    _system_data->_player._vol,
                                    _system_data->_player._time);
            //check player status is OK,othwise, reboot it
            if(_system_data->_player._status==PLAYER_STATUS_ERROR){
                //reset player
                if(ESPPlayer::resetPlayer()){
                    //reset the player status
                    _system_data->_player._status=PLAYER_STATUS_UNKNOWN;
                }
            }
        }
    }
}
/*system tools*/
int 
SystemInterface::base64Encode(const std::string& src,std::string& encode_str)
{
    return _system->base64Encode(encode_str,src);
}
int 
SystemInterface::base64Decode(const std::string& src,std::string& decode_str)
{
    return _system->base64Decode(decode_str,src);
}
int 
SystemInterface::encodeURL(const std::string& str, std::string&  after_encode)
{  
    int i;  
    int j = 0;//for result index  
    char ch;  
  
    if (str.empty()) {  
        return -1;  
    }
    char* result=(char*)memMalloc(2048); //get 2k mem
    memset(result,0,1024);
    int str_len=str.size();
    for ( i=0; (i<str_len); ++i) {  
        ch = str[i];  
        if (((ch>='A') && (ch<'Z')) ||  
            ((ch>='a') && (ch<'z')) ||  
            ((ch>='0') && (ch<'9'))) {  
            result[j++] = ch;  
        } else if (ch == ' ') {  
            result[j++] = '+';  
        } else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {  
            result[j++] = ch;  
        } else {  
            if (j+3 < 1024) {  
                sprintf(result+j, "%%%02X", (unsigned char)ch);  
                j += 3;  
            } else {  
                return 0;  
            }  
        }
    }
  
    result[j] = '\0';
    after_encode=std::string(result); 
    memFree(result);
    return j;  
}
int 
SystemInterface::aesEncrypt(const std::string& in_str, const std::string& key, std::string& out_str)
{
    return _system->aesEncrypt(in_str,key,out_str);
}
int 
SystemInterface::aesDecrypt(const std::string& in_str, const std::string& key, std::string& out_str)
{
    return _system->aesDecrypt(in_str,key,out_str);
}
unsigned long long 
SystemInterface::getCurrentTimeUs()
{
    return _system->getCurrentTimeUs();
}
unsigned long long 
SystemInterface::getCurrentTimeMs()
{
    return (unsigned long long)_system->getCurrentTimeUs()*0.001;
}
void* 
SystemInterface::memMalloc(size_t size)
{
    return sysMemMalloc(size);
}
void 
SystemInterface::memFree(void *ptr)
{
    if(ptr==nullptr){
        return;
    }
    sysMemFree(ptr);
}