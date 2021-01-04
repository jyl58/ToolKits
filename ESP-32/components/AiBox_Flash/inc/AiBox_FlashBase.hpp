/****************************************
*   Flash base  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#pragma once
#include <string>
#include "AiBox_FlashInterface.hpp"
#include "AiBox_FlashErrCode.hpp"
namespace AiBox
{
    class FlashInterface;
    class FlashBase
    {
        public:
            friend FlashInterface;
            FlashBase(){}
            virtual ~FlashBase(){}
            virtual bool init()=0;
            virtual bool open()=0;
            virtual void close()=0;
            /*read*/
            virtual flash_err_code_t read(const std::string& key,int8_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,uint8_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,int16_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,uint16_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,int32_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,uint32_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,int64_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,uint64_t* value)=0;
            virtual flash_err_code_t read(const std::string& key,std::string& value)=0;
            virtual flash_err_code_t read(const std::string& key,void* value,std::size_t length)=0;
            /*write */
            virtual flash_err_code_t write(const std::string& key,int8_t value)=0;
            virtual flash_err_code_t write(const std::string& key,uint8_t value)=0;
            virtual flash_err_code_t write(const std::string& key,int16_t value)=0;
            virtual flash_err_code_t write(const std::string& key,uint16_t value)=0;
            virtual flash_err_code_t write(const std::string& key,int32_t value)=0;
            virtual flash_err_code_t write(const std::string& key,uint32_t value)=0;
            virtual flash_err_code_t write(const std::string& key,int64_t value)=0;
            virtual flash_err_code_t write(const std::string& key,uint64_t value)=0;
            virtual flash_err_code_t write(const std::string& key,const std::string& value)=0;
            virtual flash_err_code_t write(const std::string& key,const void* value,std::size_t length)=0;
            /*erase*/
            virtual flash_err_code_t erase()=0;
            virtual flash_err_code_t erase(const std::string& key)=0;
            
        protected:
            bool _initialized{false};
            bool _opened{false};
            std::string _flash_name{"AiBoxFlash"};

    };
};
