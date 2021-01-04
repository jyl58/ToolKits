/****************************************
*   console base .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#pragma once
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
namespace AiBox
{
    class ConsoleBase
    {
        public:
            ConsoleBase(const std::string prompt):_prompt(prompt){}
            virtual ~ConsoleBase();
            virtual bool init()=0;
            virtual void exec()=0;
            void run();

        protected:
            std::string _prompt;
        private:
            static void runConsole(void* param);
            StackType_t* _task_buffer=nullptr;
            StaticTask_t* _task_TCB=nullptr;
    };
};