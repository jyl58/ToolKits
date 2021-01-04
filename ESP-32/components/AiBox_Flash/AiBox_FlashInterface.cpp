/****************************************
*   Flash interdface for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.2
*****************************************/
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
    #include "AiBox_ESPFlash.hpp"
#endif
#include "AiBox_FlashInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="FlashInterface";
FlashInterface* FlashInterface::_singleton_instance=nullptr;
FlashInterface::~FlashInterface()
{
    if(_flash!=nullptr){
        delete _flash;
        _flash=nullptr;
    }
}
FlashInterface* FlashInterface::getFlashInterfaceInstance()
{
    if(_singleton_instance==nullptr)
    {
        _singleton_instance=new FlashInterface();
    }
    return _singleton_instance;
}
FlashInterface::FlashInterface()
{
   _singleton_instance=this; 
}
bool 
FlashInterface::init()
{
#ifdef ESP32_PLATFORM
    _flash=new ESPFlashManage();
#endif
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat Flash manage err!");
        return false;
    }
    if(!_flash->init()){
        LOGI(LOG_TAG.c_str(),"Flash manage init err!");
        return false;
    }
    return true;
}
bool 
FlashInterface::openFlash()
{
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return false;
    }
    return _flash->open();
}
bool 
FlashInterface::closeFlash()
{
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return false;
    }
    _flash->close();
    return true;
}
std::string
FlashInterface::getFlashName()
{
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return std::string();
    }
    return _flash->_flash_name;
}
 /*read*/
flash_err_code_t 
FlashInterface::read(const std::string& key,int8_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,uint8_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,int16_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,uint16_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,int32_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,uint32_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,int64_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,uint64_t* value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t 
FlashInterface::read(const std::string& key,std::string& value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value);
}
flash_err_code_t
FlashInterface::read(const std::string& key,void* value,std::size_t length)
{
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->read(key,value,length);
}
/*write*/
flash_err_code_t
FlashInterface::write(const std::string& key,int8_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,uint8_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,int16_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,uint16_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,int32_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,uint32_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,int64_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,uint64_t value){
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
         return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
    
}
flash_err_code_t
FlashInterface::write(const std::string& key,const std::string& value){
     if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value);
}
flash_err_code_t
FlashInterface::write(const std::string& key,const void* value,std::size_t length)
 {
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->write(key,value,length);
 }
 flash_err_code_t 
 FlashInterface::erase()
 {
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->erase();
 }
flash_err_code_t 
FlashInterface::erase(const std::string& key)
{
    if(_flash==nullptr){
        LOGI(LOG_TAG.c_str(),"Flash pointer is nullptr!");
        return FLASH_ERR_NOT_INITIALIZED;
    }
    return _flash->erase(key);
}