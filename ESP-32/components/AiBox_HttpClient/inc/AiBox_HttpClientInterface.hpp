/****************************************
*   http client interface to app .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#pragma once
#include <string>
#include <list>
#include "AiBox_HttpRequestData.hpp"
#include "AiBox_HttpClientBase.hpp"
namespace AiBox
{
	class HttpClientBase;
	class HttpClientInterface{
		public:
			HttpClientInterface();
			HttpClientInterface(const HttpClientInterface&)=delete;
			HttpClientInterface& operator=(const HttpClientInterface&)=delete;
			~HttpClientInterface();
			/*get the singleton instance*/
			static HttpClientInterface* getHttpClientInstance();
			/*init the Interface for third partner*/
			bool init();
			/*creat a http request */
			void httpRequest(const http_request_data_t& request_data);
			void recycleHttpRequestMem();
		private:
			std::list<HttpClientBase*> _http_client_list;
			static HttpClientInterface* _singleton_instance;
	};
};