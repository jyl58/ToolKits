/****************************************
*   console app.hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#pragma once
#include "AiBox_ConsoleBase.hpp"
namespace AiBox
{
    class Console
    {
        public:
            Console();
            Console(const Console&)=delete;
            Console& operator=(const Console&)=delete;
            ~Console();
            static Console* getConsoleInstance();
            bool consoleInit(const std::string prompt);
            void runConsole();

        private:
            ConsoleBase* _console;
            static Console* _singleton_instance;
    };
};