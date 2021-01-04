/****************************************
*   Ble Base .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#pragma once
#include<string>
#include "AiBox_BleData.hpp"
#include "AiBox_SystemInterface.hpp"
namespace AiBox
{
    class BleInterface;
    class BleBase
    {
        public:
            friend  BleInterface;
            BleBase(const std::string device_name="AiBox")
            {
                _ble_status._name=device_name;
            }
            virtual ~BleBase(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            virtual bool init()=0;
            virtual bool open()=0;
            virtual bool close()=0;
        protected:
            uint16_t _app_id=0x55;
            static ble_status_t _ble_status;
            static bool _blue_initialized;
    };
};