/****************************************
*   Board interface for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.10
*****************************************/
#pragma once
#include <string>
#include "AiBox_BoardBase.hpp"
#include "AiBox_BoardDataType.hpp"
namespace AiBox
{
    class BoardBase;
    class BoardInterface
    {
        public:
            BoardInterface();
            BoardInterface(const BoardInterface&)=delete;
            void operator=(const BoardInterface&)=delete;
            ~BoardInterface(){}
            static BoardInterface* getBoardInterfaceInstance();
            bool initBoard();
            QueueHandle_t getButtonQueueHandle();
        private:
            BoardBase* _board{nullptr};
            static BoardInterface* _singleton_instance;
    };
};