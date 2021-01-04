
#include "cJSON.h"
#include <string.h>
#include "esp_log.h"
#include "audio_mem.h"
#include "esp_tls.h"
#include "mbedtls/md5.h"
#include "AiBox_author.hpp"
#include "esp_partition.h"
#include "LogInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="VoiceAuthor";
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

void decimal_to_hex(unsigned char *arr, size_t arr_len, char *hex_string)
{
    char *pos = hex_string;
    int count;

    for (count = 0; count < arr_len; count++) {
        sprintf(pos, "%02hhx", arr[count]);
        pos += 2;
    }
    *pos = '\0';
}
int bdscSendHttpsPostSync(const std::string& server, int port, char *cacert_pem_buf, size_t cacert_pem_bytes, char *post_data_in, size_t data_in_len, std::string& response)
{
    int ret = 0;
    esp_tls_cfg_t cfg;
    memset(&cfg,0,sizeof(esp_tls_cfg_t));
    cfg.cacert_pem_buf  = (const unsigned char*)cacert_pem_buf;
    cfg.cacert_pem_bytes = (unsigned int)cacert_pem_bytes;
    
    struct esp_tls *tls = esp_tls_conn_new(server.c_str(), server.size(), port, &cfg);
    
    if(tls != NULL) {
        ESP_LOGI(LOG_TAG.c_str(), "Connection established...");
    } else {
        ESP_LOGE(LOG_TAG.c_str(), "Connection failed...");
        return -1;
    }
    
    size_t written_bytes = 0;
    do {
        ret = esp_tls_conn_write(tls, post_data_in + written_bytes, data_in_len - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(LOG_TAG.c_str(), "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != MBEDTLS_ERR_SSL_WANT_READ  && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(LOG_TAG.c_str(), "esp_tls_conn_write  returned 0x%x", ret);
            esp_tls_conn_delete(tls);
            return -1;
        }
    } while(written_bytes < data_in_len);
    //step2: read response
    char buf[512];
    memset(buf,0,512);
    uint8_t *tmp_buf=(uint8_t*)audio_malloc(1024);
    memset(tmp_buf,0,1024);
    int cnt = 0;
    int len = 0;
    ESP_LOGI(LOG_TAG.c_str(), "Reading HTTP response...");
    do{
        len = sizeof(buf) - 1;
        ret = esp_tls_conn_read(tls, (char *)buf, len);
        
        if(ret == MBEDTLS_ERR_SSL_WANT_WRITE  || ret == MBEDTLS_ERR_SSL_WANT_READ){
            continue;
        }
        if(ret < 0){
            ESP_LOGE(LOG_TAG.c_str(), "esp_tls_conn_read  returned -0x%x", -ret);
            break;
        }
        if(ret == 0){
            ESP_LOGI(LOG_TAG.c_str(), "connection closed");
            break;
        }
        len = ret;
        ESP_LOGI(LOG_TAG.c_str(), "%d bytes read", len);
        memcpy(tmp_buf + cnt, buf, len);
        cnt += len;
    } while(1);
    response=std::string((char*)tmp_buf);
    free(tmp_buf);
    return ret;
}

char* generateAuthSigNeedfree(const char *ak, const int ts, const char *sk)
{
    char tmp[256] = {0};
    char sig[64];
    mbedtls_md5_context md5_ctx;
    unsigned char md5_cur[16];

    ESP_LOGI(LOG_TAG.c_str(),"==> generate_auth_sig_needfree\n");
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
    decimal_to_hex(md5_cur, sizeof(md5_cur), sig);
    ESP_LOGI(LOG_TAG.c_str(),"sig: %s\n", sig);
    mbedtls_md5_free(&md5_ctx);
    return audio_strdup(sig);
}
void*
VoiceAuthor::operator new(std::size_t size)throw(std::bad_alloc)
{
	return heap_caps_malloc( size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}
void 
VoiceAuthor::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
	heap_caps_free(ptr);
	ptr=nullptr;
}
VoiceAuthor::~VoiceAuthor()
{
    _baidu_asr=nullptr;
}
bool
VoiceAuthor::init(VoiceASRBaidu* baidu_asr)
{
    _baidu_asr=baidu_asr;
    //creat a author task for ,must use intenal memory
    if(xTaskCreatePinnedToCore(&VoiceAuthor::voiceAuthorTask, "voice_author", 1024*6, this, 10,NULL,1)==NULL){
        ESP_LOGI(LOG_TAG.c_str(),"Creat author task err!!!");
        return false;
    }
    return true;
}
int 
VoiceAuthor::generateAuthPam(char* pam_prama, size_t max_len)
{
    ESP_LOGI(LOG_TAG.c_str(),"==> generate_auth_pam\n");
    cJSON *pam_json=NULL;
    const char *sig=NULL;
    char *pam_string=NULL;

    if (!(pam_json = cJSON_CreateObject())) {
        return -1;
    }
 
    cJSON_AddStringToObject(pam_json, "fc", _baidu_asr->getVendorInfo().fc.c_str());
    cJSON_AddStringToObject(pam_json, "pk", _baidu_asr->getVendorInfo().pk.c_str());
    cJSON_AddStringToObject(pam_json, "ak", _baidu_asr->getVendorInfo().ak.c_str());
    unsigned long long time_s=SystemInterface::getSystemInterfaceInstance()->getCurrentTimeMs()*0.001;
    int ts = time_s / 60;
    cJSON_AddNumberToObject(pam_json, "time_stamp", ts);

    if (!(sig = generateAuthSigNeedfree(_baidu_asr->getVendorInfo().ak.c_str(), ts, _baidu_asr->getVendorInfo().sk.c_str()))) {
        ESP_LOGE(LOG_TAG.c_str(), "generate_auth_sig_needfree fail");
        cJSON_Delete(pam_json);
        return -1;
    }
    cJSON_AddStringToObject(pam_json, "signature", sig);
    free((void*)sig);

    if (!(pam_string = cJSON_PrintUnformatted(pam_json))) {
        ESP_LOGE(LOG_TAG.c_str(), "cJSON_PrintUnformatted fail");
        cJSON_Delete(pam_json);
        return -1;
    }
    cJSON_Delete(pam_json);
    if (strlen(pam_string) > max_len) {
        free((void*)pam_string);
        ESP_LOGE(LOG_TAG.c_str(), "sig too long");
        return -1;
    }

    memcpy(pam_prama, pam_string, strlen(pam_string) + 1);
    free((void*)pam_string);
    return 0;
}
void 
VoiceAuthor::voiceAuthorTask(void* param)
{
    ((VoiceAuthor*)param)->voiceAuthorExec();
    vTaskDelete(NULL);
}
void 
VoiceAuthor::voiceAuthorExec()
{
    std::string ret_data_out = std::string();
    std::string request=std::string();
    while(1){
        if(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
            ESP_LOGE(LOG_TAG.c_str(), "Author need WiFI connected");
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            continue;
        }
        if(!generateAuthBodyNeedfree(request)){
            ESP_LOGE(LOG_TAG.c_str(), "Generate author body err!!");
            continue;
        }
        ret_data_out = std::string();
        int err = bdscSendHttpsPostSync("smarthome.baidubce.com", 443, (char*)server_root_cert_pem_start, 
                                        server_root_cert_pem_end - server_root_cert_pem_start, 
                                        const_cast<char*>(request.c_str()), 
                                        request.size()+1, ret_data_out);
        request=std::string();
        //find body
        std::string::size_type pos=ret_data_out.find("\r\n\r\n",0);
        ret_data_out=ret_data_out.substr(pos);
       
        if(err){
            ESP_LOGE(LOG_TAG.c_str(), "auth failed!!");
        }else{
            ESP_LOGE(LOG_TAG.c_str(), "respon body: %s!!!",ret_data_out.c_str());
            cJSON *json=NULL, *brokerJ=NULL, *userJ=NULL, *passJ=NULL, *cidJ=NULL;
            if (!(json = cJSON_Parse(ret_data_out.c_str()))) {
                ESP_LOGE(LOG_TAG.c_str(), "http auth json format error");
            }else{
                if ((brokerJ = cJSON_GetObjectItem(json, "broker")) &&
                    (brokerJ->type == cJSON_String) &&
                    (brokerJ->valuestring[0] != '\0') &&
                    (userJ = cJSON_GetObjectItem(json, "user")) &&
                    (userJ->type == cJSON_String) &&
                    (userJ->valuestring[0] != '\0') &&
                    (passJ = cJSON_GetObjectItem(json, "pass")) &&
                    (passJ->type == cJSON_String) &&
                    (passJ->valuestring[0] != '\0') &&
                    (cidJ = cJSON_GetObjectItem(json, "clientID")) &&
                    (cidJ->type == cJSON_String) &&
                    (cidJ->valuestring[0] != '\0')
                )
                {
                    _baidu_asr->getVendorInfo().mqtt_broker=std::string(brokerJ->valuestring);
                    _baidu_asr->getVendorInfo().mqtt_username=std::string(userJ->valuestring);
                    _baidu_asr->getVendorInfo().mqtt_password=std::string(passJ->valuestring);
                    _baidu_asr->getVendorInfo().mqtt_cid=std::string(cidJ->valuestring);
                    ESP_LOGE(LOG_TAG.c_str(), "author success!!");
                    cJSON_Delete(json);
                    break;
                } else {
                    ESP_LOGE(LOG_TAG.c_str(), "http auth json format error, missing some key?");
                    cJSON_Delete(json);
                }
            }
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    //save the author msg to flash
    if(!saveAuthorMsg()){
        ESP_LOGI(LOG_TAG.c_str(), "Save author msg err!!");
        return;
    }
    //step start the asr system
    _baidu_asr->startASRSystem();
}
bool 
VoiceAuthor::generateAuthBodyNeedfree(std::string& request_content)
{
    char body[256];
    memset(body,0,sizeof(body));
    char cl[10];
    memset(cl,0,sizeof(cl));
    request_content="POST /v1/manage/mqtt HTTP/1.0\r\n";
    request_content+="Host: smarthome.baidubce.com\r\n";
    request_content+="User-Agent: esp32\r\n";
    request_content+="Content-Type: application/json\r\n";
    request_content+="Content-Length: ";
    if (generateAuthPam(body, sizeof(body))) {
        ESP_LOGE(LOG_TAG.c_str(), "generate_auth_pam fail");
        request_content=std::string();
        return false;
    }
    itoa(strlen(body), cl, 10);
    request_content+=cl;
    request_content+="\r\n\r\n";
    request_content+=body;
    ESP_LOGI(LOG_TAG.c_str(), "test request: %s", request_content.c_str());
    return true;
}
bool 
VoiceAuthor::saveAuthorMsg()
{
    esp_partition_t* partition = (esp_partition_t *)esp_partition_find_first(ESP_PARTITION_TYPE_DATA,(esp_partition_subtype_t)0x29, NULL);
    if (partition == NULL) {
        ESP_LOGE(LOG_TAG.c_str(), "Can not find profile partition");
        return false;
    }
    //step1:erase the old autor msg
    if (ESP_OK != esp_partition_erase_range(partition, 0, partition->size)) {
        ESP_LOGE(LOG_TAG.c_str(), "esp_partition_erase_range error.");
        return false;
    }
    //
    cJSON *profile_json = cJSON_CreateObject();
    if (profile_json == NULL) {
        ESP_LOGE(LOG_TAG.c_str(), "cJSON_CreateObject error");
        return false;
    }
    cJSON_AddStringToObject(profile_json, "fc", _baidu_asr->getVendorInfo().fc.c_str());
    cJSON_AddStringToObject(profile_json, "pk", _baidu_asr->getVendorInfo().pk.c_str());
    cJSON_AddStringToObject(profile_json, "ak", _baidu_asr->getVendorInfo().ak.c_str());
    cJSON_AddStringToObject(profile_json, "sk", _baidu_asr->getVendorInfo().sk.c_str());
    cJSON_AddStringToObject(profile_json, "cuid", _baidu_asr->getVendorInfo().cuid.c_str());

    cJSON_AddStringToObject(profile_json, "mqtt_broker", _baidu_asr->getVendorInfo().mqtt_broker.c_str());
    cJSON_AddStringToObject(profile_json, "mqtt_username", _baidu_asr->getVendorInfo().mqtt_username.c_str());
    cJSON_AddStringToObject(profile_json, "mqtt_password", _baidu_asr->getVendorInfo().mqtt_password.c_str());
    cJSON_AddStringToObject(profile_json, "mqtt_cid", _baidu_asr->getVendorInfo().mqtt_cid.c_str());

    std::string profile_json_str = std::string(cJSON_PrintUnformatted(profile_json));
    if (profile_json_str.empty()) {
        cJSON_Delete(profile_json);
        // restore profile err
        return false;
    }
    if (ESP_OK !=  esp_partition_write(partition, 0, profile_json_str.c_str(), profile_json_str.size() + 1)) {
        ESP_LOGE(LOG_TAG.c_str(), "esp_partition_write error");
        // restore profile err
        cJSON_Delete(profile_json);
        return false;
    }
    cJSON_Delete(profile_json);
    return true;
}
