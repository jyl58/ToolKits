/****************************************
*   system status data for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.09
*****************************************/
#include <cstring>
#include "LogInterface.hpp"
#include "AiBox_SystemStatusData.hpp"
using namespace AiBox;
SystemStatusData* SystemStatusData::_singleton_instance=nullptr;
SystemStatusData* SystemStatusData::getSystemStatusDataInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new SystemStatusData();
    }
    return _singleton_instance;
}
void* 
SystemStatusData::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
SystemStatusData::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}

SystemStatusData::SystemStatusData()
{
    _singleton_instance=this;
    _wifi._connect_status=false;
    _wifi._connect_config._passwd=std::string();
    _wifi._connect_config._ssid=std::string();
    _wifi._ip=std::string();
    memset(&_wifi_flash_data,0,sizeof(wifi_flash_data_t));
    memset(&_ble,0,sizeof(ble_status_t));
    memset(&_player,0,sizeof(player_status_show_t));
    _wakeupKey="HXXJ";
}
bool 
SystemStatusData::getWifiConfig(wifi_connect_config_t& wifi_connect_config,int prio)
{
    int max_wifi_config_num=sizeof(_wifi_flash_data._wifi_config)/sizeof(_wifi_flash_data._wifi_config[0]);
    if(prio>=max_wifi_config_num){
        return false;
    }
    wifi_connect_config._ssid=std::string(_wifi_flash_data._wifi_config[prio]._ssid);
    wifi_connect_config._passwd=std::string(_wifi_flash_data._wifi_config[prio]._passwd);
    //update the wifi status
    if(prio==0){
        //record the first prio wifi config
        _wifi._connect_config._ssid=wifi_connect_config._ssid;
        _wifi._connect_config._passwd=wifi_connect_config._passwd;
    }
    return true;
}