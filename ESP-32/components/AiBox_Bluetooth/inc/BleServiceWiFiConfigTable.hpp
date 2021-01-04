/****************************************
*   ESP Ble WiFi service profile.hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.04
*****************************************/
#pragma once
#include <string>
#include "BleServiceBase.hpp"
namespace AiBox
{
    class BleWiFiConfigServiceProfileTable:public BleServiceProfileBase
    {
        public:
            BleWiFiConfigServiceProfileTable(uint16_t app_id,const std::string device_name);
            ~BleWiFiConfigServiceProfileTable(){};
            void BleGattEventCallback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)override;
            enum
            {
                IDX_SVC,
                IDX_CHAR_WIFI,
                IDX_CHAR_VAL_WIFI,

                HRS_IDX_NB,
            };
            uint16_t _service_handle_table[HRS_IDX_NB];
            esp_gatts_attr_db_t _gatt_db[HRS_IDX_NB];
            uint8_t wifi_Config_value[128]={1,5,7,9,0};

            const uint16_t _GATTS_SERVICE_UUID_WIFI = 0x00FF;  //service uuid
            const uint16_t _GATTS_CHAR_UUID_CONFIG  = 0xFF01;  //char uuid
    };
};