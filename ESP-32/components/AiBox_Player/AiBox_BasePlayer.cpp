 /****************************************
*   ESP base player .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.29
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_BasePlayer.hpp"
using namespace AiBox;
const std::string LOG_TAG="BasePlayer";
void*
BasePlayer::operator new(std::size_t size)throw(std::bad_alloc)
{
	return sysMemMalloc(size);
}
void 
BasePlayer::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
	sysMemFree(ptr);
	ptr=nullptr;
}
void 
BasePlayer::clearRawStreamQueue()
{
	tts_stream_ptr_t tts_stream_info;
    memset(&tts_stream_info,0,sizeof(tts_stream_ptr_t));
    //clear the queue message
    if (uxQueueSpacesAvailable(_tts_stream_queue_handle) < TTS_STREAM_QUEUE_LENGTH) {
        int recv_cnt =  TTS_STREAM_QUEUE_LENGTH - uxQueueSpacesAvailable(_tts_stream_queue_handle);
        LOGI(LOG_TAG.c_str(),"Flush Raw Stream queue! recvd: %d\n", recv_cnt);
        for (int i = 0; i < recv_cnt; i++) {
            if (xQueueReceive(_tts_stream_queue_handle, &tts_stream_info, 0) == pdPASS) {
                sysMemFree(tts_stream_info.data);
            }
        }
    }
}