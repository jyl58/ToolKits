/****************************************
*   AiBox LOG interface .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.08.28
*****************************************/
#pragma once
#include "AiBox_LOG.h"
namespace AiBox
{
class LogManage
{
    public:
        ~LogManage();
        static std::shared_ptr<LogManage> getInstance();
        LogManage(const LogManage& )=delete;
        LogManage& operator=(const LogManage&)=delete;
        bool init(bool debug_mode=false);
        void logToFile(const std::string& file_name);
        void stopLogToFile();
    private:
        LogManage();
        static std::shared_ptr<LogManage> _singleton;
        std::streambuf *_cout_backup=nullptr;
};
};