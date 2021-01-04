/****************************************
*   Semantic Parse .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.19
*****************************************/
#include <cstring>
#include "AiBox_SemanticParse.hpp"
#include "LogInterface.hpp"
#include "AiBox_SystemInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="SemanticParse";
int processSemanticCMD(int argc, char **argv)
{
    if(argc!=2){
        LOGI(LOG_TAG.c_str(),"semantic need a content param\n");
        return 0;
    }
    std::string Semantic_string=argv[1];
    ParseStatusFlags parse_status_flags;
	memset(&parse_status_flags,0,sizeof(ParseStatusFlags));
    SemanticParse::getSemanticParseInstance()->parseJsonStr(Semantic_string,&parse_status_flags);
    return 0;
}
const std::string SemanticParse::KEY_TTS_SS="tts";
const semantic_service_handle_table_t SemanticParse::_sevice_handle_table[]={
    //{"musicX",&_singleton_instance->serviceHandleMusicX,false},
    //{"common_1",&_singleton_instance->serviceHandleMusicX,false},
    {"musicY",&SemanticParse::serviceHandleMusicY,true},
    {"tvcontrol",&SemanticParse::serviceHandleTvControl,false},
    {"audiocontrol",&SemanticParse::serviceHandleTvControl,false},
    {"common_2",&SemanticParse::serviceHandleTvControl,false},
    {"common_3",&SemanticParse::serviceHandleTvControl,false},

    {"scheduleX",&SemanticParse::serviceHandleScheduleX,false},
    {"common_4",&SemanticParse::serviceHandleScheduleX,false},

    {"SMART_HOME",&SemanticParse::serviceHandleSmartHome,false},
    {"common_5",&SemanticParse::serviceHandleSmartHome,false},
    {"customSkills",&SemanticParse::serviceHandleCustomSkills,false},
    {"news.scene",&SemanticParse::serviceHandleScene,false},
    {"weather.scene",&SemanticParse::serviceHandleScene,false},
    {"tvmodel",&SemanticParse::serviceHandleTvModel,false},
    {"common_6",&SemanticParse::serviceHandleTvModel,false},

    {"KLLI3.poetryChallenge",&SemanticParse::serviceHandleKLL3,false},
    {"LEIQIAO.moraGame",&SemanticParse::serviceHandleKLL3,false},
    {"LEIQIAO.BMI",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.BBaishuaya",&SemanticParse::serviceHandleKLL3,false},
    {"RUYIAI.RuyiChinese",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.YYshiwu",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.CFB",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.BBxishou",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.BBxitou",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.MSC",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.DMXshiwu",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.ZWDMX",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.ZJxiaozhu",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.DMXkonglong",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.BBXsanjiaoxing",&SemanticParse::serviceHandleKLL3,false},
    {"KLLI3.skipNumber",&SemanticParse::serviceHandleKLL3,false},
    {"LEIQIAO.voiceCopy",&SemanticParse::serviceHandleKLL3,false},
    {"AIUI.flipCoin",&SemanticParse::serviceHandleKLL3,false},
    {"EGO.healthKnowledge",&SemanticParse::serviceHandleKLL3,false},
    {"PHOTO.challenge",&SemanticParse::serviceHandleKLL3,false},
    {"LEIQIAO.timesTable",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.YYshiwu",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.sanzijing",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.ZWDMX",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.caicaiwoshishui",&SemanticParse::serviceHandleKLL3,false},
    {"RUYIAI.RuyiIdiomGame",&SemanticParse::serviceHandleKLL3,false},
    {"MYDEMOTEST.BBaixizao",&SemanticParse::serviceHandleKLL3,false},
    {"AIUI.Riddle",&SemanticParse::serviceHandleKLL3,false},
    {"AIUI.brainTeaser",&SemanticParse::serviceHandleKLL3,false},
    {"AIUI.guessNumber",&SemanticParse::serviceHandleKLL3,false},
    {"common_7",&SemanticParse::serviceHandleKLL3,false},

    {"audioProgram",&SemanticParse::serviceHandleAudioProgram,true},
    {"novel",&SemanticParse::serviceHandleAudioProgram,false},
    {"health",&SemanticParse::serviceHandleAudioProgram,false},
    {"drama",&SemanticParse::serviceHandleAudioProgram,false},
    {"story",&SemanticParse::serviceHandleAudioProgram,false},
    {"radio",&SemanticParse::serviceHandleAudioProgram,true},
    {"internetRadio",&SemanticParse::serviceHandleAudioProgram,false},
    {"storyTelling",&SemanticParse::serviceHandleAudioProgram,false},
    {"crossTalk",&SemanticParse::serviceHandleAudioProgram,false},
    {"LEIQIAO.funnyPassage",&SemanticParse::serviceHandleAudioProgram,false},
    {"LEIQIAO.speech",&SemanticParse::serviceHandleAudioProgram,false},
    {"LEIQIAO.openClass",&SemanticParse::serviceHandleAudioProgram,false},
    {"drama",&SemanticParse::serviceHandleAudioProgram,false},
    {"story",&SemanticParse::serviceHandleAudioProgram,false},
    {"crossTalk",&SemanticParse::serviceHandleAudioProgram,false},
    {"audio.hisense",&SemanticParse::serviceHandleAudioProgram,true},
    {"common_8",&SemanticParse::serviceHandleAudioProgram,false},

    {"AIUI.ocularGym",&SemanticParse::serviceHandleAIUI,false},
    {"animalCries",&SemanticParse::serviceHandleAIUI,false},
    {"news",&SemanticParse::serviceHandleAIUI,true},
    {"KLLI3.studyPinYin",&SemanticParse::serviceHandleAIUI,false},
    {"RUYIAI.RuyiMuseMusic",&SemanticParse::serviceHandleAIUI,false},
    {"history",&SemanticParse::serviceHandleAIUI,false},
    {"AIUI.Bible",&SemanticParse::serviceHandleAIUI,false},
    {"AIUI.smarter",&SemanticParse::serviceHandleAIUI,false},
    {"AIUI.meditationTime",&SemanticParse::serviceHandleAIUI,false},
    {"AIUI.sleepWell",&SemanticParse::serviceHandleAIUI,false},
    {"AIUI.whiteNoise",&SemanticParse::serviceHandleAIUI,false},
    {"common_9",&SemanticParse::serviceHandleAIUI,false},
//weather    
    {"weather",&SemanticParse::serviceHandleWeather,true},
//multi chat mode
    {"multichat",&SemanticParse::serviceHandleMultiChat,false}
};
SemanticParse* SemanticParse::_singleton_instance=nullptr;
SemanticParse* 
SemanticParse::getSemanticParseInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new SemanticParse();
    }
    return _singleton_instance;
}
void* 
SemanticParse::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
SemanticParse::operator delete(void *ptr)throw()
{
	if(ptr==nullptr) return;
	sysMemFree(ptr);
	ptr=nullptr;
}
SemanticParse::SemanticParse()
{
    //creat a directive queue handle
    _directive_queue_handle= xQueueCreate(DIRECTIVE_MAX_LEN, sizeof(directive_queue_t));
    if(_directive_queue_handle==nullptr){
        LOGI(LOG_TAG.c_str(), "Creat Semantic queue handle err!!");
    }
    _singleton_instance=this;
}
void 
SemanticParse::initSemanticParse()
{
    _currentService.clear();
    _iscontinueMode=false;
}
void 
SemanticParse::sendDirectiveQueue(const directive_queue_t& directive_data)
{
    //send the qq music to directive queue
    if(_directive_queue_handle!=nullptr){
        xQueueSend(_directive_queue_handle,&directive_data,0);
    }
}
std::string  
SemanticParse::getTtsText()
{
    if(_pJson==NULL){
        return NULL;
    }
    //get semantic text content
    cJSON *pAnswer = cJSON_GetObjectItem(_pJson, KEY_TTS_SS.c_str());
    if (NULL == pAnswer){
        return NULL;
    }
    cJSON *pAnswerText = cJSON_GetObjectItem(pAnswer, "text");
    if (NULL == pAnswerText){
        return NULL;
    }
    return std::string(pAnswerText->valuestring);
}
void 
SemanticParse::serviceHandleMusicY(const cJSON* pData)
{
    _singleton_instance->_parse_status_flag.needResume = 0;
    _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL == pData){
       _singleton_instance->_parse_status_flag.needResume = 1;
       _singleton_instance->_parse_status_flag.needPlayTTS = 1;
       return;
    }
    LOGI(LOG_TAG.c_str(),"---musicy data--\n");
    cJSON *pResult = cJSON_GetObjectItem(pData, "result");
    //handle the play mode
    char isloop = _singleton_instance->handlePlayMode(pData);
    if (NULL == pResult){
        _singleton_instance->_parse_status_flag.needResume = 1;
        _singleton_instance->_parse_status_flag.needPlayTTS = 1;
        return;
    }
    int arrSize = cJSON_GetArraySize(pResult);
    LOGI(LOG_TAG.c_str(),"musicy array size=%d\n", arrSize);
    arrSize = (arrSize > TOTAL_URL_LIST) ? TOTAL_URL_LIST : arrSize; //change
    if (arrSize <= 0){
        _singleton_instance->_parse_status_flag.needResume = 1;
        _singleton_instance->_parse_status_flag.needPlayTTS = 1;
        return;
    }

    int i, totalUrl = 0;
    cJSON *pObject;
    msg_qqmusic_t itemUrl;
    MsgQQMusicList msgUrlList;
    msgUrlList.isloop = isloop;

    for (i = 0; i < arrSize; i++)
    {
        pObject = cJSON_GetArrayItem(pResult, i);
        if (NULL != pObject)
        {
            cJSON *pBackgroundUrl = cJSON_GetObjectItem(pObject, "url");
            memset(&itemUrl, 0, sizeof(itemUrl));
            if ((pBackgroundUrl != NULL) && (NULL != pBackgroundUrl->valuestring))
            {
                itemUrl.url=std::string(pBackgroundUrl->valuestring);
            }
            cJSON *pSongID = cJSON_GetObjectItem(pObject, "songID");
            if ((pSongID != NULL) && (NULL != pSongID->valuestring))
            {
                itemUrl.song_id=std::string(pSongID->valuestring);
            }
            cJSON *pSong = cJSON_GetObjectItem(pObject, "song");
            if ((pSong != NULL) && (NULL != pSong->valuestring))
            {
                itemUrl.song=std::string(pSong->valuestring);
            }
            cJSON *pSinger = cJSON_GetObjectItem(pObject, "singer");
            if ((pSinger != NULL) && (NULL != pSinger->valuestring))
            {
                itemUrl.singer=std::string(pSinger->valuestring);
            }
            cJSON *pAlbumID = cJSON_GetObjectItem(pObject, "albumID");
            if ((pAlbumID != NULL) && (NULL != pAlbumID->valuestring))
            {
                itemUrl.album_id=std::string(pAlbumID->valuestring);
            }
            memcpy(&(msgUrlList.url_item[totalUrl++]), &itemUrl, sizeof(msg_qqmusic_t));
        }
    }
    msgUrlList.url_num = totalUrl;
    if (!_singleton_instance->_sAnswer.empty()){
        msgUrlList.tts_content=_singleton_instance->_sAnswer;
    }
    MsgQQMusicListClass* msgUrlListclass=MsgQQMusicListClass::getSemanticDataSingletonPtr();
    msgUrlListclass->setData(msgUrlList);
    directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_QQMUSIC;
    directive_data._directive_ptr=(void*)msgUrlListclass;
    _singleton_instance->sendDirectiveQueue(directive_data);
}

