/****************************************
*   AiBox LOG interface .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.08.28
*****************************************/
#pragma once
#include <iostream>
#include <fstream>
#include <memory>
namespace AiBoxLog
{
typedef struct LogColor{
    std::string _color;
    LogColor(const std::string& color):_color(color){}
}log_color_t;

static log_color_t _err_color("\033[1;31m");
static log_color_t _war_color("\033[33m");
static log_color_t _debug_color("\033[36m");
static log_color_t _info_color("\033[0m");
static std::shared_ptr<std::ofstream> log_file_stream_ptr=nullptr;
inline void startLogToFile(const std::string& file_name){
    if(log_file_stream_ptr.get()!=nullptr){
        return ;
    }
    log_file_stream_ptr=std::make_shared<std::ofstream>(file_name,std::ofstream::out);
}
inline void stopLogToFile(){
    if(log_file_stream_ptr.get()!=nullptr){
        log_file_stream_ptr->close();
    }
    log_file_stream_ptr.reset();
}
inline void AiBoxPrint(){
    std::cout<<"\n";
    if(log_file_stream_ptr.get()!=nullptr) *(log_file_stream_ptr.get())<<"\n";
}

template<typename H,typename... T>
void AiBoxPrint(H first,T... Args){
    std::cout<<first;
    if(log_file_stream_ptr.get()!=nullptr) *(log_file_stream_ptr.get())<<first;
    AiBoxPrint(Args...);
}
template<typename H,typename... T>
void AiBoxPrint(const log_color_t& color, H first,T... Args){
    std::cout<<color._color<<first;
    if(log_file_stream_ptr.get()!=nullptr) *(log_file_stream_ptr.get())<<first;
    AiBoxPrint(Args...);
}

typedef enum{
    LOG_LEVEL_NONE=0,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WAR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
}aibox_log_level_t;
static aibox_log_level_t _aibox_log_level=LOG_LEVEL_NONE;
inline void setLogLeveL(const aibox_log_level_t& level){_aibox_log_level=level;}
};

#include <chrono>
#define FILE_NAME(file_path) std::string(file_path).substr(std::string(file_path).find_last_of("/")+1)
#define CURRENT_TIME std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

#define AIBOX_LOGD(...)    if(AiBoxLog::_aibox_log_level>=AiBoxLog::LOG_LEVEL_DEBUG)  AiBoxLog::AiBoxPrint(AiBoxLog::_debug_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__)
#define AIBOX_LOGI(...)    if(AiBoxLog::_aibox_log_level>=AiBoxLog::LOG_LEVEL_INFO)   AiBoxLog::AiBoxPrint(AiBoxLog::_info_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__);
#define AIBOX_LOGW(...)    if(AiBoxLog::_aibox_log_level>=AiBoxLog::LOG_LEVEL_WAR)    AiBoxLog::AiBoxPrint(AiBoxLog::_war_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__);
#define AIBOX_LOGE(...)    if(AiBoxLog::_aibox_log_level>=AiBoxLog::LOG_LEVEL_ERR)    AiBoxLog::AiBoxPrint(AiBoxLog::_err_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__);
