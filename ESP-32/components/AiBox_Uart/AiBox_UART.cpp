/****************************************
*   uart interface for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.09
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_UART.h"
#ifdef ESP32_PLATFORM
    #include "AiBox_UARTESP32.h"
#endif
using namespace AiBox;
const std::string LOG_TAG="AiBoxUART";
SystemUart* SystemUart::_singleton_instance=nullptr;
SystemUart* SystemUart::getSystemUartInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new SystemUart();
    }
    return _singleton_instance;
}

bool 
SystemUart::init()
{
#ifdef ESP32_PLATFORM
    _uart=new ESP32Uart();
#endif
    if(!_uart->init())
    {
        LOGI(LOG_TAG.c_str(),"uart init err!!");
        return false;
    }
    return true;
}
int 
SystemUart::write(const std::string& command)
{
    if(_uart==nullptr){
        return -1;
    }
    return _uart->write(command);
}
int 
SystemUart::read(std::string& msg,int size)
{
    if(_uart==nullptr){
        return -1;
    }
    return _uart->read(msg,size);
}
const QueueHandle_t& 
SystemUart::getUartEventHandle()
{
    if(_uart==nullptr){
        return NULL;
    }
    return _uart->_commnd_uart_event_handle;
}