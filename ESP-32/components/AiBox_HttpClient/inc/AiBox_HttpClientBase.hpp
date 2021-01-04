/****************************************
*   http client base .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "AiBox_HttpClientInterface.hpp"
#include "AiBox_HttpRequestData.hpp"
#define HTTP_TASK_TACK_SIZE 1024*10 
namespace AiBox
{
	class HttpClientBase{
		public :
			HttpClientBase(){}
			void* operator new(std::size_t size);
            void  operator delete(void *ptr);
			void creatHttpRequestTask(const http_request_data_t& request_data);
			virtual void httpRequestRun()=0;
			virtual ~HttpClientBase();
			bool isFinished(){return _request_finished==true;}
		protected:
			http_request_data_t _request_data;
			bool _request_finished{false};
			StackType_t * _task_buffer=nullptr;
            StaticTask_t * _task_TCB=nullptr;
	};
};
extern "C" void HttpRequestTaskFunction(void * param);