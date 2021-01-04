/****************************************
*   esp32 http client base .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.24
*****************************************/
#include <cstring>
#include"AiBox_ESP32HttpClient.hpp"
#include "esp_log.h"
using namespace AiBox;
static const std::string LOG_TAG ="ESPHttpClient";
esp_err_t _http_event_handler(esp_http_client_event_t *evt){
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
                (*((HttpResponseHandle*)(evt->user_data)))._response_data=std::string();
                (*((HttpResponseHandle*)(evt->user_data)))(esp_http_client_get_status_code(evt->client));//request success run the response handle function
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;
        case HTTP_EVENT_HEADER_SENT:
            break;
        case HTTP_EVENT_ON_HEADER:
            break;
        case HTTP_EVENT_ON_DATA:
                (*((HttpResponseHandle*)(evt->user_data)))._response_data+=std::string((char*)evt->data,evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
			    (*((HttpResponseHandle*)(evt->user_data)))(esp_http_client_get_status_code(evt->client));  //request success run the response handle function
            break;
        case HTTP_EVENT_DISCONNECTED:
            break;
    }
    return ESP_OK;
}

void 
ESPHttpClientManage::httpRequestRun(){
	//get this task need request data
	http_request_data_t temp_request_data=_request_data;
	HttpResponseHandle* temp_response_handle=new HttpResponseHandle(temp_request_data._response_func);

    esp_http_client_config_t config; 
	memset(&config,0,sizeof(esp_http_client_config_t));
    config.url = temp_request_data._url.c_str();
    config.event_handler = &_http_event_handler;
	config.user_data = temp_response_handle;
	//step1: init the esp http client
	esp_http_client_handle_t client = esp_http_client_init(&config);
    if(client==NULL){
        ESP_LOGI(LOG_TAG.c_str(),"Init http client err!!!");
        delete temp_response_handle;
        return;
    }
    //step2: parser and set the http header,if need
    if(!temp_request_data._header.empty()){
        std::string::size_type pos1=temp_request_data._header.find_first_not_of(" ",0);
        std::string::size_type pos2=0;
        std::string::size_type pos3=0;
        std::string  key=std::string();
        std::string  value=std::string();
        //earse the last ";" if  exist
        if(temp_request_data._header.back()==';'){
            temp_request_data._header.pop_back();
        }
        do{
            pos2=temp_request_data._header.find(";",pos1);
            pos3=temp_request_data._header.find("=",pos1);
            if(pos3==std::string::npos){
                ESP_LOGI(LOG_TAG.c_str(),"Header format err!!");
                break;
            }
            key   = temp_request_data._header.substr(pos1,pos3-pos1);
            value = temp_request_data._header.substr(pos3+1,pos2-pos3-1);
            //ESP_LOGI(LOG_TAG.c_str(),"Header: key=%s, value=%s",key.c_str(),value.c_str());
            esp_http_client_set_header(client,key.c_str(),value.c_str());
            if(pos2!=std::string::npos){
                pos1=pos2+1;
            }
        }while(pos2!=std::string::npos);
    }
    //step3: set the request
    if(temp_request_data._method==RequestMethod_POST){
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_post_field(client, temp_request_data._post_data.c_str(), temp_request_data._post_data.size());
    }else{
        esp_http_client_set_method(client, HTTP_METHOD_GET);
    }
    //step4: run the http request
    esp_err_t ret=esp_http_client_perform(client);
    if(ret!=ESP_OK){
        ESP_LOGE(LOG_TAG.c_str(), "HTTP request failed: %s", esp_err_to_name(ret));
    }else{
    }
	//clean the http client
	esp_http_client_cleanup(client);
	delete temp_response_handle;
    //set the finished flag
    _request_finished=true;
}


void*
HttpResponseHandle::operator new(std::size_t size)throw(std::bad_alloc)
{
	return heap_caps_malloc( size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}
void 
HttpResponseHandle::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
	heap_caps_free(ptr);
	ptr=nullptr;
}


















