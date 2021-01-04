/****************************************
*    Flash interdface for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#pragma once
#include <string>
#include "AiBox_FlashBase.hpp"
#include "AiBox_FlashErrCode.hpp"
namespace AiBox
{
    class FlashBase;
    class FlashInterface
    {
        public:
            FlashInterface();
            FlashInterface(const FlashInterface&)=delete;
            FlashInterface& operator=(const FlashInterface&)=delete;
            ~FlashInterface();
            static FlashInterface* getFlashInterfaceInstance();
            bool init();
            bool openFlash();
            bool closeFlash();
            std::string getFlashName();
            /*read*/
            flash_err_code_t read(const std::string& key,int8_t* value);
            flash_err_code_t read(const std::string& key,uint8_t* value);
            flash_err_code_t read(const std::string& key,int16_t* value);
            flash_err_code_t read(const std::string& key,uint16_t* value);
            flash_err_code_t read(const std::string& key,int32_t* value);
            flash_err_code_t read(const std::string& key,uint32_t* value);
            flash_err_code_t read(const std::string& key,int64_t* value);
            flash_err_code_t read(const std::string& key,uint64_t* value);
            flash_err_code_t read(const std::string& key,std::string& value);
            flash_err_code_t read(const std::string& key,void* value,std::size_t length);
            /*write*/
            flash_err_code_t write(const std::string& key,int8_t value);
            flash_err_code_t write(const std::string& key,uint8_t value);
            flash_err_code_t write(const std::string& key,int16_t value);
            flash_err_code_t write(const std::string& key,uint16_t value);
            flash_err_code_t write(const std::string& key,int32_t value);
            flash_err_code_t write(const std::string& key,uint32_t value);
            flash_err_code_t write(const std::string& key,int64_t value);
            flash_err_code_t write(const std::string& key,uint64_t value);
            flash_err_code_t write(const std::string& key,const std::string& value);
            flash_err_code_t write(const std::string& key,const void* value,std::size_t length);
            /*erase*/
            flash_err_code_t erase();
            flash_err_code_t erase(const std::string& key);
            
        private:
            FlashBase* _flash;
            static FlashInterface* _singleton_instance;
    };
};