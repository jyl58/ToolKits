/****************************************
*   Ble ESP .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.03
*****************************************/
#include <cstring>
#include "esp_log.h"
#include "AiBox_BleESP.hpp"
#include "BleServiceWiFiConfigTable.hpp"
#include "esp_console.h"
#include "esp_bt_device.h"
#include "AiBox_BleInterface.hpp"
#include "AiBox_BoardESP.hpp"
#include "a2dp_stream.h"
using namespace AiBox;
static const std::string LOG_TAG = "BleESP";
/*ble cmd*/
static int bleOpenCMD(int argc, char **argv)
{
    int ret=0;
    bool blue_initialized=BleInterface::getBleInterfaceInstance()->isInitialized();
    if(blue_initialized){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Bluetooth is already opened", __func__);
        return 0;
    }
    if(!BleInterface::getBleInterfaceInstance()->openBle()){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Open Bluetooth failed", __func__);
        ret=1;
    }
    return ret;
}
static int bleCloseCMD(int argc, char **argv)
{
    int ret=0;
    bool blue_initialized=BleInterface::getBleInterfaceInstance()->isInitialized();
    if(!blue_initialized){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Bluetooth already closed", __func__);
        return 0;
    }
    if(!BleInterface::getBleInterfaceInstance()->closeBle()){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Close Bluetooth failed", __func__);
        ret=1;
    }
    return ret;
}
static int bleInfoCMD(int argc, char **argv)
{
    bool blue_initialized=BleInterface::getBleInterfaceInstance()->isInitialized();
    if(!blue_initialized){
        ESP_LOGE(LOG_TAG.c_str(), "Bluetooth is not open");
        return 0;
    }
    ble_status_t ble_status;
    BleInterface::getBleInterfaceInstance()->getBleStatus(ble_status);
    ESP_LOGI(LOG_TAG.c_str(),"name:%s  address: %X:%X:%X:%X:%X:%X ",ble_status._name.c_str(),
																	ble_status._addr[0],ble_status._addr[1],ble_status._addr[2],
																	ble_status._addr[3],ble_status._addr[4],ble_status._addr[5]);
    return 0;
}

/*esp ble */
uint16_t ESPBleManage::_service_count=0;

