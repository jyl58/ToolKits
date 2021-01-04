/****************************************
*   console base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.02
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_ConsoleBase.hpp"
using namespace AiBox;
const std::string LOG_TAG="AiBoxConsoleBase";
void 
ConsoleBase::runConsole(void* param)
{
    ((ConsoleBase*)param)->exec();
    vTaskDelete(NULL);
}
 ConsoleBase::~ConsoleBase()
{
    if(_task_buffer!=NULL){
        sysMemFree(_task_buffer);
        _task_buffer=NULL;
    }
    if(_task_TCB!=NULL){
        sysMemFree(_task_TCB);
        _task_TCB=NULL;
    }
}
void 
ConsoleBase::run()
{
    _task_buffer=(StackType_t*)sysMemMalloc(1024*8);
    if(_task_buffer==NULL){
        return ;
    }
    _task_TCB=(StaticTask_t*)sysMemMallocInner(1,sizeof(StaticTask_t));
    if(_task_TCB==NULL){
        sysMemFree(_task_buffer);
        return;
    }
    if(xTaskCreateStaticPinnedToCore(&ConsoleBase::runConsole, "console_task", 1024*8, this, 5, _task_buffer,_task_TCB,1)==NULL){
        LOGI(LOG_TAG.c_str(),"Creat Console task err!");
        return;
    }
    LOGI(LOG_TAG.c_str(),"Creat Console run task!");
}