/****************************************
*   Semantic Parse .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.19
*****************************************/
#pragma once
#include <string>
#include <functional>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "cJSON.h"
#include "AiBox_SemanticDataType.hpp"
#define DIRECTIVE_MAX_LEN 10
//service  handle function
using ServiceHandleFunction= std::function<void (const cJSON *)>;
typedef struct ServiceHandleTalbe{
    char _name[50];
    ServiceHandleFunction _handle_func;
    bool _support_continue;
}semantic_service_handle_table_t;
typedef struct ParseStatusFlags{
    int needResume;
    int needPlayTTS;
}parse_status_t;
extern "C" int processSemanticCMD(int argc, char **argv);
namespace AiBox
{
    class SemanticParse
    {
        public:
            SemanticParse();
            SemanticParse(const SemanticParse&)=delete;
            SemanticParse& operator=(const SemanticParse&)=delete;
            ~SemanticParse(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            static SemanticParse* getSemanticParseInstance();
            const QueueHandle_t& getSemanticDirectiveQueueHandle(){return _directive_queue_handle;}
            void initSemanticParse();// clear the current Semantic Parse status,after wakeup
            std::string parseJsonStr(const std::string& JsonStr, struct ParseStatusFlags* parse_status);
            /**
             * return:
             *     false---one-shot
             *     true---continuous
             */
            bool getOneShotMode(){return _isOneShotMode;}
            int getTTSSpeed(){return _ttsSpeed;}

            std::string& getRequestSessionId(){return _requestSessionId;}
            //voice record mode 
            bool getVoiceMemoryMode(){return _isVoiceMemoryMode;}
            void setVoiceMemoryMode(bool isVoiceRecord){_isVoiceMemoryMode = isVoiceRecord;}
            //multi chat mode 
            bool getMultiChatMode(){return _isMultiChatMode;}
           
            bool isNeedPowerOff(){return _needPowerOffFlag;}
            void setNeedPowerOff(bool isNeed){_needPowerOffFlag = isNeed;}
            
        private:
            std::string getTtsText();
            void setOneShotMode(bool val){_isOneShotMode=val;}
            void setMultiChatMode(bool value){_isMultiChatMode=value;}
            //musicX 
           // void serviceHandleMusicX(const cJSON* pData);
            //void parseMusicXSemantic(const cJSON* subjson,const std::string item,char* value);
            static void serviceHandleMusicY(const cJSON* pData);
            //tv control
            static void serviceHandleTvControl(const cJSON* pData);
            static void tvControlExit(const cJSON* pDataControl);
            static void tvControlViceMemo(const cJSON* pDataControl);
            static void tvControlVolume(const cJSON* subjson);
            static void tvControlStatusVersion(const cJSON* subjson);
            static void tvControlPush(const cJSON* subjson);
            static const struct ServiceHandleTalbe _tv_control_item[];

            static void serviceHandleScheduleX(const cJSON* pData);
            static void serviceHandleSmartHome(const cJSON* pData);
            static void serviceHandleCustomSkills(const cJSON* pData);
            static void serviceHandleScene(const cJSON* pData);
            static void serviceHandleTvModel(const cJSON* pData);
            static void serviceHandleKLL3(const cJSON* pData);
            static void serviceHandleAudioProgram(const cJSON* pData);
            static void serviceHandleAIUI(const cJSON* pData);
            //weather
            static void serviceHandleWeather(const cJSON* pData);
            //Multi Chat
            static void serviceHandleMultiChat(const cJSON* pData);

            int handleXFMedia(const cJSON *pResult, char isloop, const std::string& keyUrl, const std::string& ttsParam);
            int handleJHKMedia(const cJSON *pDataResult, cJSON *pUICommand, const std::string& ttsContentParam, char isloop, const std::string& sessionID);
            void playJHKMediaList(const msg_jhkmedia_list_t& msgJhkList);
            char handlePlayMode(const cJSON *pJSONData);
            void handlerVoiceMemory(int actionParam, const std::string& backupContent);
            void controlBox(const std::string& curService, const std::string& action, int val);
            void sendDirectiveQueue(const directive_queue_t& directive_data);
            void handlerTvPartner(int actionParam, const std::string& strTVCommand);
            void sendCommonData(directive_type_t id,const std::string& content);
            void playUrlList(const msg_url_list_t& url_list);
            void controlSchedule(const std::string& dateTime, const std::string& suggestTime,const std::string& content, const std::string& intent, const std::string& type, const std::string& repeat, const std::string& scene);
            cJSON* _pJson=NULL;
            std::string _currentService;
            std::string _sAnswer;
            parse_status_t _parse_status_flag;
            //one shot mode
            bool _isOneShotMode;
            int _ttsSpeed;
            
            std::string _requestSessionId;
            //voice record mode 
            bool _isVoiceMemoryMode;
            //multi chat mode 
            bool _isMultiChatMode;
            bool _needPowerOffFlag;
            //contimue mode flag
            bool _iscontinueMode;

            static const semantic_service_handle_table_t _sevice_handle_table[];
            static const std::string KEY_TTS_SS;
            static SemanticParse* _singleton_instance;
            QueueHandle_t _directive_queue_handle=nullptr;
    };
};