/****************************************
*   ESP system .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#include  <string>
#include "AiBox_ESPSystem.hpp"
#include "esp_console.h"
#include "esp_sntp.h"
#include "esp_heap_caps.h"
#include "mbedtls/base64.h"
#include "mbedtls/aes.h"
#include "lwip/dns.h"
#include "AiBox_SystemInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="ESPSystem";
static int testMode(int argc, char **argv)
{
    ip_addr_t test_dns_mode;
    test_dns_mode.type=IPADDR_TYPE_V4;
    test_dns_mode.u_addr.ip4.addr=IPV4(10,18,207,100);
    dns_setserver(0, &test_dns_mode);
    return 0;
}
static int getDNS(int argc, char **argv)
{
    for(int i=0;i<2;i++){
        const ip_addr_t* test_dns_mode=dns_getserver(i);
        if(test_dns_mode->type!=IPADDR_TYPE_V4){
            continue;
        }
        uint8_t ip1[4];
        uint32_t tem_ip=test_dns_mode->u_addr.ip4.addr;
        for(int i=0;i<4;i++){
            ip1[i]=tem_ip&0x000000FF;
            tem_ip=tem_ip>>8;
        }
        ESP_LOGI(LOG_TAG.c_str(),"DNS server%d ip=%d.%d.%d.%d\n",i,ip1[0],ip1[1],ip1[2],ip1[3]);
    }
    return 0;
}
static int printCurrentTime(int argc, char **argv)
{
    unsigned long long time_ms=SystemInterface::getSystemInterfaceInstance()->getCurrentTimeMs();
    ESP_LOGI(LOG_TAG.c_str(),"Current time from unix epoch:%llu (ms)",time_ms);
    time_t rawtime=(time_t)(time_ms*0.001);//ms --> s
    struct tm *info;
    info = gmtime(&rawtime);
    ESP_LOGI(LOG_TAG.c_str(),"Current Data and time ：%d/%02d/%02d %02d:%02d:%02d\n",info->tm_year+1900,info->tm_mon+1,info->tm_mday,(info->tm_hour+8)%24,info->tm_min,info->tm_sec);
    return 0;
}
void registerSystemCMD()
{
    //system cmd
	esp_console_cmd_t join_cmd;
    join_cmd.command = "testmode";
    join_cmd.help = "enter test mode ";
    join_cmd.hint = NULL;
    join_cmd.func = &testMode;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));

    esp_console_cmd_t join_cmd2;
    join_cmd2.command = "dns";
    join_cmd2.help = "get DNS IP";
    join_cmd2.hint = NULL;
    join_cmd2.func = &getDNS;
    join_cmd2.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd2));

    esp_console_cmd_t join_cmd3;
    join_cmd3.command = "time";
    join_cmd3.help = "get current time,unit:ms";
    join_cmd3.hint = NULL;
    join_cmd3.func = &printCurrentTime;
    join_cmd3.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd3));
}
bool 
ESPSystemManage::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"execute the ESP system init\n");
    /*set system print level*/
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("AUDIO_ELEMENT", ESP_LOG_ERROR);
    esp_log_level_set("AUDIO_PIPELINE", ESP_LOG_ERROR);
    esp_log_level_set("spi_master", ESP_LOG_WARN);
    esp_log_level_set("ESP_AUDIO_CTRL", ESP_LOG_ERROR);
    esp_log_level_set("ESP_AUDIO_TASK", ESP_LOG_INFO);
    esp_log_level_set("HTTP_CLIENT",ESP_LOG_NONE);
    esp_log_level_set("HTTP_STREAM",ESP_LOG_NONE);
    //init the tcp ip adapter
	tcpip_adapter_init();
    //init the timer server
    SNTPInit();
    return true;
}

unsigned long long 
ESPSystemManage::getCurrentTimeUs()
{
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    unsigned long long time = current_time.tv_sec;
    time = time * 1000000 + current_time.tv_usec;
    return time;
}
int 
ESPSystemManage::generateUuid(std::string&  uuid_out)
{
    const char *c = "89ab";
    char buf[37];
    char *p = buf;
    int n = 0;
    srand((unsigned)getCurrentTimeUs() * 16);
    for (n = 0; n < 16; ++n){
        int b = rand() % 255;
        switch(n){
            case 6:
                sprintf(p, "4%x", b % 15);
                break;
            case 8:
                sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
                break;
            default:
                sprintf(p, "%02x", b);
                break;
        }
        p += 2;
        switch(n){
            case 3:
            case 5:
            case 7:
            case 9:
                *p++ = '-';
                break;
        }
    }
    *p = 0;
    uuid_out=buf;
    return uuid_out.size();
}

