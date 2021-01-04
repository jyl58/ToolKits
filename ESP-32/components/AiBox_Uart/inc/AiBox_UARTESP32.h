/****************************************
*   uart esp32 class for .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.09
*****************************************/
#pragma once
#include <string>
#include "AiBox_UARTBase.h"
#include "driver/uart.h"
#define COMMAND_UART_NUM  UART_NUM_1
#define COMMAND_UART_TX_GPIO_NUM 22
#define COMMAND_UART_RX_GPIO_NUM 21
namespace AiBox
{
    class ESP32Uart :public SystemUartBase
    {
        public:
            ESP32Uart(){}
            ESP32Uart(const ESP32Uart&)=delete;
            ~ESP32Uart(){}
            bool init()override;
            int write(const std::string& command)override;  
            int read(std::string& msg,int size)override;
        private:
            void registerUartCMD();

    };
};