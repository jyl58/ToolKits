/****************************************
*   Ble interface to app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#pragma once
#include<string>
#include "AiBox_BleBase.hpp"
namespace AiBox
{
    class BleInterface
    {
        public:
            BleInterface();
            BleInterface(const BleInterface&)=delete;
            BleInterface& operator=(const BleInterface&)=delete;
            static BleInterface* getBleInterfaceInstance();
            ~BleInterface();
            bool initBle();
            bool openBle();
            bool closeBle();
            bool isInitialized(){return _ble->_blue_initialized;}
            void getBleAddr(uint8_t* addr);
            void getBleStatus(ble_status_t& ble_status);
        private:
            BleBase* _ble;
            static BleInterface* _singleton_instance;

    };
};