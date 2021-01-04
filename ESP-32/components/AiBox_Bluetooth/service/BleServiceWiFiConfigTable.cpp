/****************************************
*   ESP Ble WiFi service profile .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.04
*****************************************/
#include <cstring>
#include "esp_log.h"
#include "BleServiceWiFiConfigTable.hpp"
#include "AiBox_BleESP.hpp"
using namespace AiBox;
#define SVC_INST_ID            0
static const std::string LOG_TAG = "BleWiFiConfig";

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read_write   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;

BleWiFiConfigServiceProfileTable::BleWiFiConfigServiceProfileTable(uint16_t app_id,const std::string device_name)
:BleServiceProfileBase(app_id,device_name)
{
    for(int i=0; i<HRS_IDX_NB;i++){
      memset(&_gatt_db[i],0,sizeof(esp_gatts_attr_db_t));
    }
    // Service Declaration
    _gatt_db[IDX_SVC]={{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,sizeof(uint16_t), sizeof(_GATTS_SERVICE_UUID_WIFI), (uint8_t *)&_GATTS_SERVICE_UUID_WIFI}};

    // Characteristic Declaration 
    _gatt_db[IDX_CHAR_WIFI] ={{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_write}};

    // Characteristic Value 
    _gatt_db[IDX_CHAR_VAL_WIFI]= {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&_GATTS_CHAR_UUID_CONFIG, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,128, 5, (uint8_t *)wifi_Config_value}};
    
}
void 
BleWiFiConfigServiceProfileTable::BleGattEventCallback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(_device_name.c_str());
            if (set_dev_name_ret){
                ESP_LOGE(LOG_TAG.c_str(), "set device name failed, error code = %x", set_dev_name_ret);
            }
            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&ESPBleManage::_adv_data);
            if (ret){
                ESP_LOGE(LOG_TAG.c_str(), "config adv data failed, error code = %x", ret);
            }
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&ESPBleManage::_scan_rsp_data);
            if (ret){
                ESP_LOGE(LOG_TAG.c_str(), "config scan response data failed, error code = %x", ret);
            }

            esp_err_t create_attr_ret =esp_ble_gatts_create_attr_tab(_gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(LOG_TAG.c_str(), "create attr table failed, error code = %x", create_attr_ret);
            }
        }
       	break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_READ_EVT");
       	break;
    case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_WRITE_EVT");
            memcpy(wifi_Config_value,param->write.value,param->write.len);
      	    break;
    case ESP_GATTS_EXEC_WRITE_EVT: 
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX. 
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_EXEC_WRITE_EVT");
            break;
    case ESP_GATTS_MTU_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
    case ESP_GATTS_CONF_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
    case ESP_GATTS_START_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
    case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_ble_conn_update_params_t conn_params;
            memset(&conn_params,0,sizeof(esp_ble_conn_update_params_t));
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            //For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. 
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
    case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            if(param->disconnect.reason!=ESP_GATT_CONN_TERMINATE_LOCAL_HOST){
                //if close the blut tooth,so do not advertising 
                esp_ble_gap_start_advertising(&ESPBleManage::_adv_params);
            }
            break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(LOG_TAG.c_str(), "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(LOG_TAG.c_str(), "create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }else {
                ESP_LOGI(LOG_TAG.c_str(), "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(_service_handle_table, param->add_attr_tab.handles, sizeof(_service_handle_table));
                esp_ble_gatts_start_service(_service_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
    default:
        break;
    }
}