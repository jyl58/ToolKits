/****************************************
*   Ble Data .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.09
*****************************************/
#pragma once
#include <string>
namespace AiBox
{
    typedef struct BleStatus{
        std::string _name;
        bool _connect_status;
        uint8_t _addr[6];
    }ble_status_t;
};