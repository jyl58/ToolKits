/****************************************
*   uart base class for   .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.09
*****************************************/
#pragma once
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "AiBox_UART.h"
#define COMMAND_UART_QUEUE_SIZE 10
#ifdef ESP32_PLATFORM
#include "driver/uart.h"
    typedef uart_event_t AiBox_uart_event_t;
#endif

namespace AiBox
{
    class SystemUart;
    class SystemUartBase
    {
        public:
            friend SystemUart;
            SystemUartBase(){}
            virtual ~SystemUartBase(){}
            virtual bool init()=0;
            virtual int write(const std::string& command)=0;
            virtual int read(std::string& msg,int size)=0;
        protected:
            QueueHandle_t _commnd_uart_event_handle=nullptr;

    };
};