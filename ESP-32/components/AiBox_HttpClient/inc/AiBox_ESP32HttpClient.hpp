/****************************************
*   esp32  http client .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#pragma once
#include <string>
#include "AiBox_HttpClientBase.hpp"
#include "esp_http_client.h"
#include "esp_err.h"
#include "AiBox_HttpRequestData.hpp"
namespace AiBox
{
	class ESPHttpClientManage : public HttpClientBase{
		public:
			ESPHttpClientManage():HttpClientBase(){}
			~ESPHttpClientManage(){}
			void httpRequestRun()override;
	};

	class HttpResponseHandle{
		public:
			HttpResponseHandle(requestCallback response_handle){_response_handle=response_handle;}
			void* operator new(std::size_t size);
            void  operator delete(void *ptr);
			void operator()(int response_code){
				_response_handle(_response_data,response_code);
			}
			std::string _response_data{std::string()};
		private:
			requestCallback _response_handle;
			
	};
};
