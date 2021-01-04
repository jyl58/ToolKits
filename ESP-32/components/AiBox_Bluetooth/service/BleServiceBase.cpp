/****************************************
*   ESP Ble service base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.04
*****************************************/
#include "LogInterface.hpp"
#include "BleServiceBase.hpp"
using namespace AiBox;
static const std::string LOG_TAG = "BleServiceBase";
void* 
BleServiceProfileBase::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
BleServiceProfileBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}

