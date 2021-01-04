/****************************************
*   uart interface for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.05.09
*****************************************/
#pragma once
#include <string>
#include "AiBox_UARTBase.h"
namespace AiBox
{
    class SystemUartBase;
    class SystemUart
    {
        public:
            SystemUart(const SystemUart&)=delete;
            SystemUart& operator=(const SystemUart&)=delete;
            ~SystemUart(){}
            static SystemUart* getSystemUartInstance();
            bool init();
            const QueueHandle_t& getUartEventHandle();
            int write(const std::string& command);
            int read(std::string& msg,int size);
        private:
            SystemUart(){}
            SystemUartBase* _uart;
            static SystemUart* _singleton_instance;
    };
};
