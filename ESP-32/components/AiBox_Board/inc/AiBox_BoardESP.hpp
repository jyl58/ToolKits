/****************************************
*   ESP Board  for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include "board.h"
#include "AiBox_BoardBase.hpp"
#include "periph_service.h"
#include "esp_peripherals.h"
namespace AiBox
{
    class ESPBoard: public BoardBase
    {
        public:
            ESPBoard():BoardBase(){}
            ~ESPBoard(){}
            bool init()override;
            static audio_board_handle_t _board_handle;
            static esp_periph_set_handle_t _periph_set;
        private:
            bool gpioInit();
    };
};