int 
ESPSystemManage::SNTPInit()
{
    if (sntp_enabled()){
        sntp_stop();
    }
    ESP_LOGI(LOG_TAG.c_str(), "-------Initializing SNTP--------");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp01.baidu.com");	//设置访问服务器	中国提供商
    sntp_init();
    setenv("TZ", "GMT+8", 1);
    tzset();
	return 0;
}
void 
ESPSystemManage::SNTPStop(void)
{
	ESP_LOGI(LOG_TAG.c_str(), "-------stop SNTP--------");
	return sntp_stop();
}
int 
ESPSystemManage::base64Encode(std::string& encode_str,const std::string& src)
{
    std::size_t use_len=0;
    int out_len=0;
    //get the need size
    int ret=mbedtls_base64_encode( NULL, 0, &use_len, (unsigned char*)src.data(), src.size());
    if(ret==MBEDTLS_ERR_BASE64_INVALID_CHARACTER){
        ESP_LOGI(LOG_TAG.c_str(), "base64 encode step1 invalid character!!");
        return 0;
    }
    unsigned char* d_encode_str=(unsigned  char*)calloc(1, use_len);
    if(d_encode_str==NULL){
        ESP_LOGI(LOG_TAG.c_str(), "base64 encode request mem err!!");
        return 0;
    }
    ret=mbedtls_base64_encode( d_encode_str, use_len, (size_t *)&out_len, (unsigned char*)src.data(), src.size());
    if(ret==MBEDTLS_ERR_BASE64_INVALID_CHARACTER){
        ESP_LOGI(LOG_TAG.c_str(), "base64 encode step2 invalid character!!");
        return 0;
    }
    encode_str=std::string((char*)d_encode_str,out_len);
    free(d_encode_str);
    return ret;
}
int 
ESPSystemManage::base64Decode(std::string& decode_str,const std::string& src)
{
    std::size_t use_len=0;
    int out_len=0;
    //get the need size
    int ret=mbedtls_base64_decode( NULL, 0, &use_len, (unsigned char*)src.data(), src.size());
    if(ret==MBEDTLS_ERR_BASE64_INVALID_CHARACTER){
        ESP_LOGI(LOG_TAG.c_str(), "base64 decode step1 invalid character!!");
        return 0;
    }
    unsigned char* d_decode_str=(unsigned  char*)heap_caps_malloc(use_len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if(d_decode_str==NULL){
        ESP_LOGI(LOG_TAG.c_str(), "base64 decode request mem err!!");
        return 0;
    }
    ret=mbedtls_base64_decode( d_decode_str, use_len, (size_t *)&out_len, (unsigned char*)src.data(), src.size());
    if(ret==MBEDTLS_ERR_BASE64_INVALID_CHARACTER){
        ESP_LOGI(LOG_TAG.c_str(), "base64 decode step2 invalid character!!");
        return 0;
    }
    decode_str=std::string((char*)d_decode_str,out_len);
    free(d_decode_str);
    return ret;
}
int 
ESPSystemManage::aesEncrypt(const std::string& in_str, const std::string& key, std::string& out_str)
{
    if (in_str.empty()|| key.empty()){
        out_str=std::string();
        return 0;
    }
    
    mbedtls_aes_context ctx;
    //init ctx
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, (unsigned char*)key.data(),128);

    std::string strPlain = in_str;
    int lenPlain = in_str.length();

    char chT = (16 - lenPlain % 16);
    for (int i = 0; i < (16 - lenPlain % 16); i++){
        strPlain.push_back(chT);
    }
    int len = strPlain.length();
    int en_len = 0;
    unsigned char* strInput = (unsigned char *)strPlain.data();
    unsigned char* encryptData=(unsigned  char*)heap_caps_malloc(MAX_AES_MALLOC_LEN, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);//4k
    if(encryptData==NULL){
        ESP_LOGI(LOG_TAG.c_str(), "AES Encrypt request mem err!!");
        return 0;
    }
    memset(encryptData,0,MAX_AES_MALLOC_LEN);
    while (en_len < len) 
    {
        mbedtls_aes_encrypt(&ctx, strInput, &(encryptData[en_len]));
        strInput += AES_BLOCK_SIZE;
        en_len += AES_BLOCK_SIZE;
        if(en_len>MAX_AES_MALLOC_LEN){
            ESP_LOGI(LOG_TAG.c_str(), "ERR:AES lenth is overflow!!!");
            abort();
        }
    }
    out_str=std::string((char*)encryptData,en_len);
    heap_caps_free(encryptData);
    //free the ctx 
    mbedtls_aes_free(&ctx);
    return en_len;
}
int 
ESPSystemManage::aesDecrypt(const std::string& in_str, const std::string& key, std::string& out_str)
{
    if (in_str.empty()|| key.empty()){
        out_str=std::string();
        return 0;
    }
    mbedtls_aes_context ctx;
    //init ctx
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_dec(&ctx, (unsigned char*)key.data(),128);

    int en_len = 0;
    unsigned char* strInput = (unsigned char *)in_str.data();
    unsigned char* decryptData=(unsigned char*)heap_caps_malloc(MAX_AES_MALLOC_LEN, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);//4k
    if(decryptData==NULL){
        ESP_LOGI(LOG_TAG.c_str(), "AES Decrypt request mem err!!");
        return 0;
    }
    memset(decryptData,0,MAX_AES_MALLOC_LEN);
    while (en_len < in_str.size()) 
    {
        mbedtls_aes_decrypt(&ctx, strInput, &(decryptData[en_len]));
        strInput += AES_BLOCK_SIZE;
        en_len += AES_BLOCK_SIZE;
        if(en_len>MAX_AES_MALLOC_LEN){
            ESP_LOGI(LOG_TAG.c_str(), "ERR:AES lenth is overflow!!!");
            abort();
        }
    }
    out_str=std::string((char *)decryptData);
    heap_caps_free(decryptData);
    //free the ctx 
    mbedtls_aes_free(&ctx);
    return en_len;
}