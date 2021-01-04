/****************************************
*   system base  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#pragma once
#include <string>
#include "AiBox_ProcessASR.hpp"
#define  AES_BLOCK_SIZE 16
#define IPV4(a,b,c,d) ((a<<0)|(b<<8)|(c<<16)|(d<<24))
namespace AiBox
{
    class SystemBase
    {
        public:
            SystemBase(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            virtual ~SystemBase(){}
            virtual bool init()=0;
            virtual unsigned long long getCurrentTimeUs()=0;
            virtual int generateUuid(std::string&  uuid_out)=0;
            virtual int base64Encode(std::string& encode_str,const std::string& src)=0;
            virtual int base64Decode(std::string& decode_str,const std::string& src)=0;
            virtual int aesEncrypt(const std::string& in_str, const std::string& key, std::string& out_str)=0;
            virtual int aesDecrypt(const std::string& in_str, const std::string& key, std::string& out_str)=0;
            static const std::string SEMANTIC_AES_KEY;
            static const std::string SEMANTIC_AES_KEY_SMARTHOME;

            static const std::string SEMANTIC_KEY_TV ;
            static const std::string SEMANTIC_KEY_SMARTHOME ;

            static const std::string SEMANTIC_AES_KEY_MUSIC;
            static const std::string SEMANTIC_KEY_MUSIC ;

            static const std::string SEMANTIC_AES_KEY_FAULT ;
            static const std::string SEMANTIC_KEY_FAULT ;

#ifdef ENABLE_CUSTOMID_UPLOAD
            static const std::string SEMANTIC_AES_KEY_CUSTOMID;
            static const std::string SEMANTIC_KEY_CUSTOMID;
#endif          
        protected:
    };
};