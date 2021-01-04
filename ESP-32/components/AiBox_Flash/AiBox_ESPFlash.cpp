/****************************************
*   ESP32 Flash   .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#include <cstring>
#include "AiBox_ESPFlash.hpp"
#include "esp_console.h"
using namespace AiBox;
const static std::string LOG_TAG="ESPFlash";
static int flashCMD(int argc, char **argv)
{
    if(argc<2){
        ESP_LOGI(LOG_TAG.c_str(),"Flash CMD need subcmd param!!");
        return 0;
    }
    if(!FlashInterface::getFlashInterfaceInstance()->openFlash()){
        ESP_LOGI(LOG_TAG.c_str(),"Open flash err!!");
        return 0;
    }
    std::string subcmd=argv[1];
    if(subcmd.compare("status")==0){
        nvs_stats_t nvs_stats;
        memset(&nvs_stats,0,sizeof(nvs_stats_t));
        esp_err_t err=nvs_get_stats(NVS_DEFAULT_PART_NAME, &nvs_stats);
        switch (err) {
            case ESP_OK:
                ESP_LOGI(LOG_TAG.c_str(),"total entries:%d; used entries:%d; free entries:%d",nvs_stats.total_entries,nvs_stats.used_entries,nvs_stats.free_entries);
                break;
            default:
                ESP_LOGI(LOG_TAG.c_str(),"Error:%s !\n", esp_err_to_name(err));
                break;
        }
    }else if(subcmd.compare("erase")==0){
        if(argc!=3){
            ESP_LOGI(LOG_TAG.c_str(),"Flash erase CMD need key param!!");
            return 0;
        }
        std::string key=argv[2];
        if(key.compare("all")==0){
            FlashInterface::getFlashInterfaceInstance()->erase();
        }else{
            FlashInterface::getFlashInterfaceInstance()->erase(key);
        }
    }else if(subcmd.compare("write")==0){
        if(argc!=4){
            ESP_LOGI(LOG_TAG.c_str(),"Flash write CMD need a key and value param,ex:flash write key value!!");
            return 0;
        }
        std::string key=argv[2];
        std::string value=argv[3];
        if(FlashInterface::getFlashInterfaceInstance()->write(key,value)!=FLASH_ERR_NONE){
            ESP_LOGI(LOG_TAG.c_str(),"Write flash Err");
        }
    }else if(subcmd.compare("read")==0){
        if(argc!=3){
            ESP_LOGI(LOG_TAG.c_str(),"Flash read CMD need key param!!");
            return 0;
        }
        std::string key=argv[2];
        std::string value=std::string();
        if(!FlashInterface::getFlashInterfaceInstance()->openFlash()){
            ESP_LOGI(LOG_TAG.c_str(),"Open flash err!!");
            return 0;
        }
        if(FlashInterface::getFlashInterfaceInstance()->read(key,value)!=FLASH_ERR_NONE){
            ESP_LOGI(LOG_TAG.c_str(),"Read flash Err!!");
        }else{
            ESP_LOGI(LOG_TAG.c_str(),"Read flash: %s=%s",key.c_str(),value.c_str());
        }
    }
    FlashInterface::getFlashInterfaceInstance()->closeFlash();
    return 0;
}
void registerFlashCMD()
{
    //flash cmd
	esp_console_cmd_t join_cmd;
    join_cmd.command = "flash";
    join_cmd.help = "flash cmd: flash [status|erase|write|read] (key value)";
    join_cmd.hint = NULL;
    join_cmd.func = &flashCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
bool 
ESPFlashManage::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Execute the ESP Flash init\n");
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_LOGI(LOG_TAG.c_str(),"ESP Flash init ERR!!!\n");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    if(err==ESP_OK){
        _initialized=true;
    }
    return true;
}
bool 
ESPFlashManage::open()
{
    if(!_initialized){
        ESP_LOGI(LOG_TAG.c_str(),"Flash open failed, Do not init.");
        return false;
    }
    esp_err_t err = nvs_open(_flash_name.c_str(), NVS_READWRITE, &_flash_handle);
    if (err != ESP_OK) {
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        _opened=false;
    } else {
        _opened=true;
    }
    return  _opened;
}
void 
ESPFlashManage::close()
{
    nvs_close(_flash_handle);
    _opened=false;
}
/*read*/
flash_err_code_t
ESPFlashManage::read(const std::string& key,int8_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_i8(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,uint8_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_u8(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t
ESPFlashManage::read(const std::string& key,int16_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_i16(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,uint16_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_u16(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,int32_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_i32(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t
ESPFlashManage::read(const std::string& key,uint32_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_u32(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,int64_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_i64(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,uint64_t* value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_get_u64(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,std::string& value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    //get the length
    std::size_t data_length;
    esp_err_t err = nvs_get_str(_flash_handle,key.c_str(),NULL,&data_length);
    switch (err) {
    case ESP_OK:
        break;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
    //get the data content
    char* char_value=new char[data_length+1];
    err = nvs_get_str(_flash_handle,key.c_str(),char_value,&data_length);
    switch (err) {
    case ESP_OK:
        value=std::string(char_value);
        delete[] char_value;
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        delete[] char_value;
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::read(const std::string& key,void* value,std::size_t length)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    //get the length
    std::size_t data_length=0;
    esp_err_t err = nvs_get_blob(_flash_handle,key.c_str(),NULL,&data_length);
    switch (err) {
    case ESP_OK:
        break;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
    if(data_length!=length){
        ESP_LOGI(LOG_TAG.c_str(),"Error reading: Data length is not identical!!");
        return FLASH_ERR_INVALID_LENGTH;
    }
    //get data content
    err = nvs_get_blob(_flash_handle,key.c_str(),value,&data_length);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) reading!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
/*write*/
flash_err_code_t 
ESPFlashManage::write(const std::string& key,int8_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_i8(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,uint8_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_u8(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,int16_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_i16(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,uint16_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_u16(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,int32_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_i32(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t
ESPFlashManage::write(const std::string& key,uint32_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_u32(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,int64_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_i64(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,uint64_t value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_u64(_flash_handle,key.c_str(),value);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,const std::string& value)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err = nvs_set_str(_flash_handle,key.c_str(),value.c_str());
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::write(const std::string& key,const void* value,std::size_t length)
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err=nvs_set_blob(_flash_handle,key.c_str(),value,length);
    switch (err) {
    case ESP_OK:
        nvs_commit(_flash_handle);
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) writing!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
} 
flash_err_code_t 
ESPFlashManage::erase()
{
    if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err=nvs_erase_all(_flash_handle);
    switch (err) {
    case ESP_OK:
        return FLASH_ERR_NONE;
    default :
        ESP_LOGI(LOG_TAG.c_str(),"Error (%s) erase!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}
flash_err_code_t 
ESPFlashManage::erase(const std::string& key)
{
   if(!_opened){
        return FLASH_ERR_NOT_OPENED;
    }
    esp_err_t err=nvs_erase_key(_flash_handle,key.c_str());
    switch (err) {
        case ESP_OK:
            return FLASH_ERR_NONE;
        default :
            ESP_LOGI(LOG_TAG.c_str(),"Error (%s) erase!\n", esp_err_to_name(err));
        return (flash_err_code_t)(err-ESP_ERR_NVS_BASE);
    }
}