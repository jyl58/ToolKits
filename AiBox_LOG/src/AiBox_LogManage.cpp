#include "AiBox_LogManage.h"
using namespace AiBox;
std::shared_ptr<std::ofstream> AiBoxLog::_log_file_ptr=nullptr;
const AiBoxLog::log_color_t AiBoxLog::_err_color("\033[1;31m");
const AiBoxLog::log_color_t AiBoxLog::_war_color("\033[33m");
const AiBoxLog::log_color_t AiBoxLog::_debug_color("\033[36m");
const AiBoxLog::log_color_t AiBoxLog::_info_color("\033[0m");
std::shared_ptr<LogManage> LogManage::_singleton=nullptr;
std::shared_ptr<LogManage> LogManage::getInstance()
{
    if(_singleton.get()==nullptr){
        new LogManage();
    }
    return _singleton;
}
LogManage::LogManage()
{
    _singleton=std::shared_ptr<LogManage>(this);
}

bool
LogManage::init(bool debug_mode)
{
    if(!debug_mode){
        std::cout.rdbuf(nullptr);
    }
    return true;
}
LogManage::~LogManage()
{
    stopLogToFile();
}
void 
LogManage::logToFile(const std::string& file_name)
{
    AiBoxLog::_log_file_ptr=std::make_shared<std::ofstream>(file_name,std::ofstream::out);
    if(AiBoxLog::_log_file_ptr->is_open()){
        _cout_backup=std::cout.rdbuf();
        std::cout.rdbuf(AiBoxLog::_log_file_ptr.get()->rdbuf());
    }else{
        AIBOX_LOGE("Open Log File err.");
    }
}
void 
LogManage::stopLogToFile()
{
    if(AiBoxLog::_log_file_ptr.get()!=nullptr){
        std::cout.rdbuf(_cout_backup);
        AiBoxLog::_log_file_ptr.get()->close();
        AiBoxLog::_log_file_ptr.reset();
    }
}