/****************************************
*   ASR process class .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.16
*****************************************/
#pragma once
#include <string>
#define ENABLE_CUSTOMID_UPLOAD 

#define SEMANTIC_URL_PROJECT       "http://nlp.hismarttv.com:83/aicloud_d1/getDataContent"   //203.130.44.109:83
#define SEMANTIC_URL_FAULT         "http://nlp.hismarttv.com:83/aicloud_d1/errupload"
#define SEMANTIC_URL_MUSIC         "http://nlp.hismarttv.com:83/aicloud_d1/uploadMusicList"
#ifdef ENABLE_CUSTOMID_UPLOAD
    #define SEMANTIC_URL_CUSTOMID  "http://nlp.hismarttv.com:83/aicloud_d1/uploadCustomerId"
#endif
extern "C" int processASRCMD(int argc, char **argv);
namespace AiBox
{
    enum UploadDataType
    {
        DATA_TYPE_SEMANTIC_ANALYSIS = 0,
        DATA_TYPE_MUSIC_INFO,
        DATA_TYPE_FAULT_LOGCAT,
    #ifdef ENABLE_CUSTOMID_UPLOAD
        DATA_TYPE_CUSTOM_ID,
    #endif
    };
    class ASRProcess
    {
        public:
            ASRProcess();
            ASRProcess(const ASRProcess&)=delete;
            void operator=(const ASRProcess&)=delete;
            ~ASRProcess();
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            static ASRProcess* getProcessASRInstance();
            std::string createMusicJson(const std::string& sessionID, const std::string& musicTTS);
            std::string createSystemFaultJson(const std::string& sessionID,const std::string& errType,const std::string& desc, const std::string& engineType,unsigned int cost=0);
            std::string HttpPostDataCreate(const std::string& strText, int dataType);
            void ProcessASRResult(const std::string& asr_result,const std::string& wakeup_sessionid);
            static void semanticResponseCallBack(const std::string& response,int response_code);
            //upload system fault
            void uploadSystemFault(const std::string& upload_fault_json);
            static void uploadSystemFaultCallBack(const std::string& response,int response_code);

            //upload music info
            void uploadMusicInfo(const std::string& upload_json);
            static void uploadMusicInfoCallBack(const std::string& response,int response_code);

            //upload custom id
            void uploadCustomID();
            static void uploadCustomIDCallBack(const std::string& response,int response_code);
        private:
            const std::string VIDAA_VERSION{"5.3"};
            const std::string PACKAGE_NAME{"aiVoiceD@"};
            const std::string _mic_arrayfac{"AISPEECH"};
            static ASRProcess* _singleton_instance;
    };
};
