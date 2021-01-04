/****************************************
*   voice ASR  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include <string>
#include "AiBox_VoiceASRBase.hpp"
namespace AiBox
{
    class VoiceASR
    {
        public:
            VoiceASR(const VoiceASR&)=delete;
            void operator=(const VoiceASR&)=delete;
            ~VoiceASR(){}
            static VoiceASR* getVoiceASRInstance();
            bool initVoiceASR();

        private:
            VoiceASR(){}
            VoiceASRBase* _voice_asr{nullptr};
            static VoiceASR* _singleton_instance;
    };
};