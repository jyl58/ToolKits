/****************************************
*   voice ASR base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.14
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_VoiceASRBase.hpp"
#include "bds_client_event.h"
using namespace AiBox;
static const std::string LOG_TAG="VoiceBase";
void*
VoiceASRBase::operator new(std::size_t size)throw(std::bad_alloc)
{
	return sysMemMalloc(size);
}
void 
VoiceASRBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
	sysMemFree(ptr);
	ptr=nullptr;
}