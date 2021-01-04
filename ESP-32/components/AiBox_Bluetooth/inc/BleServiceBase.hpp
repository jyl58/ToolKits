/****************************************
*   ESP Ble service base .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.04
*****************************************/
#pragma once
#include <string>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "AiBox_BleESP.hpp"
#include "AiBox_SystemInterface.hpp"
namespace AiBox
{
    class BleServiceProfileBase{
        public:
            BleServiceProfileBase(uint16_t app_id, const std::string device_name)
            :_device_name(device_name),
            _app_id(app_id)
            {}
            ~BleServiceProfileBase(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            virtual void BleGattEventCallback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)=0;
            std::string _device_name;
            uint16_t _gatts_if=ESP_GATT_IF_NONE;
            uint16_t _app_id;
            uint16_t _conn_id;
            uint16_t _service_handle;
            esp_gatt_srvc_id_t _service_id;
            uint16_t _char_handle;
            esp_bt_uuid_t _char_uuid;
            esp_gatt_perm_t _perm;
            esp_gatt_char_prop_t _property;
            uint16_t _descr_handle;
            esp_bt_uuid_t _descr_uuid;
    };
};