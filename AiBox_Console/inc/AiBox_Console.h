/****************************************
*   AiBox console .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.09.2
*****************************************/
#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
namespace AiBox
{
class ConsoleFuncBase
{
    public:
        ConsoleFuncBase(){} 
        virtual void runConsoleFunc(const std::string& param)=0;
};
template<typename T>
class ConsoleFunc: public ConsoleFuncBase
{
    public:
        ConsoleFunc(T* ptr,void (T::*func)(const std::string&)):_ptr(ptr),_func(func){};
        void runConsoleFunc(const std::string& param)override
        {
            (_ptr->*_func)(param);
        }
    private:
        T* _ptr;
        void (T::*_func)(const std::string&);
};

class Console
{
    public:
        Console(const Console&)=delete;
        ~Console();
        void operator=(const Console&)=delete;
        static std::shared_ptr<Console> getInstance();
        template<typename H>
        static void registeConsoleFunc(const std::string& func_key, void (H::*func)(const std::string&),H* class_ptr)
        {
            std::lock_guard<std::mutex> lock(_map_mutex);
            _console_func_map.insert(std::make_pair(func_key,std::make_shared<ConsoleFunc<H>>(class_ptr,func)));
        }
        bool run();
    private:
        Console();
        void runConsoleThread();
        static std::map<std::string,std::shared_ptr<ConsoleFuncBase>> _console_func_map;
        static std::mutex _map_mutex;
        std::shared_ptr<std::thread> _console_thread;
        bool _console_thread_should_exit;
        static std::shared_ptr<Console> _singlon;
};
};