/****************************************
*   http client interface to app .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include "AiBox_HttpClientInterface.hpp"
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
	#include"AiBox_ESP32HttpClient.hpp"
#endif
using namespace AiBox;
static const std::string LOG_TAG = "HttpClientInterface";

HttpClientInterface* HttpClientInterface::_singleton_instance=nullptr;

HttpClientInterface* 
HttpClientInterface::getHttpClientInstance(){
	if(_singleton_instance==nullptr){
		_singleton_instance=new HttpClientInterface();
	}
	return _singleton_instance;
}
HttpClientInterface::HttpClientInterface()
{
	_singleton_instance=this;
}
HttpClientInterface::~HttpClientInterface()
{
}
bool
HttpClientInterface::init(){
	return true;
}
void 
HttpClientInterface::httpRequest(const http_request_data_t& request_data){
	//step1: creat a http client 
	HttpClientBase* http_client=new ESPHttpClientManage();
	if( http_client==NULL){
		LOGI(LOG_TAG.c_str(),"Creat http client err!");
		return;
	}
	//step2: creat a request 
	http_client->creatHttpRequestTask(request_data);
	//step3： add the http clietn to list
	_http_client_list.push_back(http_client);
}
void 
HttpClientInterface::recycleHttpRequestMem()
{
	if(_http_client_list.empty()){
		return;
	}
	for(auto &item:_http_client_list){
		if(item->isFinished()){
			delete item;
			item=nullptr;
		}
	}
	_http_client_list.remove(nullptr);
	LOGI(LOG_TAG.c_str(),"Delete the finished http request memory.");
}