uint8_t ESPBleManage::_service_uuid[16] = {
    // LSB <--------------------------------------------------------------------------------> MSB 
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

uint8_t ESPBleManage::_blue_name[32];
/* The length of adv data must be less than 31 bytes */
esp_ble_adv_data_t ESPBleManage::_adv_data; 
// scan response data
esp_ble_adv_data_t ESPBleManage::_scan_rsp_data;
esp_ble_adv_params_t ESPBleManage::_adv_params;
BleServiceProfileBase* ESPBleManage::_profile_tab[MAX_PROFILE_NUM]={nullptr,nullptr,nullptr};

esp_periph_handle_t ESPBleManage::_bt_periph=nullptr;
ESPBleManage::ESPBleManage(const std::string device_name)
:AiBox::BleBase(device_name)
{
    memset(&_adv_params,0,sizeof(esp_ble_adv_params_t));
    _adv_params.adv_int_min=0x20;
    _adv_params.adv_int_max=0x40;
    _adv_params.adv_type=ADV_TYPE_IND;
    _adv_params.own_addr_type=BLE_ADDR_TYPE_PUBLIC;
    _adv_params.channel_map=ADV_CHNL_ALL;
    _adv_params.adv_filter_policy=ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

    //copy blue name
     memcpy(_blue_name,_ble_status._name.c_str(),_ble_status._name.size()+1);
    // The length of adv data must be less than 31 bytes 
    _adv_data.set_scan_rsp        = false;
    _adv_data.include_name        = true;
    _adv_data.include_txpower     = true;
    _adv_data.min_interval        = 0x0006; //slave connection min interval, Time = min_interval * 1.25 msec
    _adv_data.max_interval        = 0x0010; //slave connection max interval, Time = max_interval * 1.25 msec
    _adv_data.appearance          = 0x00;
    _adv_data.manufacturer_len    = _ble_status._name.size()+1; 
    
    _adv_data.p_manufacturer_data = _blue_name;
    _adv_data.service_data_len    = 0;
    _adv_data.p_service_data      = NULL;
    _adv_data.service_uuid_len    = sizeof(_service_uuid);
    _adv_data.p_service_uuid      = _service_uuid;
    _adv_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

    // scan response data
    _scan_rsp_data.set_scan_rsp        = true;
    _scan_rsp_data.include_name        = true;
    _scan_rsp_data.include_txpower     = true;
    _scan_rsp_data.min_interval        = 0x0006;
    _scan_rsp_data.max_interval        = 0x0010;
    _scan_rsp_data.appearance          = 0x00;
    _scan_rsp_data.manufacturer_len    = _ble_status._name.size()+1; 
    _scan_rsp_data.p_manufacturer_data = _blue_name;
    _scan_rsp_data.service_data_len    = 0;
    _scan_rsp_data.p_service_data      = NULL;
    _scan_rsp_data.service_uuid_len    = sizeof(_service_uuid);
    _scan_rsp_data.p_service_uuid      = _service_uuid;
    _scan_rsp_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

}
void 
ESPBleManage::gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    bool need_send_ble_status=false;
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&_adv_params);
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&_adv_params);
            break;
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&_adv_params);
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&_adv_params);
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(LOG_TAG.c_str(), "advertising start failed");
            }else{
                ESP_LOGI(LOG_TAG.c_str(), "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(LOG_TAG.c_str(), "Advertising stop failed");
            }else {
                ESP_LOGI(LOG_TAG.c_str(), "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                                                                                                                                                param->update_conn_params.status,
                                                                                                                                                param->update_conn_params.min_int,
                                                                                                                                                param->update_conn_params.max_int,
                                                                                                                                                param->update_conn_params.conn_int,
                                                                                                                                                param->update_conn_params.latency,
                                                                                                                                                param->update_conn_params.timeout);
            _ble_status._connect_status=true;
            //need_send_ble_status=true;
            break;
        default:
            break;
    
    }
    //check if need send ble status
    if(need_send_ble_status){
        //send ble realtime status to system maintain
        auto ble_system_queue_handle=SystemInterface::getSystemInterfaceInstance()->getSystemBlueStatusHandle();
        if(ble_system_queue_handle!=nullptr){
            xQueueSend(ble_system_queue_handle,&_ble_status,0);
        }else{
            ESP_LOGI(LOG_TAG.c_str(), "System Ble queue handle is empty\n");
        }
    }
}

void 
ESPBleManage::gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            BleServiceProfileBase *tmp_ptr=getBleServiceByAppID(param->reg.app_id);
            if(tmp_ptr==nullptr){
                ESP_LOGE(LOG_TAG.c_str(), "the id: %d service is nullptr ",param->reg.app_id);
                return ;
            }
            tmp_ptr->_gatts_if = gatts_if;
        } else {
            ESP_LOGE(LOG_TAG.c_str(), "reg app failed, app_id %04x, status %d",param->reg.app_id,param->reg.status);
            return;
        }
    }
    BleServiceProfileBase* tmep_callback_ptr=getBleServiceByGatts_if(gatts_if);
    if(tmep_callback_ptr==nullptr){
        ESP_LOGE(LOG_TAG.c_str(), "the gatts_if: %d is nullptr",gatts_if);
        return;
    }
    tmep_callback_ptr->BleGattEventCallback(event, gatts_if, param);
}

ESPBleManage::~ESPBleManage()
{
}
bool 
ESPBleManage::init()
{
    ESP_LOGI(LOG_TAG.c_str(), "Exec init the blutooth");
    registerWiFiCMD();
    return open();
}

