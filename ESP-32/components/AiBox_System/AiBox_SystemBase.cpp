/****************************************
*   system base  .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_SystemBase.hpp"
using namespace AiBox;
const static std::string LOG_TAG="SystemBase";

const std::string SystemBase::SEMANTIC_AES_KEY = "speech_data@d1v1";
const std::string SystemBase::SEMANTIC_AES_KEY_SMARTHOME= "HiSmartHome_B1f2";

const std::string SystemBase::SEMANTIC_KEY_TV = "getdatacontent";
const std::string SystemBase::SEMANTIC_KEY_SMARTHOME = "shomeDevicesUpload";

const std::string SystemBase::SEMANTIC_AES_KEY_MUSIC = "uf09b217opemvy5x";
const std::string SystemBase::SEMANTIC_KEY_MUSIC = "uploadmusiclist";

const std::string SystemBase::SEMANTIC_AES_KEY_FAULT = "speech_data@d1e1";
const std::string SystemBase::SEMANTIC_KEY_FAULT = "errupload";

#ifdef ENABLE_CUSTOMID_UPLOAD
const std::string SystemBase::SEMANTIC_AES_KEY_CUSTOMID = "speech_data@d1c1";
const std::string SystemBase::SEMANTIC_KEY_CUSTOMID =  "uploadcustomerid";
#endif

void* 
SystemBase::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
SystemBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
}