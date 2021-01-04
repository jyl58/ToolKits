/****************************************
*   Ble interface to app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#include <cstring>
#include "LogInterface.hpp"
#include "AiBox_BleInterface.hpp"
#ifdef ESP32_PLATFORM
	#include "AiBox_BleESP.hpp"
#endif
using namespace AiBox;
static const std::string LOG_TAG = "BleInterface";
BleInterface* BleInterface::_singleton_instance=nullptr;
BleInterface* BleInterface::getBleInterfaceInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new BleInterface();
    }
    return _singleton_instance;
}
BleInterface::BleInterface()
{
    _singleton_instance=this;
}
BleInterface::~BleInterface()
{

}
void 
BleInterface::getBleAddr(uint8_t* addr)
{
    memcpy(addr,_ble->_ble_status._addr,6);
}
void 
BleInterface::getBleStatus(ble_status_t& ble_status)
{
    ble_status=_ble->_ble_status;
}
bool 
BleInterface::initBle()
{
#ifdef ESP32_PLATFORM
	_ble=new ESPBleManage();
#endif
    if(_ble==nullptr){
        LOGD(LOG_TAG.c_str(),"Creat Bluetooth manage err!");
        return false;
    }
    if(!_ble->init()){
        LOGD(LOG_TAG.c_str(),"Bluetooth manage init err!");
        return false;
    }
    return true;
}
bool
BleInterface::openBle()
{
    return _ble->open();
}
bool 
BleInterface::closeBle()
{
    return _ble->close();
}