void 
ESPBleManage::registeBleService(BleServiceProfileBase *service_profile)
{
    if(_service_count<MAX_PROFILE_NUM){
        _profile_tab[_service_count]=service_profile;
        _service_count++;
    }
}
BleServiceProfileBase*
ESPBleManage::getBleServiceByAppID(uint16_t app_id)
{
    for(int i=0;i< MAX_PROFILE_NUM;i++){
        if((_profile_tab!=nullptr)&&(_profile_tab[i]->_app_id==app_id)){
            return _profile_tab[i];
        }
    }
    return nullptr;
}
BleServiceProfileBase*
ESPBleManage::getBleServiceByGatts_if(esp_gatt_if_t gatts_if)
{
    for(int i=0;i< MAX_PROFILE_NUM;i++){
        if((_profile_tab!=nullptr)&&(_profile_tab[i]->_gatts_if==gatts_if)){
            return _profile_tab[i];
        }
    }
    return nullptr;
}

bool 
ESPBleManage::open()
{
    esp_bt_controller_config_t bt_cfg ;
    memset(&bt_cfg,0,sizeof(esp_bt_controller_config_t));
    bt_cfg= BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(LOG_TAG.c_str(), "<%s> enable controller failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    //enable the dual mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(LOG_TAG.c_str(), "<%s> enable BLE mode failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(LOG_TAG.c_str(), "<%s> init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(LOG_TAG.c_str(), "<%s> enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    //set the scan mode
    ret=esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

    _bt_periph= bt_create_periph();
    if(ESPBoard::_periph_set==nullptr){
        ESP_LOGE(LOG_TAG.c_str(), " board periph is nullptr");
        return false;
    }
    esp_periph_start(ESPBoard::_periph_set, _bt_periph);
    ESP_LOGI(LOG_TAG.c_str(), "Start Bluetooth peripherals");

    ret = esp_ble_gatts_register_callback(ESPBleManage::gattsEventHandler);
    if (ret){
        ESP_LOGE(LOG_TAG.c_str(), "gatts register error, error code = %x", ret);
        return false;
    }
    ret =esp_ble_gap_register_callback(ESPBleManage::gapEventHandler);
    if (ret){
        ESP_LOGE(LOG_TAG.c_str(), "gap register error, error code = %x", ret);
        return false;
    }
    registeBleService(new BleWiFiConfigServiceProfileTable(_app_id,_ble_status._name));
    ret = esp_ble_gatts_app_register(_app_id);
    if (ret){
        ESP_LOGE(LOG_TAG.c_str(), "gatts app register error, error code = %x", ret);
        return false;
    }
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(LOG_TAG.c_str(), "set local  MTU failed, error code = %x", local_mtu_ret);
    }
    memcpy(_ble_status._addr,esp_bt_dev_get_address(),6);
    _blue_initialized=true;
    return true;
}
bool 
ESPBleManage::close()
{
    esp_err_t ret=esp_bluedroid_disable();
    if(ret){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Disable bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    ret=esp_bluedroid_deinit();
    if(ret){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Deinit bluedroid failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    ret=esp_bt_controller_disable();
    if(ret){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Disable BT controller failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    ret=esp_bt_controller_deinit();
    if(ret){
        ESP_LOGE(LOG_TAG.c_str(), "<%s> Deinit BT controller failed: %s", __func__, esp_err_to_name(ret));
        return false;
    }
    _blue_initialized=false;
    return true;
}
void 
ESPBleManage::registerWiFiCMD()
{
    esp_console_cmd_t join_cmd;
    //cmd open
    join_cmd.command = "btopen";
    join_cmd.help = "open the device bluetooth";
    join_cmd.hint = NULL;
    join_cmd.func = &bleOpenCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));

    //cmd close
    join_cmd.command = "btclose";
    join_cmd.help = "close the device bluetooth";
    join_cmd.hint = NULL;
    join_cmd.func = &bleCloseCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));

    //cmd info
    join_cmd.command = "btinfo";
    join_cmd.help = "print the device bluetooth info";
    join_cmd.hint = NULL;
    join_cmd.func = &bleInfoCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
