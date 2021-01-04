/****************************************
*   console app.cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
	#include "AiBox_ESPConsole.hpp"
#endif
#include "AiBox_Console.hpp"

using namespace AiBox;
const std::string LOG_TAG="AiBoxConsole";
Console* Console::_singleton_instance=nullptr;
Console* Console::getConsoleInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new Console();
    }
    return _singleton_instance;
}
Console::Console()
{
    _singleton_instance=this;
}
Console::~Console()
{
    if(_console!=nullptr){
        delete  _console;
         _console=nullptr;
    }
}
bool 
Console::consoleInit(const std::string prompt)
{
#ifdef ESP32_PLATFORM
	_console=new ESPConsole(prompt);
#endif
    if(_console==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat Console manage err!");
		return false;
    }
    if(!_console->init()){
        LOGI(LOG_TAG.c_str(),"Console init err!");
		return false;
    }
    return true;
}
void 
Console::runConsole()
{
    if(_console!=nullptr){
        _console->run();
    }
}