/****************************************
*   http request struct data .h
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.25
*****************************************/
#pragma once
#include <functional>
#include <string>
namespace AiBox
{
	typedef enum RequestMethod{
		RequestMethod_GET,
		RequestMethod_POST	
	}request_method_t;
	using requestCallback = std::function<void (const std::string&,int)>;
	typedef struct HttpRequestData{
		std::string _url;
		request_method_t _method;
		std::string _post_data;
		std::string _header;    //used to add the requst headr msg ,format:   key1=value1;key2=value2,...
		requestCallback _response_func;
	}http_request_data_t;
};
