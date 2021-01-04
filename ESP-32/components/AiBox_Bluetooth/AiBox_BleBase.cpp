/****************************************
*   Ble Base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_BleBase.hpp"
using namespace AiBox;
static const std::string LOG_TAG = "BleBase";
ble_status_t BleBase::_ble_status;
bool BleBase::_blue_initialized=false;
void* 
BleBase::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
BleBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}