/****************************************
*   voice ASR baidu .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#include <sys/time.h>
#include "AiBox_VoiceASRBaidu.hpp"
#include "esp_log.h"
#include "audio_error.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "cJSON.h"
#include "mbedtls/md5.h"
#include "AiBox_PlayManage.hpp"
using namespace AiBox;
const static std::string LOG_TAG="VoiceASRBaidu";
bds_client_context_t VoiceASRBaidu::_bds_context_client;
bds_client_handle_t VoiceASRBaidu::_g_client_handle;
QueueHandle_t VoiceASRBaidu::_g_voice_event_queue;
VoiceASRBaidu*  VoiceASRBaidu::_singleton_instance=nullptr;
const voice_handle_table_t VoiceASRBaidu::_voice_handle_table[]={
    {EVENT_WAKEUP_TRIGGER,&VoiceASRBaidu::handleWakeup},
    {EVENT_ASR_END,&VoiceASRBaidu::handleASREnd},
    {EVENT_ASR_TTS_DATA,&VoiceASRBaidu::handleTTSData},
    {EVENT_ASR_RESULT,&VoiceASRBaidu::handleASRResult},
    {EVENT_LINK_ERROR,&VoiceASRBaidu::handleLinkERR}
};
VoiceASRBaidu::VoiceASRBaidu()
:VoiceASRBase()
{
    _singleton_instance=this;
    memset(&_bds_context_client,0,sizeof(bds_client_context_t));
    memset(&_tts_info,0,sizeof(TTS_HEADER));
}

bool 
VoiceASRBaidu::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Exec the Baidu Voice ASR init");
    
    //step1:check the profile fs ps ak sk
    if(!confirmProfile()){
        ESP_LOGI(LOG_TAG.c_str(),"Confirm profile fail!!");
        return false;
    }
    //step2: need author?
    if( _g_vendor_info.mqtt_broker.empty()||
        _g_vendor_info.mqtt_username.empty()||
        _g_vendor_info.mqtt_password.empty()||
        _g_vendor_info.mqtt_cid.empty()
      )
    {
        //creat a author tassk
        ESP_LOGI(LOG_TAG.c_str(), "Device not author,need author first");
        _author=new VoiceAuthor();
        return _author->init(this);
    }else{
        startASRSystem();
    }
    return true;
}
bool 
VoiceASRBaidu::confirmProfile()
{
    esp_partition_t *partition= (esp_partition_t *)esp_partition_find_first(ESP_PARTITION_TYPE_DATA,(esp_partition_subtype_t)0x29, NULL);
    if (partition == NULL) {
        ESP_LOGW(LOG_TAG.c_str(), "Can not find profile partition");
        return false;
    }
    ESP_LOGI(LOG_TAG.c_str(), "%d: type[0x%x]", __LINE__, partition->type);
    ESP_LOGI(LOG_TAG.c_str(), "%d: subtype[0x%x]", __LINE__, partition->subtype);
    ESP_LOGI(LOG_TAG.c_str(), "%d: address:0x%x", __LINE__, partition->address);
    ESP_LOGI(LOG_TAG.c_str(), "%d: size:0x%x", __LINE__, partition->size);
    ESP_LOGI(LOG_TAG.c_str(), "%d: label:%s", __LINE__,  partition->label);
    char *buf =new char[partition->size];
    //read the profile data
    if (esp_partition_read(partition, 0, buf, partition->size) != 0) {
        ESP_LOGW(LOG_TAG.c_str(), "Read profile failed and use default profile.");
        return false;
    }
    //step1:
    cJSON* json = cJSON_Parse(buf);
    assert(json != NULL);
    cJSON* fc = cJSON_GetObjectItem(json, "fc");
    assert(fc != NULL);
    cJSON* pk = cJSON_GetObjectItem(json, "pk");
    assert(pk != NULL);
    cJSON* ak = cJSON_GetObjectItem(json, "ak");
    assert(ak != NULL);
    cJSON* sk = cJSON_GetObjectItem(json, "sk");
    assert(sk != NULL);
    cJSON* cuid = cJSON_GetObjectItem(json, "cuid");
    assert(cuid != NULL);
    //read the profile 
    _g_vendor_info.fc=fc->valuestring;
    _g_vendor_info.pk=pk->valuestring;
    _g_vendor_info.ak=ak->valuestring;
    _g_vendor_info.sk=sk->valuestring;
    _g_vendor_info.cuid=cuid->valuestring;

    ESP_LOGI(LOG_TAG.c_str(), "fc = %s", _g_vendor_info.fc.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "pk = %s", _g_vendor_info.pk.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "ak = %s", _g_vendor_info.ak.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "sk = %s", _g_vendor_info.sk.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "cuid = %s", _g_vendor_info.cuid.c_str());
    //step2: 
    // bellow is optional
    cJSON* mqtt_broker   = cJSON_GetObjectItem(json, "mqtt_broker");
    if(mqtt_broker!=NULL&&mqtt_broker->type==cJSON_String){
        _g_vendor_info.mqtt_broker=std::string(mqtt_broker->valuestring);
    }else{
        _g_vendor_info.mqtt_broker=std::string();
    }
    cJSON* mqtt_username = cJSON_GetObjectItem(json, "mqtt_username");
    if(mqtt_username!=NULL&&mqtt_username->type==cJSON_String){
        _g_vendor_info.mqtt_username=std::string(mqtt_username->valuestring);
    }else{
        _g_vendor_info.mqtt_username=std::string();
    }
    cJSON* mqtt_password = cJSON_GetObjectItem(json, "mqtt_password");
    if(mqtt_password!=NULL&&mqtt_password->type==cJSON_String){
        _g_vendor_info.mqtt_password=std::string(mqtt_password->valuestring);
    }else{
        _g_vendor_info.mqtt_password=std::string();
    }
    cJSON* mqtt_cid      = cJSON_GetObjectItem(json, "mqtt_cid");
    if(mqtt_cid!=NULL&&mqtt_cid->type==cJSON_String){
        _g_vendor_info.mqtt_cid=std::string(mqtt_cid->valuestring);
    }else{
        _g_vendor_info.mqtt_cid=std::string();
    }
    ESP_LOGI(LOG_TAG.c_str(), "mqtt_broker= %s", _g_vendor_info.mqtt_broker.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "mqtt_username= %s", _g_vendor_info.mqtt_username.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "mqtt_password= %s", _g_vendor_info.mqtt_password.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "mqtt_cid= %s", _g_vendor_info.mqtt_cid.c_str());

    cJSON_Delete(json);
    delete buf ;
    return true;
}
bool 
VoiceASRBaidu::startASRSystem()
{
    _g_client_handle = bds_client_create(&_bds_context_client);
    bds_client_set_event_listener(_g_client_handle, VoiceASRBaidu::bdscEventCallback, NULL);
    startVoiceEngine();
    //start the voice event task
    _g_voice_event_queue = xQueueCreate(EVENT_ENGINE_QUEUE_LEN, sizeof(baidu_voice_event_t));
    int ret = xTaskCreate(&VoiceASRBaidu::VoiceEventHandleTask,"voice_event_task", 1024 * 4, this, 15, NULL);
    if (ret != pdPASS) {
        ESP_LOGI(LOG_TAG.c_str(),"start engine fail!!");
        return false;
    }
    ESP_LOGI(LOG_TAG.c_str(), "Start link");
    //start the link
    bdscLinkStart();
    //start wake up
    bdscStartWakeup();
    return true;
}
void 
VoiceASRBaidu::startVoiceEngine()
{
    std::string uuid;
    SystemInterface::getSystemInterfaceInstance()->generateUuid(uuid);

    ESP_LOGI(LOG_TAG.c_str(), "UUID=%s",uuid.c_str());
    std::string pam_data_string = std::string();
    generateDcsPamNeedFree(pam_data_string);
    if(pam_data_string.empty()){
        ESP_LOGI(LOG_TAG.c_str(), "Generate doc param err!!!");
        return;
    }
    ESP_LOGI(LOG_TAG.c_str(), "generate_dcs_pam=%s",pam_data_string.c_str());
    bdsc_engine_params_t *engine_params = bdsc_engine_params_create(const_cast<char*>(uuid.c_str()), 1146, 
                                                                    "leetest.baidu.com", 443, PROTOCOL_TLS,
                                                                    const_cast<char*>(_g_vendor_info.cuid.c_str()), 
                                                                    "duer_app",pam_data_string.size()+1, 
                                                                    const_cast<char*>(pam_data_string.c_str()));
    
    bds_client_params_t params;
    memset(&params, 0 , sizeof(bds_client_params_t));
    params.engine_params = engine_params;
    bds_client_config(_g_client_handle, &params);
    bdsc_engine_params_destroy(engine_params);
    ESP_LOGI(LOG_TAG.c_str(), "Start Voice sdk.");
    bds_client_start(_g_client_handle);
    bdscConfigNqe(ASR_FORCE_FEATURE);
}
void 
VoiceASRBaidu::bdscConfigNqe(ASR_FORCE_MODE mode)
{
    cJSON *config_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(config_json, KEY_NQE_MODE, mode);
    char *config_str = cJSON_PrintUnformatted(config_json);
    cJSON_Delete(config_json);
    bdscDynamicConfig(config_str);
    free(config_str);
}
void 
VoiceASRBaidu::bdscDynamicConfig(char *config)
{
    if (_g_client_handle == NULL || config == NULL) {
        return;
    }
    bds_client_command_t dynamic_config;
    memset(&dynamic_config,0,sizeof(bds_client_command_t));
    dynamic_config.key = CMD_DYNAMIC_CONFIG;
    dynamic_config.content = config;
    dynamic_config.content_length = strlen(config) + 1;

    bds_client_send(_g_client_handle, &dynamic_config);
}
void 
VoiceASRBaidu::stopVoiceSDK()
{
    bds_client_stop(_g_client_handle);
}
void 
VoiceASRBaidu::destoryVoiceSDK()
{
    bds_client_destroy(_g_client_handle);
}

void
VoiceASRBaidu::generateDcsPamNeedFree(std::string& doc_param) 
{
    cJSON *pam_json = cJSON_CreateObject();
    assert(pam_json != NULL);
    if (pam_json == NULL) {
        ESP_LOGE(LOG_TAG.c_str(), "pam_json memory error");
        doc_param=std::string();
        return ;
    }

    // 这块依旧沿用度秘的tcp链路，暂时不改
    cJSON_AddStringToObject(pam_json, "dueros-device-id", "637b7b81fe4142e687d4ef6510e8066d");
    cJSON_AddStringToObject(pam_json, "StandbyDeviceId", "");
    cJSON_AddStringToObject(pam_json, "user-agent", "test");
    cJSON_AddStringToObject(pam_json, "Authorization", "Bearer 24.608939b04dc0e559a8575a0126e3bc43.2592000.1591947805.282335-16030937");
    cJSON_AddStringToObject(pam_json, "from", "dumi");
    cJSON_AddNumberToObject(pam_json, "LinkVersion", 2);

    doc_param = cJSON_PrintUnformatted(pam_json);
    if (NULL != pam_json) {
        cJSON_Delete(pam_json);
        pam_json = NULL;
    }
    return ;
}
int32_t 
VoiceASRBaidu::bdscEventCallback(bds_client_event_t *event, void *custom)
{
    ESP_LOGI(LOG_TAG.c_str(), "bdscEventCallback get envent %d",event->key);
    if (event == NULL) {
        ESP_LOGI(LOG_TAG.c_str(), "!!!Empty event !!!");
        return 0;
    }
    switch (event->key) {
        case EVENT_ASR_ERROR:
        {
            eventEngineElemEnQueue(EVENT_ASR_ERROR, (uint8_t*)event->content, sizeof(bdsc_event_error_t));
            break;
        }
        case EVENT_ASR_CANCEL:
        {
            eventEngineElemEnQueue(EVENT_ASR_CANCEL, (uint8_t*)event->content, sizeof(bdsc_event_process_t));
            break;
        }
        case EVENT_ASR_BEGIN:
        {
            eventEngineElemEnQueue(EVENT_ASR_BEGIN, (uint8_t*)event->content, sizeof(bdsc_event_process_t));
            break;
        }
        case EVENT_ASR_RESULT:
        {
            bdsc_event_data_t * asr_result = (bdsc_event_data_t*)event->content;
            eventEngineElemEnQueue(EVENT_ASR_RESULT, (uint8_t*)event->content, sizeof(bdsc_event_data_t) + asr_result->buffer_length);
            break;
        }
        case EVENT_ASR_EXTERN_DATA:
        {
            bdsc_event_data_t * extern_data = (bdsc_event_data_t*)event->content;
            eventEngineElemEnQueue(EVENT_ASR_EXTERN_DATA, (uint8_t*)event->content, sizeof(bdsc_event_data_t) + extern_data->buffer_length);
            break;
        }
        case EVENT_ASR_TTS_DATA:
        {
            bdsc_event_data_t * tts_data = (bdsc_event_data_t*)event->content;
            eventEngineElemEnQueue(EVENT_ASR_TTS_DATA, (uint8_t*)event->content, sizeof(bdsc_event_data_t) + tts_data->buffer_length);
            break;
        }
        case EVENT_ASR_END:
        {
            eventEngineElemEnQueue(EVENT_ASR_END, (uint8_t*)event->content, sizeof(bdsc_event_process_t));
            break;
        }
        case EVENT_WAKEUP_TRIGGER:
        {
            eventEngineElemEnQueue(EVENT_WAKEUP_TRIGGER, (uint8_t*)event->content, sizeof(bdsc_event_wakeup_t));
            break;
        }
        case EVENT_WAKEUP_ERROR:
        {
            eventEngineElemEnQueue(EVENT_WAKEUP_ERROR, (uint8_t*)event->content, sizeof(bdsc_event_error_t));
            break;
        }
        case EVENT_LINK_CONNECTED:
        {
            eventEngineElemEnQueue(EVENT_LINK_CONNECTED, (uint8_t*)event->content, sizeof(bdsc_event_data_t));
            break;
        }
        case EVENT_LINK_DISCONNECTED:
        {
            eventEngineElemEnQueue(EVENT_LINK_DISCONNECTED, (uint8_t*)event->content, sizeof(bdsc_event_data_t));
            break;
        }
        case EVENT_LINK_ERROR:
        {
            eventEngineElemEnQueue(EVENT_LINK_ERROR, (uint8_t*)event->content, sizeof(bdsc_event_error_t));
            break;
        }
        case EVENT_RECORDER_DATA:
        {
            bdsc_event_data_t *pcm_data = (bdsc_event_data_t*)event->content;
            eventEngineElemEnQueue(EVENT_RECORDER_DATA, (uint8_t*)event->content, sizeof(bdsc_event_data_t) + pcm_data->buffer_length);
            break;
        }
        case EVENT_RECORDER_ERROR:
        {
            eventEngineElemEnQueue(EVENT_RECORDER_ERROR, (uint8_t*)event->content, sizeof(bdsc_event_error_t));
            break;
        }
        default:
            ESP_LOGE(LOG_TAG.c_str(), "!!! unknow event id=%d!!!",event->key);
            break;
        }
    return 0;
}
void 
VoiceASRBaidu::eventEngineElemEnQueue(int event, uint8_t *buffer, size_t len)
{
    baidu_voice_event_t elem;
    elem.event_key = event;
    elem.data_len = len;
    elem.data =(uint8_t*)sysMemMalloc(len);
    memcpy(elem.data, buffer, len);

    // EVENT_WAKEUP_TRIGGER high priotiry, FlushQueque! include the tts data
    if (EVENT_WAKEUP_TRIGGER == event) {
       eventEngineElemFlushQueue();
    }
    //send the event msg to queue
    xQueueSend(_g_voice_event_queue, (void *)&elem, 0);
}
void 
VoiceASRBaidu::eventEngineElemFlushQueue()
{
    int recv_cnt;
    ESP_LOGE(LOG_TAG.c_str(),"==> event_engine_elem_FlushQueque\n");
    if (uxQueueSpacesAvailable(_g_voice_event_queue) < EVENT_ENGINE_QUEUE_LEN) {

        recv_cnt =  EVENT_ENGINE_QUEUE_LEN - uxQueueSpacesAvailable(_g_voice_event_queue);
        baidu_voice_event_t elem;
        memset(&elem, 0, sizeof(baidu_voice_event_t));
        int i;
        ESP_LOGE(LOG_TAG.c_str(),"FLUSH QUEUE! recvd: %d\n", recv_cnt);

        for (i = 0; i < recv_cnt; i++) {
            if (xQueueReceive((_g_voice_event_queue), &elem, 0) == pdPASS) {
                sysMemFree(elem.data);
            }
        }
    } else {
        ESP_LOGI(LOG_TAG.c_str(),"_g_voice_event_queue content is empty\n");
    }
}
void
VoiceASRBaidu::VoiceEventHandleTask(void* param)
{
    ESP_LOGE(LOG_TAG.c_str(),"start run the voice handle task.\n");
    ((VoiceASRBaidu*)param)->exec();
    vTaskDelete(NULL);
}
void 
VoiceASRBaidu::exec()
{
    //delete the author ptr
    if(_author!=nullptr){
        delete _author;
        _author=nullptr;
    }
    baidu_voice_event_t voice_event_msg;
    while (1) {
        if (EVENT_ENGINE_QUEUE_LEN == uxQueueSpacesAvailable(_g_voice_event_queue)){
            vTaskDelay(20 / portTICK_PERIOD_MS);
            continue;
        }
        int i=0;
        if (xQueueReceive(_g_voice_event_queue, &voice_event_msg, 0) == pdPASS){
            for(i=0;i<sizeof(_voice_handle_table)/sizeof(_voice_handle_table[0]);i++){
                if(voice_event_msg.event_key==_voice_handle_table[i]._event){
                    _voice_handle_table[i]._func(voice_event_msg);
                    break;
                }
            }
            if(i>=sizeof(_voice_handle_table)/sizeof(_voice_handle_table[0])){
                //add err msg
            }
            //delete the ptr
            sysMemFree(voice_event_msg.data);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
/* asr cmd */
void 
VoiceASRBaidu::bdscLinkStart()
{
    bds_client_command_t link_start;
    memset(&link_start,0,sizeof(bds_client_command_t));
    link_start.key = CMD_LINK_START;
    link_start.content = NULL;
    link_start.content_length =0 ;
    bds_client_send(_g_client_handle, &link_start);
}
void 
VoiceASRBaidu::bdscLinkStop()
{
    bds_client_command_t link_stop;
    link_stop.key = CMD_LINK_STOP;
    link_stop.content = NULL;
    link_stop.content_length = 0;

    bds_client_send(_g_client_handle, &link_stop);
}
void 
VoiceASRBaidu::bdscStartWakeup()
{
    bds_client_command_t wakeup_start;
    memset(&wakeup_start,0,sizeof(bds_client_command_t));
    wakeup_start.key = CMD_WAKEUP_START;
    bds_client_send(_g_client_handle, &wakeup_start);
}
void 
VoiceASRBaidu::bdscStopWakeup()
{
    bds_client_command_t wakeup_stop;
    memset(&wakeup_stop,0,sizeof(bds_client_command_t));
    wakeup_stop.key = CMD_WAKEUP_STOP;
    bds_client_send(_g_client_handle, &wakeup_stop);
}

