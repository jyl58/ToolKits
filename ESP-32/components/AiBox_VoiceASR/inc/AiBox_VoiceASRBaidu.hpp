/****************************************
*   voice ASR baidu .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
extern "C"{
#include "bds_client_command.h"
#include "bds_client_context.h"
#include "bds_client_event.h"
#include "bds_client_params.h"
#include "bds_client.h"
#include "bds_common_utility.h"
}
#include "AiBox_VoiceASRBase.hpp"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_author.hpp"
#define EVENT_ENGINE_QUEUE_LEN 256
typedef struct {
    int err;
    int idx;
    int data_len;
    int data_left;
    bool playing;
} TTS_HEADER;
namespace AiBox
{
    class VoiceAuthor;
    class VoiceASRBaidu : public VoiceASRBase
    {
        public:
            VoiceASRBaidu();
            ~VoiceASRBaidu(){}
            bool init()override;
            void startVoiceEngine();
            void stopVoiceSDK();
            void destoryVoiceSDK();
            typedef struct {
                std::string fc;
                std::string pk;
                std::string ak;
                std::string sk;
                std::string cuid;
                std::string mqtt_broker;
                std::string mqtt_username;
                std::string mqtt_password;
                std::string mqtt_cid;
            } vendor_info_t;
            vendor_info_t& getVendorInfo(){return _g_vendor_info;}
            bool startASRSystem();
        private:
            VoiceAuthor* _author=nullptr;
            static int32_t bdscEventCallback(bds_client_event_t *event, void *custom);
            static void VoiceEventHandleTask(void* param);
            static void eventEngineElemEnQueue(int event, uint8_t *buffer, size_t len);
            static void eventEngineElemFlushQueue();
            bool confirmProfile();
            void bdscConfigNqe( ASR_FORCE_MODE mode);
            void bdscDynamicConfig(char *config);
            
            void generateDcsPamNeedFree(std::string& doc_param);
            /*cmd*/
            void bdscLinkStart();
            void bdscLinkStop();
            void bdscStopWakeup();
            void bdscStartWakeup();
            void bdscStartASR(int back_time);
            int generateASRThirdPartyPam(std::string& pam_prama, size_t max_len);
            std::string generateAuthSigNeedfree(const char *ak, const int ts, const char *sk);
            void decimalToHex(unsigned char *arr, size_t arr_len, char *hex_string);
            void exec();
            /*tts*/
            void readStream(char* buf);
            char* getTTSHeader(char* buffer, uint16_t* length);
            short covertToShort(char* buffer);
            int covertToInt(char* buffer);
            /*handle*/
            static void handleWakeup(const baidu_voice_event_t& wakeup_data);
            static void handleASREnd(const baidu_voice_event_t& asr_end_data);
            static void handleTTSData(const baidu_voice_event_t& tts);
            static void handleASRResult(const baidu_voice_event_t& asr_result);
            static void handleLinkERR(const baidu_voice_event_t& link_err);
            
            static bds_client_context_t _bds_context_client;
            static bds_client_handle_t _g_client_handle;
            static QueueHandle_t _g_voice_event_queue;
            vendor_info_t _g_vendor_info;
            TTS_HEADER _tts_info;
            static const voice_handle_table_t _voice_handle_table[];
            static VoiceASRBaidu*  _singleton_instance;
    };
};