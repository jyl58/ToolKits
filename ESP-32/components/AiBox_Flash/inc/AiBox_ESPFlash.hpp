/****************************************
*   ESP32 Flash  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#include <string>
#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "AiBox_FlashBase.hpp"
extern "C" void registerFlashCMD();
namespace AiBox
{
    class ESPFlashManage: public AiBox::FlashBase
    {
        public:
            ESPFlashManage(): AiBox::FlashBase(){}
            ~ESPFlashManage(){};
            bool init()override;
            bool open()override;
            void close()override;
            /*read*/
            flash_err_code_t read(const std::string& key,int8_t* value)override;
            flash_err_code_t read(const std::string& key,uint8_t* value)override;
            flash_err_code_t read(const std::string& key,int16_t* value)override;
            flash_err_code_t read(const std::string& key,uint16_t* value)override;
            flash_err_code_t read(const std::string& key,int32_t* value)override;
            flash_err_code_t read(const std::string& key,uint32_t* value)override;
            flash_err_code_t read(const std::string& key,int64_t* value)override;
            flash_err_code_t read(const std::string& key,uint64_t* value)override;
            flash_err_code_t read(const std::string& key,std::string& value)override;
            flash_err_code_t read(const std::string& key,void* value,std::size_t length)override;
            /*write*/
            flash_err_code_t write(const std::string& key,int8_t value)override;
            flash_err_code_t write(const std::string& key,uint8_t value)override;
            flash_err_code_t write(const std::string& key,int16_t value)override;
            flash_err_code_t write(const std::string& key,uint16_t value)override;
            flash_err_code_t write(const std::string& key,int32_t value)override;
            flash_err_code_t write(const std::string& key,uint32_t value)override;
            flash_err_code_t write(const std::string& key,int64_t value)override;
            flash_err_code_t write(const std::string& key,uint64_t value)override;
            flash_err_code_t write(const std::string& key,const std::string& value)override;
            flash_err_code_t write(const std::string& key,const void* value,std::size_t length)override;
            /*erase*/
            virtual flash_err_code_t erase()override;
            virtual flash_err_code_t erase(const std::string& key)override;
            
        private:
            nvs_handle _flash_handle;
            
    };
};