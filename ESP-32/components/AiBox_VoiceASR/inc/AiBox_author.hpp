/****************************************
*   voice ASR author .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.19
*****************************************/
#pragma once
#include "AiBox_VoiceASRBaidu.hpp"
namespace AiBox
{
     class VoiceASRBaidu;
     class VoiceAuthor
    {
        public:
            VoiceAuthor(){}
            ~VoiceAuthor();
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            bool init(VoiceASRBaidu* baidu_asr);
            static void voiceAuthorTask(void* param);
        private:
            void voiceAuthorExec();
            bool generateAuthBodyNeedfree(std::string& request_content);
            int generateAuthPam(char* pam_prama, size_t max_len);
            bool saveAuthorMsg();
            VoiceASRBaidu* _baidu_asr{nullptr};
    };
};