#define     BDSC_MAX_UUID_LEN   37
void 
VoiceASRBaidu::bdscStartASR(int back_time)
{
    char *sn;
    std::string pam_data;
    int param_max_len = 4096;

    sn = (char*)heap_caps_malloc(BDSC_MAX_UUID_LEN, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    AUDIO_MEM_CHECK("start_asr", sn, return);
    bds_generate_uuid(sn);
    generateASRThirdPartyPam(pam_data, param_max_len);
    bdsc_asr_params_t *asr_params = bdsc_asr_params_create( sn, 1601, 1601, "com.baidu.iot2", 16000, 
                                                            const_cast<char*>(_g_vendor_info.cuid.c_str()), back_time,
                                                            pam_data.size()+1, const_cast<char*>(pam_data.c_str()));
    //asr command format
    bds_client_command_t asr_start;
    asr_start.key = CMD_ASR_START;
    asr_start.content = asr_params;
    asr_start.content_length = sizeof(bdsc_asr_params_t) + pam_data.size() + 1;
    bds_client_send(_g_client_handle, &asr_start);
    bdsc_asr_params_destroy(asr_params);
    free(sn);
}
int 
VoiceASRBaidu::generateASRThirdPartyPam(std::string& pam_prama, size_t max_len)
{
    printf("==> generate_asr_thirdparty_pam\n");
    const char *methods[3] = {"ASR", "UNIT", "TTS"};
    cJSON *pam_json=cJSON_CreateObject();
    if (pam_json==NULL) {
        return -1;
    }

    cJSON_AddStringToObject(pam_json, "fc", _g_vendor_info.fc.c_str());
    cJSON_AddStringToObject(pam_json, "pk", _g_vendor_info.pk.c_str());
    cJSON_AddStringToObject(pam_json, "ak", _g_vendor_info.ak.c_str());
    cJSON_AddNumberToObject(pam_json, "aue", 3); // 3：mp3， default is 0 (pcm)
    cJSON_AddNumberToObject(pam_json, "rate", 4); // bitrate, 4: for mp3
    
    unsigned long long time_s=SystemInterface::getSystemInterfaceInstance()->getCurrentTimeMs()*0.001;
    int ts = time_s / 60;
    char ts_str[12];
    itoa(ts,ts_str,10);
    cJSON_AddStringToObject(pam_json, "time_stamp", ts_str);

    cJSON *methodJ = cJSON_CreateStringArray(methods, 3);
    cJSON_AddItemToObject(pam_json, "methods", methodJ);

    std::string sig=generateAuthSigNeedfree(_g_vendor_info.ak.c_str(), ts, _g_vendor_info.sk.c_str());
    if (sig.empty()){
        ESP_LOGE(LOG_TAG.c_str(), "generate_auth_sig_needfree fail");
        cJSON_Delete(pam_json);
        return -1;
    }
    cJSON_AddStringToObject(pam_json, "signature", sig.c_str());

    std::string pam_string=cJSON_PrintUnformatted(pam_json);
    if (pam_string.empty()){
        ESP_LOGE(LOG_TAG.c_str(), "cJSON_PrintUnformatted fail");
        cJSON_Delete(pam_json);
        return -1;
    }
    cJSON_Delete(pam_json);
    if (pam_string.size() > max_len){
        ESP_LOGE(LOG_TAG.c_str(), "sig too long");
        return -1;
    }
    pam_prama=pam_string;
    printf("pam_string: %s\n", pam_string.c_str());
    return 0;
}

std::string 
VoiceASRBaidu::generateAuthSigNeedfree(const char *ak, const int ts, const char *sk)
{
    char tmp[256] = {0};
    char sig[64];
    mbedtls_md5_context md5_ctx;
    unsigned char md5_cur[16];

    printf("==> generate_auth_sig_needfree\n");
    if (!ak || !sk) {
        return NULL;
    }

    sprintf(tmp, "%s&%d%s", ak, ts, sk);
    mbedtls_md5_init(&md5_ctx);

    if (mbedtls_md5_starts_ret(&md5_ctx)) {
        ESP_LOGE(LOG_TAG.c_str(), "mbedtls_md5_starts_ret() error");
        mbedtls_md5_free(&md5_ctx);
        return NULL;
    }
    if (mbedtls_md5_update_ret(&md5_ctx, (const unsigned char *)tmp, strlen(tmp))) {
        ESP_LOGE(LOG_TAG.c_str(), "mbedtls_md5_update_ret() failed");
        mbedtls_md5_free(&md5_ctx);
        return NULL;
    }
    if (mbedtls_md5_finish_ret(&md5_ctx, md5_cur)) {
        ESP_LOGE(LOG_TAG.c_str(), "mbedtls_md5_finish_ret() error");
        mbedtls_md5_free(&md5_ctx);
        return NULL;
    }
    decimalToHex(md5_cur, sizeof(md5_cur), sig);
    mbedtls_md5_free(&md5_ctx);

    return std::string(sig);
}
void 
VoiceASRBaidu::decimalToHex(unsigned char *arr, size_t arr_len, char *hex_string)
{
    char *pos = hex_string;
    int count;

    for (count = 0; count < arr_len; count++) {
        sprintf(pos, "%02hhx", arr[count]);
        pos += 2;
    }
    *pos = '\0';
}

/*handle*/
#define DEFAULT_WAKEUP_BACKTIME 200
void 
VoiceASRBaidu::handleWakeup(const baidu_voice_event_t& wakeup_data)
{
    ESP_LOGE(LOG_TAG.c_str(),"get wakeup event");
    AiBox::media_node_t media_node;
	media_node._media_info._media_type=AiBox::MEDIA_TYPE_TONE_FLASH;
	media_node._media_info._uri="wakeup";
	media_node._media_info._start_pos=0;
    media_node._not_allow_seek=true;
	media_node._media_info._start_time=0;
	media_node._func=[](const AiBox::player_status_show_t& status){
        ESP_LOGI(LOG_TAG.c_str(), "Start Asr.");
        _singleton_instance->bdscStartASR(DEFAULT_WAKEUP_BACKTIME);
    };
	media_node._business_type=Business_Type_WAKEUP;
	MediaVehicle::getMediaVehicleInstance()->setMediaData(media_node);
	media_vehicle_data_t media_vehicle;
	media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
	auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
	if(media_queue_handle!=nullptr){
	    xQueueSend(media_queue_handle,&media_vehicle,0);
	}else{
	    ESP_LOGI(LOG_TAG.c_str(), "Media handle is nullptr.");
	}
    
}
void 
VoiceASRBaidu::handleASREnd(const baidu_voice_event_t& asr_end_data)
{
    ESP_LOGE(LOG_TAG.c_str(),"get ASR END event");
    memset(&_singleton_instance->_tts_info,0,sizeof(TTS_HEADER));
}
void
VoiceASRBaidu::handleTTSData(const baidu_voice_event_t& tts)
{
    bdsc_event_data_t *tts_data = (bdsc_event_data_t*)tts.data;
    ESP_LOGW(LOG_TAG.c_str(), "---> EVENT_ASR_TTS_DATA sn=%s, idx=%d, buffer_length=%d, buffer=%p",
                            tts_data->sn, tts_data->idx,tts_data->buffer_length, tts_data->buffer);
    char* payload_data = (char*)tts_data->buffer;
    uint16_t payload_length=tts_data->buffer_length;
    if(tts_data->idx==1||tts_data->idx==-1){
        //first tts stream ,need parase the tts head
        if (_singleton_instance->_tts_info.data_left != 0){
            ESP_LOGW(LOG_TAG.c_str(), "last package may miss data.");
            _singleton_instance->_tts_info.data_left = 0;
        }
        if (tts_data->buffer) {
            _singleton_instance->readStream((char*)tts_data->buffer);
        }
        payload_data = _singleton_instance->getTTSHeader((char*)tts_data->buffer, &payload_length);
        if (payload_data == NULL) {
            ESP_LOGE(LOG_TAG.c_str(), "tts header error, stream truncked.");
            return ;
        }
        //send to play
        if(!_singleton_instance->_tts_info.playing){
            ESP_LOGI(LOG_TAG.c_str(), "Send tts play node to play manage.");
            _singleton_instance->_tts_info.playing=true;
            PlayerInterface::getPlayerInterfaceInstance()->clearRawStreamQueue();
            media_node_t tts_media_node;
            tts_media_node._media_info._media_type=MEDIA_TYPE_MUSIC_RAW;
            tts_media_node._media_info._uri="tts";
            tts_media_node._media_info._start_pos=0;
            tts_media_node._media_info._start_time=0;
            tts_media_node._not_allow_seek=true;
            tts_media_node._func=nullptr;
            tts_media_node._business_type=Business_Type_NONE;
            MediaVehicle::getMediaVehicleInstance()->setMediaData(tts_media_node);
            media_vehicle_data_t media_vehicle;
            media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
            auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
            if(media_queue_handle!=nullptr){
                xQueueSend(media_queue_handle,&media_vehicle,0);
            }else{
                ESP_LOGI(LOG_TAG.c_str(), "Media handle is nullptr.");
            }
        }
    }else{
        _singleton_instance->_tts_info.data_left-= payload_length;
    }
    //wirte the tts data to raw ring buffer
	PlayerInterface::getPlayerInterfaceInstance()->writeRawMediaData(payload_data, payload_length);
    return ;
}

void 
VoiceASRBaidu::readStream(char* buf) {
    if (buf) {
        short header_len = covertToShort(buf);
        buf += (6 + header_len);
    }
}
char* 
VoiceASRBaidu::getTTSHeader(char* buffer, uint16_t* length) 
{
    char* origin_buffer = buffer;
    int origin_length = *length;
    short header_len = covertToShort(buffer);
    buffer += 2;
    cJSON* json = cJSON_Parse(buffer);
    char* content = cJSON_Print(json);
    printf("tts header json: %s\n", content);
    free(content);
    cJSON* err = cJSON_GetObjectItem(json, "err");
    cJSON* idx = cJSON_GetObjectItem(json, "idx");

    _tts_info.err = err->valueint;
    if (_tts_info.err != 0) {
        ESP_LOGE(LOG_TAG.c_str(), "tts header shows error, will drop it.");
        cJSON_Delete(json);
        return NULL;
    }
    _tts_info.idx = idx->valueint;

    cJSON_Delete(json);
    buffer += header_len;
    _tts_info.data_len = covertToInt(buffer);
    buffer += 4;
    int consumed_bytes = buffer - origin_buffer;
    *length = origin_length - consumed_bytes;
    _tts_info.data_left = _tts_info.data_len - *length;

    return buffer;
}
short
VoiceASRBaidu::covertToShort(char* buffer) 
{
    short len = buffer[1] << 8;
    len |=  buffer[0];
    return len;
}
int 
VoiceASRBaidu::covertToInt(char* buffer) {
    int len = buffer[3] << 24;
    len |= buffer[2] << 16;
    len |= buffer[1] << 8;
    len |=  buffer[0];
    return len;
}
void
VoiceASRBaidu::handleASRResult(const baidu_voice_event_t& asr_result)
{
    bdsc_event_data_t* asr_data=(bdsc_event_data_t*)asr_result.data;
    ESP_LOGW(LOG_TAG.c_str(), "---> EVENT_ASR_RESULT sn=%s, idx=%d, buffer_length=%d, buffer=%s",
                                asr_data->sn, asr_data->idx, asr_data->buffer_length, asr_data->buffer);
    cJSON* asr_pJson=NULL;
    asr_pJson=cJSON_Parse((char*)asr_data->buffer);
    if(asr_pJson==NULL){
        ESP_LOGE(LOG_TAG.c_str(),"Parse ASR json buffer err!!");
        return;
    }
    cJSON* asr_err_json=cJSON_GetObjectItem(asr_pJson,"err_no");
    if(asr_err_json==NULL){
        ESP_LOGE(LOG_TAG.c_str(),"ASR Json err code is NULL!!");
        cJSON_Delete(asr_pJson);
        return; 
    }
    if(asr_err_json->valueint>0){
        ESP_LOGE(LOG_TAG.c_str(),"ASR err, errcode=%d!!",asr_err_json->valueint);
        cJSON_Delete(asr_pJson);
        return; 
    }
    cJSON* asr_result_json= cJSON_GetObjectItem(asr_pJson,"result");
    if(asr_result_json==NULL){
        cJSON_Delete(asr_pJson);
        return;
    }
    cJSON* asr_word_json=cJSON_GetObjectItem(asr_result_json,"word");
    if(asr_word_json==NULL){
        cJSON_Delete(asr_pJson);
        return;
    } 
    std::string asr_content=std::string(cJSON_GetStringValue(cJSON_GetArrayItem(asr_word_json,0)));
    ESP_LOGI(LOG_TAG.c_str(),"ASR content=%s",asr_content.c_str());
    cJSON_Delete(asr_pJson);
    return;
}
void
VoiceASRBaidu::handleLinkERR(const baidu_voice_event_t& link_err)
{
    bdsc_event_error_t *error = (bdsc_event_error_t*)link_err.data;
    if (error) {
        ESP_LOGW(LOG_TAG.c_str(), "---> EVENT_LINK_ERROR code=%d; --info=%s", error->code, error->info);
    } else {
        ESP_LOGE(LOG_TAG.c_str(), "---> EVENT_LINK_ERROR error null");
    }
}