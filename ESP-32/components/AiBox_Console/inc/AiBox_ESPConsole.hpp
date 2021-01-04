/****************************************
*   ESP console .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#pragma once
#include <string>
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "esp_err.h"
#include "AiBox_ConsoleBase.hpp"
namespace AiBox
{
    class ESPConsole:public AiBox::ConsoleBase
    {
        public:
            ESPConsole(const std::string prompt):AiBox::ConsoleBase(prompt){}
            ~ESPConsole(){}
            bool init()override;
            void exec()override;
            void registerDefaultCMD();
    };
};