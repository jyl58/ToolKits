/****************************************
*   voice ASR  .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#include "AiBox_VoiceASR.hpp"
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
    #include "AiBox_VoiceASRBaidu.hpp"
#endif
using namespace AiBox;
const std::string LOG_TAG="VoiceASR";
VoiceASR* VoiceASR::_singleton_instance=nullptr;
VoiceASR* 
VoiceASR::getVoiceASRInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new VoiceASR();
    }
    return _singleton_instance;
}
bool 
VoiceASR::initVoiceASR()
{
#ifdef ESP32_PLATFORM
    _voice_asr= new VoiceASRBaidu();
#endif 
    if(_voice_asr==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat voice instance err!!");
        return false;
    }
    if(!_voice_asr->init()){
        LOGI(LOG_TAG.c_str(),"Init voice instance err!!");
        return false;
    }
    return true;
}