/*tv*/
const struct ServiceHandleTalbe SemanticParse::_tv_control_item[]=
{
    {"exit",&SemanticParse::tvControlExit,false},
    {"voiceMemo",&SemanticParse::tvControlViceMemo,false},
    {"statusVolume",&SemanticParse::tvControlVolume,false},
    {"statusVersion",&SemanticParse::tvControlStatusVersion,false},
    {"tvPush",&SemanticParse::tvControlPush,false}
};
void 
SemanticParse::tvControlExit(const cJSON* pDataControl)
{
    cJSON *pDataControlParam = cJSON_GetObjectItem(pDataControl, "param");
    if (NULL != pDataControlParam)
    {
        char *valtemp = pDataControlParam->valuestring;
        if ((valtemp != NULL) && (!strcmp(valtemp, "hypnotic")))
        {
            _singleton_instance->controlBox(_singleton_instance->_currentService, "cancelPowerOff", 0);
        }
    }else if(_singleton_instance->_iscontinueMode){
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        _singleton_instance->_iscontinueMode=false;
    }else {
        _singleton_instance->controlBox("audiocontrol", "stop", 0);
        _singleton_instance->_parse_status_flag.needResume = 0;
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    }
}
void 
SemanticParse::tvControlViceMemo(const cJSON* pDataControl)
{
    cJSON *pDataControlParam = cJSON_GetObjectItem(pDataControl, "param");
    if (NULL == pDataControlParam){
        return;
    }
    int voiceParam = pDataControlParam->valueint;
    _singleton_instance->_parse_status_flag.needResume = 0;
    LOGI(LOG_TAG.c_str(),"voice backup param :%d\n", voiceParam);
    if (voiceParam == 0){   //0 
        _singleton_instance->_parse_status_flag.needPlayTTS = 1;
        _singleton_instance->setOneShotMode(false);
        _singleton_instance->setVoiceMemoryMode(true);
    }else if(voiceParam == 2){ //delete memo=2
        cJSON *pDataControl_content=cJSON_GetObjectItem(pDataControl, "content");
        std::string vice_memo_content;
        if(pDataControl_content!=NULL){
            vice_memo_content=pDataControl_content->valuestring;
        }
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        _singleton_instance->handlerVoiceMemory(voiceParam, vice_memo_content.c_str());
        _singleton_instance->setOneShotMode(true);
        _singleton_instance->setVoiceMemoryMode(false);  
    }else {
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        _singleton_instance->handlerVoiceMemory(voiceParam, NULL);
        _singleton_instance->setOneShotMode(true);
        _singleton_instance->setVoiceMemoryMode(false);
    }
}
void 
SemanticParse::tvControlVolume(const cJSON* subjson)
{
    int curVolume = SystemStatusData::getSystemStatusDataInstance()->getPlyerStatus()._vol;
    _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    char curTTS[30] = {0};
    sprintf(curTTS, "当前音量%d", curVolume);
}
void 
SemanticParse::tvControlStatusVersion(const cJSON* subjson)
{
    _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    char curTTS[128] = {0};
    sprintf(curTTS, "当前版本是:%s", SystemStatusData::getSystemStatusDataInstance()->getPackageVersion().c_str()); 
}
void 
SemanticParse::tvControlPush(const cJSON* subjson)
{
    cJSON *pRetextType = cJSON_GetObjectItem(_singleton_instance->_pJson, "retext");
    std::string sAnswer=std::string();
    if (pRetextType != NULL){
        sAnswer = pRetextType->valuestring;
    }
    _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    _singleton_instance->_parse_status_flag.needResume = 0;
    _singleton_instance->handlerTvPartner(TVMIC_TVPUSH, sAnswer);
}
void 
SemanticParse::serviceHandleTvControl(const cJSON* pData)
{
     //get tts text msg
     _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL == pData){
        return ;
    }
    cJSON *pDataControl = cJSON_GetObjectItem(pData, "control");
    if (NULL == pDataControl){
        return;
    }

    char *action;
    cJSON *pDataControlAction = cJSON_GetObjectItem(pDataControl, "action");
    if (NULL == pDataControlAction){
        return;
    }

    int param = 0xffff;
    action = pDataControlAction->valuestring;
    LOGI(LOG_TAG.c_str(),"control action:%s\n", action);
    if (action == NULL){   
        return;
    }
    //loop find the control item and handle it
    int j=0;
    for(j=0;j< sizeof(_tv_control_item)/sizeof(_tv_control_item[0]);j++){
        if (!strcmp(action,_tv_control_item[j]._name)){
            _tv_control_item[j]._handle_func(pDataControl);
            //find the contorl item and handle compelete,so break;
            break;
        }
    }
    // do not find the control item
    if(j>= sizeof(_tv_control_item)/sizeof(_tv_control_item[0]))
    {
        if (_singleton_instance->_currentService.compare("audiocontrol")!=0){
            _singleton_instance->_parse_status_flag.needResume = 0;
            _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        }

        if ((!strcmp(action, "volUp")) || 
            (!strcmp(action, "volDown")) || 
            (!strcmp(action, "statusElectricity")) || 
            (!strcmp(action, "statusWifi")))
        {
            _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        }else if (!strcmp(action, "vol")){
            if (SystemStatusData::getSystemStatusDataInstance()->getPlyerStatus()._status==PLAYER_STATUS_RUNNING){
                _singleton_instance->_parse_status_flag.needPlayTTS = 0;
            }
        }else if (!strcmp(action, "bluetoothOn")){
            _singleton_instance->_parse_status_flag.needPlayTTS = 0;
            _singleton_instance->_parse_status_flag.needResume = 0;
        }else if ((!strcmp(action, "tvPowerOn")) || 
                  (!strcmp(action, "tvPowerOff"))||
                  (!strcmp(action, "mute"))
        ){
            _singleton_instance->_parse_status_flag.needPlayTTS = 0; 
        }

        cJSON *pDataControlParam = cJSON_GetObjectItem(pDataControl, "param");

        if (NULL == pDataControlParam){
            _singleton_instance->controlBox(_singleton_instance->_currentService, action, 0xffff);
            return;
        }
        //if not hypnotic
        if (strcmp(action, "hypnotic")!=0)
        {
            param = pDataControlParam->valueint;
            if ((!strcmp(action, "powerOff")) && (param == 0))
            {
                _singleton_instance->_parse_status_flag.needResume = 0;
                _singleton_instance->_parse_status_flag.needPlayTTS = 1;
                _singleton_instance->setNeedPowerOff(true);
            }else if ((!strcmp(action, "scheduleStop")) && (param == -1)){
                _singleton_instance->_parse_status_flag.needResume = 1;
                _singleton_instance->_parse_status_flag.needPlayTTS = 0; 		
            }else if ((!strcmp(action, "scheduleStop")) && (param >= 0)){
                _singleton_instance->_parse_status_flag.needResume = 1;
                _singleton_instance->_parse_status_flag.needPlayTTS = 1;	
            }else if ((!strcmp(action, "childMode"))){
                _singleton_instance->_parse_status_flag.needResume = 0;
                _singleton_instance->_parse_status_flag.needPlayTTS = 0; 
            }else if ((!strcmp(action, "nightMode"))){
                _singleton_instance->_parse_status_flag.needResume = 1;
                _singleton_instance->_parse_status_flag.needPlayTTS = 0; 	
            }
            _singleton_instance->controlBox(_singleton_instance->_currentService,action,param);
            return;
        }
        /* hypnotic */
        char *paramHypnotic = pDataControlParam->valuestring;
        if (NULL != paramHypnotic)
        {
            cJSON *pJsonHypnotic = cJSON_Parse(paramHypnotic);
            if (pJsonHypnotic != NULL)
            {
                cJSON *pPowerOff = cJSON_GetObjectItem(pJsonHypnotic, "powerOff");
                if (NULL != pPowerOff){
                    int param=pPowerOff->valueint;
                    _singleton_instance->controlBox(_singleton_instance->_currentService, "hypnoticPowerOff", param);
                }
                cJSON *pVol = cJSON_GetObjectItem(pJsonHypnotic, "vol");
                if (pVol != NULL)
                {
                    int param=pVol->valueint;
                    _singleton_instance->_singleton_instance->controlBox(_singleton_instance->_currentService, "vol", param);
                }
            }
            return;
        }
        if (SystemStatusData::getSystemStatusDataInstance()->getPlyerStatus()._status==PLAYER_STATUS_RUNNING){
             _singleton_instance->_parse_status_flag.needPlayTTS = 1;
             _singleton_instance->_parse_status_flag.needResume = 1;
        }else{
            cJSON *pDataResult = cJSON_GetObjectItem(pData, "result");
            if (pDataResult != NULL){
                cJSON *pUICommand = NULL;
                pUICommand = cJSON_GetObjectItem(pData, "pagepara");
                _singleton_instance->_parse_status_flag.needResume = _singleton_instance->handleJHKMedia(pDataResult, pUICommand, _singleton_instance->_sAnswer, 'n', _singleton_instance->_requestSessionId);
                _singleton_instance->_parse_status_flag.needPlayTTS = 0;
            }
        }
    }
}
void 
SemanticParse::serviceHandleScheduleX(const cJSON* pData)
{
    cJSON *pSemantic = NULL;
	char *strScene = (char *)"";

     //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();

    if (NULL == pData){
        return;
    }
    //get semantic
    pSemantic = cJSON_GetObjectItem(pData, "semantic");
	cJSON * pScene = cJSON_GetObjectItem(pData, "scene");
	if (NULL != pScene){
		strScene = pScene->valuestring;
	}
    if (NULL == pSemantic){
        return;
    }

    char *strIntent = NULL, *strType = NULL, *strDateTime = NULL, *strSuggestDatetime = NULL, *strContent = (char *)"", *strRepeat = (char *)"";
    cJSON *pIntent = cJSON_GetObjectItem(pSemantic, "intent");
    if (NULL != pIntent)
    {
        strIntent = pIntent->valuestring;
        if ((strIntent != NULL) && (!strcmp(strIntent, "CANCEL"))){
            _singleton_instance->_parse_status_flag.needPlayTTS = 0;
        }
    }
    cJSON *pSlots = cJSON_GetObjectItem(pSemantic, "slots");
    if (NULL == pSlots){
        return;
    }
    int arrSize = cJSON_GetArraySize(pSlots);
    LOGI(LOG_TAG.c_str(),"schedule array size=%d\n", arrSize);
    if (arrSize <=0){
        return;
    }
    int i;
    cJSON *pName, *pValue, *pScheduleItem;
    for (i = 0; i < arrSize; i++)
    {
        LOGI(LOG_TAG.c_str(),"slots i=%d\n", i);
        pScheduleItem = cJSON_GetArrayItem(pSlots, i);
        if (NULL == pScheduleItem){
            continue;
        }
        pName = cJSON_GetObjectItem(pScheduleItem, "name");
        if (pName == NULL){
            continue;
        }
        char *curName = pName->valuestring;
        if (!strcmp(curName, "datetime"))
        {
            LOGI(LOG_TAG.c_str(),"schedule datetime\n");
            pValue = cJSON_GetObjectItem(pScheduleItem, "normValue");
            if (NULL != pValue)
            {
                LOGI(LOG_TAG.c_str(),"schedule normValue:%s\n", pValue->valuestring);
                cJSON *pNormValue = cJSON_Parse(pValue->valuestring);
                if (NULL != pNormValue)
                {
                    cJSON *pDateTime = cJSON_GetObjectItem(pNormValue, "datetime");
                    if (NULL != pDateTime)
                    {
                        strDateTime = pDateTime->valuestring;
                        LOGI(LOG_TAG.c_str(),"schedule date:%s\n", strDateTime);
                        if ((!strchr(strDateTime, ':')) && (!strchr(strDateTime, '+')))
                        {
                            strDateTime = NULL;
                            LOGI(LOG_TAG.c_str(),"schedule date NG\n");
                            break;
                        }
                        cJSON *pSuggest = cJSON_GetObjectItem(pNormValue, "suggestDatetime");
                        if (NULL != pSuggest)
                        {
                            strSuggestDatetime = pSuggest->valuestring;
                            LOGI(LOG_TAG.c_str(),"schedule suggest date----%s\n", strSuggestDatetime);
                        }
                    }else{
                        strDateTime = NULL;
                        LOGI(LOG_TAG.c_str(),"--schedule datetime NG\n");
                        break;
                    }
                }else{
                    strDateTime = NULL;
                    LOGI(LOG_TAG.c_str(),"--schedule normvalue NG\n");
                    break;
                }
            }
        }else if (!strcmp(curName, "content")){
            pValue = cJSON_GetObjectItem(pScheduleItem, "value");
            if (NULL != pValue)
            {
                strContent = pValue->valuestring;
                LOGI(LOG_TAG.c_str(),"schedule content:%s\n", strContent);
            }
        }else if (!strcmp(curName, "name")){
            pValue = cJSON_GetObjectItem(pScheduleItem, "value");
            if (NULL != pValue)
            {
                strType = pValue->valuestring;
                LOGI(LOG_TAG.c_str(),"schedule Type:%s\n", strType);
            }
        }else if (!strcmp(curName, "repeat")){
            pValue = cJSON_GetObjectItem(pScheduleItem, "value");
            if (NULL != pValue)
            {
                strRepeat = pValue->valuestring;
            }
        }
    }
    if (NULL != strIntent)
    {
        LOGI(LOG_TAG.c_str(),"schedule intent:%s\n", strIntent);
        _singleton_instance->controlSchedule(strDateTime, strSuggestDatetime, strContent, strIntent, strType, strRepeat, strScene);
    }
}
void 
SemanticParse::serviceHandleSmartHome(const cJSON* pData)
{
    //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL== pData){
        return;
    }
    char *param = cJSON_Print(pData);
    if (NULL != param)
    {
        LOGI(LOG_TAG.c_str(),"smart home:%s\n", param);
        _singleton_instance->sendCommonData(Directive_Type_SMARTHOME,param);
    }
}
void
SemanticParse::serviceHandleCustomSkills(const cJSON* pData)
{
     //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();

    if (NULL == pData){
        return;
    }
    char *param = cJSON_Print(pData);
    if (NULL != param){
        LOGI(LOG_TAG.c_str(),"custom skills:%s\n", param);
        _singleton_instance->sendCommonData(Directive_Type_SKILL, param);
    }
	_singleton_instance->_parse_status_flag.needPlayTTS = 0;
}
void 
SemanticParse::serviceHandleScene(const cJSON* pData)
{
    if (NULL != _singleton_instance->_pJson)
    {
        char *param = cJSON_Print(_singleton_instance->_pJson);
        if (NULL != param)
        {
            LOGI(LOG_TAG.c_str(),"scene json:%s\n", param);
            _singleton_instance->sendCommonData(Directive_Type_SCENE, param);
        }
    }
	_singleton_instance->_parse_status_flag.needPlayTTS = 0;
}
void 
SemanticParse::serviceHandleTvModel(const cJSON* pData)
{
    cJSON *pRetextType = cJSON_GetObjectItem(_singleton_instance->_pJson, "retext");
    if (pRetextType != NULL)
    {
        _singleton_instance->_sAnswer = pRetextType->valuestring;
    }
    _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    bool isClose = false;
    if (NULL != pData)
    {
        cJSON *pDataControl = cJSON_GetObjectItem(pData, "control");
        if (NULL != pDataControl)
        {
            cJSON *pDataControlAction = cJSON_GetObjectItem(pDataControl, "action");
            if ((NULL != pDataControlAction) && (!strcmp(pDataControlAction->valuestring, "close")))
            {
                isClose = true;
                _singleton_instance->handlerTvPartner(TVMIC_CLOSE, _singleton_instance->_sAnswer);
            }
        }
    }
    if (!isClose)
    {
        _singleton_instance->handlerTvPartner(TVMIC_OPEN, _singleton_instance->_sAnswer);	
        _singleton_instance->_parse_status_flag.needResume = 0;
    }
}
void 
SemanticParse::serviceHandleKLL3(const cJSON* pData)
{
    _singleton_instance->_parse_status_flag.needResume = 0;
    _singleton_instance->_parse_status_flag.needPlayTTS = 1;
     //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL != pData){
        return;
    }
    LOGI(LOG_TAG.c_str(),"---game data--\n");
    cJSON *pSemantic = cJSON_GetObjectItem(pData, "semantic");
    if (NULL != pSemantic)
    {
        cJSON *pIntent = cJSON_GetObjectItem(pSemantic, "intent");
        if (pIntent != NULL)
        {
            if (!strcmp("CHANGESPEED", pIntent->valuestring))
            {
                cJSON *pSlots = cJSON_GetObjectItem(pSemantic, "slots");
                if (pSlots != NULL)
                {
                    int arrSize = cJSON_GetArraySize(pSlots);
                    if (arrSize > 0)
                    {
                        int j;
                        for (j = 0; j < arrSize; j++)
                        {
                            cJSON *pSlotsItem = cJSON_GetArrayItem(pSlots, j);
                            if (pSlotsItem == NULL){
                                continue;
                            }
                        
                            cJSON *pNameItem = cJSON_GetObjectItem(pSlotsItem, "name");
                            if ((NULL != pNameItem) && (!strcmp("voiceSpeed", pNameItem->valuestring)))
                            {
                                cJSON *pValueItem = cJSON_GetObjectItem(pSlotsItem, "value");
                                if (!strcmp("double_slow", pValueItem->valuestring)){
                                    _singleton_instance->_ttsSpeed = 4;
                                }else if (!strcmp("slow", pValueItem->valuestring)){
                                    _singleton_instance->_ttsSpeed = 3;
                                }else if (!strcmp("fast", pValueItem->valuestring)){
                                    _singleton_instance->_ttsSpeed = 1;
                                }else if (!strcmp("double_fast", pValueItem->valuestring)){
                                    _singleton_instance->_ttsSpeed = 2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cJSON *pResult = cJSON_GetObjectItem(pData, "result");
    if (NULL == pResult){
        return;
    }
    LOGI(LOG_TAG.c_str(),"----get json result---\n");
    int arrSize = cJSON_GetArraySize(pResult);
    LOGI(LOG_TAG.c_str(),"new or crosstalk array size=%d\n", arrSize);
    arrSize = (arrSize > TOTAL_URL_LIST) ? TOTAL_URL_LIST : arrSize; //change
    if (arrSize <= 0){
        return;
    }
    int i, totalUrl = 0;
    cJSON *pObject;
    msg_url_t itemUrl;
    msg_url_list_t msgUrlList;

    memset(&msgUrlList, 0, sizeof(msg_url_list_t));
    msgUrlList.url_num = arrSize;
    msgUrlList.isloop = 'n';
    for (i = 0; i < arrSize; i++)
    {
        LOGI(LOG_TAG.c_str(),"i=%d\n", i);
        pObject = cJSON_GetArrayItem(pResult, i);
        if (NULL == pObject){
            continue;
        }
        cJSON *pBackgroundUrl = cJSON_GetObjectItem(pObject, "background_url");
        if (pBackgroundUrl != NULL)
        {
            int arrUrlSize = cJSON_GetArraySize(pBackgroundUrl);
            if (arrUrlSize > 0)
            {
                int j;
                for (j = 0; j < arrUrlSize; j++)
                {
                    cJSON *pBackgroundUrlItem = cJSON_GetArrayItem(pBackgroundUrl, j);
                    if (pBackgroundUrlItem == NULL){
                        continue;
                    }
                    //get the url
                    cJSON *pUrlItem = cJSON_GetObjectItem(pBackgroundUrlItem, "url");
                    if ((NULL != pUrlItem) && (NULL != pUrlItem->valuestring))
                    {
                        itemUrl.url=pUrlItem->valuestring;
                        memcpy(&(msgUrlList.url_item[totalUrl++]), &itemUrl, sizeof(msg_url_t));
                    } 
                }
            }
        }else{
            pBackgroundUrl = cJSON_GetObjectItem(pObject, "url");
            if ((pBackgroundUrl != NULL) && (NULL != pBackgroundUrl->valuestring))
            {
                memset(&itemUrl, 0, sizeof(msg_url_t));
                itemUrl.url=pBackgroundUrl->valuestring;
                memcpy(&(msgUrlList.url_item[totalUrl++]), &itemUrl, sizeof(msg_url_t));
            }
        }
    }
    msgUrlList.url_num = totalUrl;    
    _singleton_instance->playUrlList(msgUrlList);
}
void 
SemanticParse::serviceHandleAudioProgram(const cJSON* pData)
{
    //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL == pData){
        LOGI(LOG_TAG.c_str(),"---no data--\n");
        _singleton_instance->_parse_status_flag.needResume = 1;
        _singleton_instance->_parse_status_flag.needPlayTTS = 1;
        return;
    }
 
    char isloop = _singleton_instance->handlePlayMode(pData);
    cJSON *pResult = cJSON_GetObjectItem(pData, "result");
    cJSON *pUseXFurl = cJSON_GetObjectItem(pData, "useXFurl");

    if ((NULL != pUseXFurl) && (!strcmp(pUseXFurl->valuestring, "1")))
    {
        char *keyURL = (char *)"url";
        keyURL = (char *)((_singleton_instance->_currentService.compare("story")==0) ? "playUrl" : "url");
        _singleton_instance->_parse_status_flag.needResume = _singleton_instance->handleXFMedia(pResult, isloop, keyURL, _singleton_instance->_sAnswer);

    }else{
        cJSON *pUICommand = NULL;
        pUICommand = cJSON_GetObjectItem(pData, "pagepara");
        _singleton_instance->_parse_status_flag.needResume = _singleton_instance->handleJHKMedia(pResult, pUICommand, _singleton_instance->_sAnswer, isloop, _singleton_instance->_requestSessionId);
    }
    if ( _singleton_instance->_parse_status_flag.needResume != 1)
    {
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    }
}
void 
SemanticParse::serviceHandleAIUI(const cJSON* pData)
{
    //get tts text msg
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    if (NULL == pData){
        LOGI(LOG_TAG.c_str(),"---no data--\n");
        _singleton_instance->_parse_status_flag.needResume = 1;
        _singleton_instance->_parse_status_flag.needPlayTTS = 1;
        return;
    }

    cJSON *pResult = cJSON_GetObjectItem(pData, "result");
    if (_singleton_instance->_currentService.compare("news")==0)
    {
        char isloop = _singleton_instance->handlePlayMode(pData);
        _singleton_instance->_parse_status_flag.needResume = _singleton_instance->handleXFMedia(pResult, isloop, "url", _singleton_instance->_sAnswer);

    }else{
        _singleton_instance->_parse_status_flag.needResume = _singleton_instance->handleXFMedia(pResult, 'n', "url", _singleton_instance->_sAnswer);
    }

    if ( _singleton_instance->_parse_status_flag.needResume != 1){
        _singleton_instance->_parse_status_flag.needPlayTTS = 0;
    }
}
void 
SemanticParse::serviceHandleWeather(const cJSON* pData)
{
    //get weather play
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    _singleton_instance->_parse_status_flag.needPlayTTS = 1;
}
void 
SemanticParse::serviceHandleMultiChat(const cJSON* pData)
{
    //first: get semantic text content
    _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
    _singleton_instance->_parse_status_flag.needPlayTTS = 1;
    //set the multi chat mode flag
    _singleton_instance->setMultiChatMode(!_singleton_instance->getOneShotMode());
}
char 
SemanticParse::handlePlayMode(const cJSON *pJSONData)
{
    cJSON *pControl = NULL;
    if (NULL == pJSONData){
        return 88;
    }
    
    pControl = cJSON_GetObjectItem(pJSONData, "control");

    if (NULL == pControl){
        return 88;
    }
    
    cJSON *pControlAction = cJSON_GetObjectItem(pControl, "action");
    if (NULL == pControlAction){
        return 88;
    }
    char *actionPlayMode = pControlAction->valuestring;
    if (actionPlayMode == NULL){
        return 88;
    }
    
    cJSON *pControlParam = cJSON_GetObjectItem(pControl, "param");
    int paramPlayMode = 88;
    if (pControlParam != NULL)
    {
        paramPlayMode = pControlParam->valueint;
    }
    if ((!strcmp(actionPlayMode, "playmode")))
    {
        return paramPlayMode;
    }else{
        controlBox("audiocontrol", actionPlayMode, paramPlayMode);
    }
    return 88;
}
void 
SemanticParse::playUrlList(const msg_url_list_t& url_list)
{
    MsgUrlListClass* url_ptr=MsgUrlListClass::getSemanticDataSingletonPtr();
    url_ptr->setData(url_list);
    directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_URL_LIST;
    directive_data._directive_ptr=url_ptr;
    sendDirectiveQueue(directive_data);
}
void
SemanticParse::sendCommonData(directive_type_t id,const std::string& content)
{
    msg_common_t smart_home;
    smart_home.content=content;
    smart_home.session_id=_requestSessionId;
    MsgCommonClass* smart_home_ptr=MsgCommonClass::getSemanticDataSingletonPtr();
    smart_home_ptr->setData(smart_home);
    directive_queue_t directive_data;
    directive_data._directive_type=id;
    directive_data._directive_ptr=smart_home_ptr;
    sendDirectiveQueue(directive_data);
}
int 
SemanticParse::handleJHKMedia(const cJSON *pDataResult, cJSON *pUICommand, const std::string& ttsContentParam, char isloop, const std::string& sessionID)
{
    if (NULL == pDataResult){
        return 1;
    }
    int arrDataSize = cJSON_GetArraySize(pDataResult);
    LOGI(LOG_TAG.c_str(),"----get JHK result:%d---\n", arrDataSize);
    arrDataSize = (arrDataSize > TOTAL_URL_LIST) ? TOTAL_URL_LIST : arrDataSize;
    if (arrDataSize <=0){
        return 1;
    }
    int ii;
    cJSON *pJHKObject;
    msg_jhk_media_t itemJHK;
    msg_jhkmedia_list_t msgJHKList;

    memset(&msgJHKList, 0, sizeof(msg_jhkmedia_list_t));
    memset(&itemJHK, 0, sizeof(msg_jhk_media_t));
    msgJHKList.jhkmedia_num = arrDataSize;
    msgJHKList.isloop = isloop;

    for (ii = 0; ii < arrDataSize; ii++)
    {
        pJHKObject = cJSON_GetArrayItem(pDataResult, ii);
        if (NULL == pJHKObject){
            continue;
        }
        
        cJSON *pVendor = cJSON_GetObjectItem(pJHKObject, "vendor");
        if ((pVendor != NULL) && (NULL != pVendor->valuestring))
        {
            itemJHK.vendor=std::string(pVendor->valuestring);
        }
        cJSON *pExtoriorID = cJSON_GetObjectItem(pJHKObject, "exterior_id");
        if ((pExtoriorID != NULL) && (NULL != pExtoriorID->valuestring))
        {
            itemJHK.src_id=std::string(pExtoriorID->valuestring);
        }
        cJSON *pItemID = cJSON_GetObjectItem(pJHKObject, "itemid");
        if ((pItemID != NULL) && (NULL != pItemID->valuestring))
        {
            itemJHK.obj_id=std::string(pItemID->valuestring);
        }
        cJSON *pObjSubType = cJSON_GetObjectItem(pJHKObject, "obj_child_type");
        if (pObjSubType != NULL){
            itemJHK.obj_child_type = pObjSubType->valueint;
        }

        cJSON *pParentMediaID = cJSON_GetObjectItem(pJHKObject, "parent_media_id");
        if ((pParentMediaID != NULL) && (NULL != pParentMediaID->valuestring))
        {
            itemJHK.parent_media_id=std::string(pParentMediaID->valuestring);
        }
		cJSON *pParentSrcID = cJSON_GetObjectItem(pJHKObject, "parent_src_id");
        if ((pParentSrcID != NULL) && (NULL != pParentSrcID->valuestring))
        {
            itemJHK.parent_src_id=std::string(pParentSrcID->valuestring);
        }
        cJSON *pContentType = cJSON_GetObjectItem(pJHKObject, "content_type");
        if (pContentType != NULL)
        {
            itemJHK.content_type = pContentType->valueint;
        }
		cJSON *pParentPlay = cJSON_GetObjectItem(pJHKObject, "isContinuedPlay");
        if ((pParentPlay != NULL) && (NULL != pParentPlay->valuestring))
        {
            itemJHK.isContinuedPlay=std::string(pParentPlay->valuestring);
			LOGI(LOG_TAG.c_str(),"ii=%d,isContinuedPlay=%s\n", ii, itemJHK.isContinuedPlay.c_str());
        }

        memcpy(&(msgJHKList.jhkmedia_item[ii]), &itemJHK, sizeof(msg_jhk_media_t));
		memset(&itemJHK, 0, sizeof(msg_jhk_media_t));				
    }
    if (pUICommand != NULL)
    {
        cJSON *pStartUpUrl = cJSON_GetObjectItem(pUICommand, "startupUrl");
        if (NULL != pStartUpUrl)
        {
            int arrSize = cJSON_GetArraySize(pStartUpUrl);
            LOGI(LOG_TAG.c_str(),"pStartUpUrl array size=%d\n", arrSize);
            if (arrSize > 0)
            {
                int i;
                cJSON *pUrlItem, *pValue;
                for (i = 0; i < arrSize; i++)
                {
                    LOGI(LOG_TAG.c_str(),"slots i=%d\n", i);
                    pUrlItem = cJSON_GetArrayItem(pStartUpUrl, i);
                    if (NULL == pUrlItem){
                        continue;
                    }
                    cJSON *pKey = cJSON_GetObjectItem(pUrlItem, "key");
                    if (pKey == NULL){
                        continue;
                    }
                    char *keyVal = pKey->valuestring;
                    if (keyVal == NULL){
                        continue;
                    }
                    if (!strcmp(keyVal, "episodePosition"))
                    {
                        pValue = cJSON_GetObjectItem(pUrlItem, "value");
                        if (pValue != NULL)
                        {
                            msgJHKList.index = pValue->valueint;
                        }
                    }else if (!strcmp(keyVal, "voice_start_position")){
                        pValue = cJSON_GetObjectItem(pUrlItem, "value");
                        if (pValue != NULL)
                        {
                            msgJHKList.time_seconds = pValue->valueint;
                        }
                    }
                }
            }
        }
    }
    if (!ttsContentParam.empty())
    {
        msgJHKList.tts_content=ttsContentParam;
    }
    msgJHKList.jhkmedia_num = arrDataSize;
	if (!sessionID.empty())
	{
		msgJHKList.session_id=sessionID;
    }
    playJHKMediaList(msgJHKList);
    return 0;
}
void 
SemanticParse::playJHKMediaList(const msg_jhkmedia_list_t& msgJhkList)
{
    directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_JHK_MEDIA_LIST;
    MsgJHKMediaListClass* jhk_mdeia_list=MsgJHKMediaListClass::getSemanticDataSingletonPtr();
    jhk_mdeia_list->setData(msgJhkList);
    directive_data._directive_ptr=(void*)jhk_mdeia_list;
    sendDirectiveQueue(directive_data);
}
void 
SemanticParse::controlBox(const std::string& curService, const std::string& action, int val)
{
    directive_queue_t directive_data;
    if(curService.compare("audiocontrol")==0){
        directive_data._directive_type=Directive_Type_PLAYCONTROL;
    }else{
        directive_data._directive_type=Directive_Type_BOXCONTROL;
    }
    msg_control_t control_data;
    control_data.action=action;
    control_data.param=val;
    control_data.session_id=_requestSessionId;
    //new memory form heap
    MsgControlClass* control_directive_ptr=MsgControlClass::creatSemanticDataVehicle();
    control_directive_ptr->setData(control_data);
    directive_data._directive_ptr=control_directive_ptr;
    sendDirectiveQueue(directive_data);
}
void 
SemanticParse::handlerVoiceMemory(int actionParam, const std::string& backupContent)
{
	msg_voice_memory_t voiceMsg;
	memset(&voiceMsg, 0, sizeof(msg_voice_memory_t));
	voiceMsg.action = actionParam;
	if (!backupContent.empty()){
		voiceMsg.content=backupContent;
	}
	if (!_requestSessionId.empty()){
		voiceMsg.session_id=_requestSessionId;
	}
    MsgVoiceMemoryClass* memory=MsgVoiceMemoryClass::getSemanticDataSingletonPtr();
    memory->setData(voiceMsg);
    directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_VOICE_MEMORY;
    directive_data._directive_ptr=memory;
    sendDirectiveQueue(directive_data);
}
void 
SemanticParse::handlerTvPartner(int actionParam, const std::string& strTVCommand)
{
	directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_TVMIC;
	msg_tvpartner_t controlMsg;
	memset(&controlMsg, 0, sizeof(msg_tvpartner_t));

	controlMsg.action = actionParam;
	if(!strTVCommand.empty()){
	    controlMsg.content=strTVCommand;
	}
    controlMsg.session_id=_requestSessionId;
    MsgTvpartnerClass* tv_Partner=MsgTvpartnerClass::getSemanticDataSingletonPtr();
    tv_Partner->setData(controlMsg);
    directive_data._directive_ptr=tv_Partner;
    sendDirectiveQueue(directive_data);
}
void 
SemanticParse::controlSchedule(const std::string& dateTime, const std::string& suggestTime,const std::string& content, const std::string& intent, const std::string& type, const std::string& repeat, const std::string& scene)
{
	msg_schedule_t schedulemsg;
	memset(&schedulemsg, 0, sizeof(msg_schedule_t));
    
	if (!dateTime.empty()){
		schedulemsg.date_time=dateTime;
	}
	if (!suggestTime.empty()){
		schedulemsg.suggest_time=suggestTime;
	}
	if (!content.empty()){
		schedulemsg.content=content;
	}
	if (!intent.empty()){
		schedulemsg.intent=intent;
	}
	if (!type.empty()){
		schedulemsg.type=type;
	}
	if (!repeat.empty()){
		schedulemsg.repeat=repeat;
	}
    if (!_requestSessionId.empty()){
		schedulemsg.session_id=_requestSessionId;
	}
	if (!scene.empty()){
		schedulemsg.scene=scene;
	}
    MsgScheduleClass* shchedule=MsgScheduleClass::getSemanticDataSingletonPtr();
    shchedule->setData(schedulemsg);
    directive_queue_t directive_data;
    directive_data._directive_type=Directive_Type_SCHEDULE;
    directive_data._directive_ptr=shchedule;
    sendDirectiveQueue(directive_data);
}
int 
SemanticParse::handleXFMedia(const cJSON *pResult, char isloop, const std::string& keyUrl, const std::string& ttsParam)
{
    if ((NULL == pResult) || (keyUrl.empty())){
        LOGI(LOG_TAG.c_str(),"----no result---\n");
        return 1;
    }
    int arrSize = cJSON_GetArraySize(pResult);
    LOGI(LOG_TAG.c_str(),"xf media array size=%d\n", arrSize);

    arrSize = (arrSize > TOTAL_URL_LIST) ? TOTAL_URL_LIST : arrSize;
    if (arrSize <= 0){
         return 1;
    }
    
    int i;
    cJSON *pObject, *pItem;
    msg_url_t itemUrl;
    msg_url_list_t msgUrlList;
    memset(&msgUrlList, 0, sizeof(msg_url_list_t));
    msgUrlList.url_num = arrSize;
    msgUrlList.isloop = isloop;
    //jyl add fill sort to url list //2020/04/07
    msgUrlList.sort=_currentService;
    for (i = 0; i < arrSize; i++)
    {
        LOGI(LOG_TAG.c_str(),"i=%d\n", i);
        pObject = cJSON_GetArrayItem(pResult, i);
        memset(&itemUrl, 0, sizeof(msg_url_t));
        if (pObject == NULL){
            continue;
        }
        //jyl:fill offsetInMilliseconds
        pItem=cJSON_GetObjectItem(pObject, "offsetInMilliseconds");
        if(pItem != NULL){
            itemUrl.offsetInMilliseconds=pItem->valueint;
        }
        //fill source
        pItem = cJSON_GetObjectItem(pObject, "source");
        if ((pItem != NULL) && (NULL != pItem->valuestring))
        {
            itemUrl.source=pItem->valuestring;
            LOGI(LOG_TAG.c_str(),"cJSON, no:%d, source: %s\n", i, pItem->valuestring);
        }
				
        pItem = cJSON_GetObjectItem(pObject, keyUrl.c_str());
        if ((pItem != NULL) && (NULL != pItem->valuestring))
        {
            itemUrl.url=pItem->valuestring;
            LOGI(LOG_TAG.c_str(),"cJSON, no:%d, url: %s\n", i, pItem->valuestring);
        }
				
		pItem = cJSON_GetObjectItem(pObject, "mediaId");
        if ((pItem != NULL) && (NULL != pItem->valuestring))
        {
            itemUrl.mediaId=pItem->valuestring;
            LOGI(LOG_TAG.c_str(),"cJSON, no:%d, mediaId: %s\n", i, pItem->valuestring);
        }

		pItem = cJSON_GetObjectItem(pObject, "title");
        if ((pItem != NULL) && (NULL != pItem->valuestring))
        {
            itemUrl.title=pItem->valuestring;
            LOGI(LOG_TAG.c_str(),"cJSON, no:%d, title: %s\n", i, pItem->valuestring);
        }
        memcpy(&(msgUrlList.url_item[i]), &itemUrl, sizeof(msg_url_t));
    }
    if (!ttsParam.empty()){
        msgUrlList.tts_content=ttsParam;
    }
    playUrlList(msgUrlList);
    return 0;
}
std::string
SemanticParse::parseJsonStr(const std::string& JsonStr, struct ParseStatusFlags* parse_status)
{
    //clear the answer pointer
    if(!_singleton_instance->_sAnswer.empty()){
        _singleton_instance->_sAnswer.clear();
    }
    //pre set the parse status
    _singleton_instance->_parse_status_flag.needResume=1;
    _singleton_instance->_parse_status_flag.needPlayTTS=1;
    if (_singleton_instance->_pJson != NULL){
        cJSON_Delete(_pJson);
        _singleton_instance->_pJson = NULL;
    }
    if (JsonStr.empty()){
        //TODO: need add answer msg to tts play
        return std::string();
    }
    //creat json struct
    _singleton_instance->_pJson = cJSON_Parse(JsonStr.c_str());
    if (_singleton_instance->_pJson == NULL){
        LOGI(LOG_TAG.c_str(),"cJSON_Parse failed\n");
        //TODO: need add answer msg to tts play
        return std::string();
    }
    //set true,waiting cloud overrid it
    _singleton_instance->setOneShotMode(true);
    cJSON *pSessionId = cJSON_GetObjectItem(_pJson, "id");
    if (pSessionId != NULL){
        char *sessionID = pSessionId->valuestring;
        if (NULL != sessionID){
            _requestSessionId=sessionID;
        }
    }
    cJSON *pServiceType = NULL;
    pServiceType = cJSON_GetObjectItem(_pJson, "sort");
    if (pServiceType != NULL)
    {
        _singleton_instance->_currentService.clear(); //clear the pointer
        _singleton_instance->_currentService = pServiceType->valuestring;
        cJSON *pData = cJSON_GetObjectItem(_pJson, "data");
        if (NULL != pData)
        {
            cJSON *pSessionIsEnd = cJSON_GetObjectItem(pData, "oneshotmode");

            if (NULL != pSessionIsEnd)
            {
                char *oneShotVal = pSessionIsEnd->valuestring;
                if (oneShotVal != NULL)
                {
                    if (!strcmp(oneShotVal, "true")){
                        _singleton_instance->setOneShotMode(true);
                    }else{
                        _singleton_instance->setOneShotMode(false);
                        //set cloud continu mode
                        _singleton_instance->_iscontinueMode=true;
                    }
                }
            }
        }
        //loop find the service item and handle it
        int i=0;
        for(i=0; i<sizeof(_sevice_handle_table)/sizeof(_sevice_handle_table[0]); i++)
        {
            if(_singleton_instance->_currentService.compare(_sevice_handle_table[i]._name)==0){
                //in continue mode ,only part online comand can run
                _sevice_handle_table[i]._handle_func(pData);
                break;
            }
        }
        //do not find the service item
        if(i >= sizeof(_sevice_handle_table)/sizeof(_sevice_handle_table[0])){
            _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
            if(_singleton_instance->_sAnswer.empty()){
                //TODO: need add answer msg to tts play
                _singleton_instance->_sAnswer=std::string();
            }
        }
    }else{
        _singleton_instance->_sAnswer=_singleton_instance->getTtsText();
        if(_singleton_instance->_sAnswer.empty()){
            //TODO: need add answer msg to tts play
            _singleton_instance->_sAnswer=std::string();
        }
    }
    //return parser statua flag
    memcpy(parse_status,&_singleton_instance->_parse_status_flag,sizeof(_singleton_instance->_parse_status_flag));
    return _singleton_instance->_sAnswer;
}

