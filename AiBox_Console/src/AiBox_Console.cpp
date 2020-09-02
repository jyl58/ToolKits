/****************************************
*   AiBox console .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.09.2
*****************************************/
#include <iostream>
#include "AiBox_Console.h"
using namespace AiBox;
std::shared_ptr<Console> Console::_singlon=nullptr;
std::map<std::string,std::shared_ptr<ConsoleFuncBase>>  Console::_console_func_map;
std::mutex Console::_map_mutex;

Console::Console()
{
    _singlon=std::shared_ptr<Console>(this);
}
Console::~Console()
{
    if(_console_thread.get()!=nullptr){
        _console_thread_should_exit=true;
        _console_thread->join();
        _console_thread.reset();
    }
    _console_func_map.clear();
}
std::shared_ptr<Console> 
Console::getInstance()
{
    if(_singlon.get()==nullptr){
        new Console();
    }
    return _singlon;
}
void 
Console::runConsoleThread()
{
    std::string input;
    std::string command;
    std::string param;
    while(!_console_thread_should_exit){
        command.clear();
        param.clear();
        getline(std::cin,input);
        input=input.substr(input.find_first_not_of(" "));
        command=input.substr(0,input.find_first_of(" "));
        param=input.substr(input.find_first_of(" ")+1);
        {
            std::lock_guard<std::mutex> lock(_map_mutex);
            if(_console_func_map.find(command)!=_console_func_map.end()){
                _console_func_map[command]->runConsoleFunc(param);
            }else{
                std::cout<<"input command: ["<<command<<"] is not support."<<std::endl;
            }
        }
    }
}
bool
Console::run()
{
    _console_thread_should_exit=false;
    _console_thread=std::make_shared<std::thread>(&Console::runConsoleThread,this);
    if(_console_thread.get()==nullptr){
        return false;
    }
    return true;
}