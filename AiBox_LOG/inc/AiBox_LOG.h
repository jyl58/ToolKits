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

extern const log_color_t _err_color;
extern const log_color_t _war_color;
extern const log_color_t _debug_color;
extern const log_color_t _info_color;
extern std::shared_ptr<std::ofstream> _log_file_ptr;

inline void AiBoxPrint(){
    std::cout<<"\n";
}
template<typename H,typename... T>
void AiBoxPrint(H first,T... Args){
    std::cout<<first;
    AiBoxPrint(Args...);
}
template<typename... T>
void AiBoxPrint(const log_color_t& color, T... Args){
    if(_log_file_ptr.get()==nullptr){
        std::cout<<color._color;
    }
    AiBoxPrint(Args...);
}
};

#include <chrono>
#define FILE_NAME(file_path) std::string(file_path).substr(std::string(file_path).find_last_of("/")+1)
#define CURRENT_TIME std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define CURRENT_TIME_S std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()

#define AIBOX_LOGD(...)    AiBoxLog::AiBoxPrint(AiBoxLog::_debug_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__)
#define AIBOX_LOGI(...)    AiBoxLog::AiBoxPrint(AiBoxLog::_info_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__)
#define AIBOX_LOGW(...)    AiBoxLog::AiBoxPrint(AiBoxLog::_war_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__)
#define AIBOX_LOGE(...)    AiBoxLog::AiBoxPrint(AiBoxLog::_err_color,"[",CURRENT_TIME,"]",FILE_NAME(__FILE__),"<",__LINE__,"> ",__VA_ARGS__)
