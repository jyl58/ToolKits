/****************************************
*   Ble ESP .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#pragma once
#include <string>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gap_bt_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "AiBox_BleBase.hpp"
#include "BleServiceBase.hpp"
#include "esp_peripherals.h"
#define MAX_PROFILE_NUM        3
#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

namespace AiBox
{
  class BleServiceProfileBase;
  class ESPBleManage:public AiBox::BleBase
    {
        public:
            ESPBleManage(const std::string device_name="AiBox");
            ~ESPBleManage();
            bool init()override;
            bool open()override;
            bool close()override;
            static void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
            static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
            static void registeBleService(BleServiceProfileBase *service_profile);
            static BleServiceProfileBase *getBleServiceByAppID(uint16_t app_id);
            static BleServiceProfileBase *getBleServiceByGatts_if(esp_gatt_if_t gatts_if);
            static uint8_t _raw_adv_data[];
            static uint8_t _raw_scan_rsp_data[];
            static uint8_t _service_uuid[16];
            /* The length of adv data must be less than 31 bytes */
            static esp_ble_adv_data_t _adv_data; 
            static uint8_t _blue_name[32];
            // scan response data
            static esp_ble_adv_data_t _scan_rsp_data;
            static esp_ble_adv_params_t _adv_params;
            static uint8_t adv_config_done;
            static BleServiceProfileBase *_profile_tab[MAX_PROFILE_NUM];
            static esp_periph_handle_t _bt_periph;
        private:
            void registerWiFiCMD();
            static uint16_t _service_count;
    };
};
