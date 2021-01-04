/****************************************
*   ESP Board  for app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#include <cstring>
#include "esp_peripherals.h"
#include "input_key_service.h"
#include "periph_adc_button.h"
#include "input_key_com_user_id.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "AiBox_BoardESP.hpp"
#include "AiBox_BoardInterface.hpp"
using namespace AiBox;
const static std::string LOG_TAG="ESPBoard";
audio_board_handle_t ESPBoard::_board_handle=nullptr;
esp_periph_set_handle_t ESPBoard::_periph_set=nullptr;
esp_err_t 
inputKeyServiceCallBack(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
    bool need_send=false;
    button_data_t button_data;
    memset(&button_data,0,sizeof(button_data_t));
    ESP_LOGI(LOG_TAG.c_str(), "type=%d, source=%d, data=%d, len=%d", evt->type, (int)evt->source, (int)evt->data, evt->len);
    switch ((int)evt->data) {
        case INPUT_KEY_USER_ID_MUTE:
            button_data._button_id=BUTTON_ID_MUTE;
            need_send=true;
            break;
        case INPUT_KEY_USER_ID_SET:
            button_data._button_id=BUTTON_ID_PLAY; //board imply set key,use to play
            need_send=true;
            break;
        case INPUT_KEY_USER_ID_VOLDOWN:
            button_data._button_id=BUTTON_ID_VOLDOWN;
            need_send=true;
            break;
        case INPUT_KEY_USER_ID_VOLUP:
            button_data._button_id=BUTTON_ID_VOLUP;
            need_send=true;
            break;
        case INPUT_KEY_USER_ID_PLAY:
            break;
        default:
            break;
    }
    if(need_send){
        button_data._type=(button_type_t)evt->type;
        const QueueHandle_t& button_queue_handle=BoardInterface::getBoardInterfaceInstance()->getButtonQueueHandle();
        if(button_queue_handle!=nullptr){
            xQueueSend(button_queue_handle,&button_data,0);
        }else{
            ESP_LOGI(LOG_TAG.c_str(), "button queue is nullptr!!!");
        }
    }
    return ESP_OK;
}
bool 
ESPBoard::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Exec Board init.");
    //step1. init the peripherals set
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    _periph_set = esp_periph_set_init(&periph_cfg);
    // step2. init the input key service
    audio_board_key_init(_periph_set);
    
    input_key_service_info_t input_info[] = INPUT_KEY_DEFAULT_INFO();
    /*input_key_service_cfg_t key_serv_info;
    memset(&key_serv_info,0,sizeof(input_key_service_cfg_t));
    key_serv_info.based_cfg.task_stack= INPUT_KEY_SERVICE_TASK_STACK_SIZE;
    key_serv_info.based_cfg.task_prio = INPUT_KEY_SERVICE_TASK_PRIORITY;
    key_serv_info.based_cfg.task_core = INPUT_KEY_SERVICE_TASK_ON_CORE;
    key_serv_info.based_cfg.extern_stack = false;
    key_serv_info.handle = _periph_set;
    */
    periph_service_handle_t input_key_handle = input_key_service_create(_periph_set);
    AUDIO_NULL_CHECK(LOG_TAG.c_str(), input_key_handle, return false);
    input_key_service_add_key(input_key_handle, input_info, INPUT_KEY_NUM);
    periph_service_set_callback(input_key_handle, inputKeyServiceCallBack,NULL);
   
   //Step3: get audio board handle
    _board_handle=audio_board_init();
    if(_board_handle==nullptr){
        ESP_LOGI(LOG_TAG.c_str(),"Get board handle fail!!!");
        return false;
    }
    //creat the button queue
    _button_queue_handle = xQueueCreate(BUTTON_MAX_QUEUE_LEN, sizeof(button_data_t));
    if(_button_queue_handle==nullptr){
        ESP_LOGI(LOG_TAG.c_str(),"Creat button handle fail!!!");
        return false;
    }
    /*if(!gpioInit()){
        return false;
    }*/
    _board_initialized=true;
    return true;
}
bool 
ESPBoard::gpioInit()
{
    gpio_config_t io_conf;
    memset(&io_conf,0,sizeof(gpio_config_t));
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that color 
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    return true;
}