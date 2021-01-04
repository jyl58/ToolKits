/****************************************
*   Board interface for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#include "AiBox_BoardInterface.hpp"
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
	#include "AiBox_BoardESP.hpp"
#endif

using namespace AiBox;
const static std::string LOG_TAG="BoardInterface";
BoardInterface* BoardInterface::_singleton_instance=nullptr;
BoardInterface* BoardInterface::getBoardInterfaceInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new BoardInterface();
    }
    return _singleton_instance;
}
BoardInterface::BoardInterface()
{
    _singleton_instance=this;
}
bool 
BoardInterface::initBoard()
{
#ifdef ESP32_PLATFORM
    _board= new ESPBoard();
#endif
    if(_board==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat Board manage err!");
        return false;
    }
    if(!_board->init()){
        LOGI(LOG_TAG.c_str(),"Board init err!");
        return false;
    }
    return true;
}
QueueHandle_t 
BoardInterface::getButtonQueueHandle()
{
    if(_board==nullptr){
        return nullptr;
    }
    return _board->_button_queue_handle;
}