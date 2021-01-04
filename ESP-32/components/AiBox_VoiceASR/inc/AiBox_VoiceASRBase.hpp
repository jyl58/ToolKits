/****************************************
*   voice ASR base .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include <string>
#include <functional>
typedef struct BaiduVoiceEvent{
    int event_key;
    uint8_t *data;
    size_t data_len;
}baidu_voice_event_t;
using VoiceHandleFunction= std::function<void (const baidu_voice_event_t&)>;
typedef struct VoiceHanldeTable{
    int _event;
    VoiceHandleFunction _func;
}voice_handle_table_t;
namespace AiBox
{
    class VoiceASRBase
    {
        public:
            VoiceASRBase(){}
            ~VoiceASRBase(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            virtual bool init()=0;
        protected:
    };
};