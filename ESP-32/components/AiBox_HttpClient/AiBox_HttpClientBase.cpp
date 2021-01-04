/****************************************
*   http client base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include <cstring>
#include "AiBox_HttpClientBase.hpp"
#include "LogInterface.hpp"
using namespace AiBox;
static const std::string LOG_TAG ="HttpClientBase";
void HttpRequestTaskFunction(void * param)
{
	//run the http request
	((HttpClientBase*)param)->httpRequestRun();
	//run complete, so delete self in the task List
	vTaskDelete(NULL);
}
void*
HttpClientBase::operator new(std::size_t size)throw(std::bad_alloc)
{
	return sysMemMalloc(size);
}
void 
HttpClientBase::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
	sysMemFree(ptr);
	ptr=nullptr;
}

void 
HttpClientBase::creatHttpRequestTask(const http_request_data_t& request_data)
{
	_request_data=request_data;
	_task_buffer=(StackType_t*)sysMemMalloc(HTTP_TASK_TACK_SIZE);
	if(_task_buffer==NULL){
		LOGI(LOG_TAG.c_str(),"http task stack is empty!");
		_request_finished=true;
		return;
	}
	memset(_task_buffer,0,HTTP_TASK_TACK_SIZE);
	_task_TCB=(StaticTask_t*)sysMemMallocInner(1, sizeof(StaticTask_t));
	if(_task_TCB==NULL){
		LOGI(LOG_TAG.c_str(),"http task TCB is empty!");
		sysMemFree(_task_buffer);
		_task_buffer=NULL;
		_request_finished=true;
		return;
	}
	//use 1 app cpu core
	if(xTaskCreateStaticPinnedToCore(&HttpRequestTaskFunction, "http_request_task",HTTP_TASK_TACK_SIZE , this, 10, _task_buffer,_task_TCB,1)==NULL){
		LOGI(LOG_TAG.c_str(),"Creat http request task err!");
	}
}
HttpClientBase::~HttpClientBase()
{
	if(_task_buffer!=NULL){
		sysMemFree(_task_buffer);
	}
	if(_task_TCB!=NULL){
		sysMemFree(_task_TCB);
	}
}