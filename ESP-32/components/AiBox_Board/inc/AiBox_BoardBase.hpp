/****************************************
*   Board base for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "AiBox_BoardInterface.hpp"
#define BUTTON_MAX_QUEUE_LEN 4
#define GPIO_LED_RED_PIN 4
#define GPIO_LED_GREEN_PIN 5
#define GPIO_LED_BLUE_PIN 18
#define GPIO_LED_ORANGE_PIN 19
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_LED_RED_PIN)|(1ULL<<GPIO_LED_GREEN_PIN)|(1ULL<<GPIO_LED_BLUE_PIN)|(1ULL<<GPIO_LED_ORANGE_PIN)
namespace AiBox
{
    class BoardInterface;
    class BoardBase
    {
        public:
            friend BoardInterface;
            BoardBase(){}
            ~BoardBase(){}
            virtual bool init()=0;
        protected:
            bool _board_initialized{false};
            QueueHandle_t _button_queue_handle=nullptr;
    };
};