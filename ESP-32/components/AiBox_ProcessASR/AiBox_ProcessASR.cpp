/****************************************
*   ASR process class .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.16
*****************************************/
#include <cstring>
#include "LogInterface.hpp"
#include "AiBox_ProcessASR.hpp"
#include "cJSON.h"
#include "AiBox_SystemInterface.hpp"
#include "AiBox_HttpClientInterface.hpp"
#include "AiBox_SemanticParse.hpp"
using namespace AiBox;
std::string LOG_TAG="ASRProcess";

int processASRCMD(int argc, char **argv)
{
	if(argc!=2){
		LOGI(LOG_TAG.c_str(),"asr CMD need a param\n");
		return 0;
	}
	ASRProcess::getProcessASRInstance()->ProcessASRResult(argv[1],"111111111111");
	return 0;
}
ASRProcess* ASRProcess::_singleton_instance=nullptr;
ASRProcess* ASRProcess::getProcessASRInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new ASRProcess();
    }
    return  _singleton_instance;
}
void* 
ASRProcess::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
ASRProcess::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
}
ASRProcess::ASRProcess()
{
	_singleton_instance=this;
}
ASRProcess::~ASRProcess()
{

}
std::string
ASRProcess::createMusicJson(const std::string& sessionID, const std::string& musicTTS)
{
	if (sessionID.empty()){
		return std::string();
	}
	cJSON *pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot){
		//error happend here
		LOGI(LOG_TAG.c_str(),"create json root fail\n");
		return  std::string();
	}
	cJSON_AddStringToObject(pJsonRoot, "deviceid", SystemStatusData::getSystemStatusDataInstance()->getJHKDeviceID().c_str());
	cJSON_AddStringToObject(pJsonRoot, "dialid", sessionID.c_str());
	if (!musicTTS.empty()){
		cJSON_AddStringToObject(pJsonRoot, "response", musicTTS.c_str());
	}else{
		cJSON_AddStringToObject(pJsonRoot, "response", "");
	}
	LOGI(LOG_TAG.c_str(),"create music id:%s, tts:%s\n", sessionID.c_str(), musicTTS.c_str());

	std::string p = cJSON_Print(pJsonRoot);
	if ( p.empty()){
		//convert json list to string faild, exit
		//because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
		cJSON_Delete(pJsonRoot);
		LOGI(LOG_TAG.c_str(),"no music\n");
		return  std::string();
	}
	cJSON_Delete(pJsonRoot);
	LOGI(LOG_TAG.c_str(),"music json::%s\n", p.c_str());
	return p;
}
std::string 
ASRProcess::createSystemFaultJson(const std::string& sessionID,const std::string& errType,const std::string& desc, const std::string& engineType,unsigned int cost)
{
	if (sessionID.empty()){
		return std::string();
	}
	cJSON *pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();
	if (NULL == pJsonRoot){
		//error happend here
		LOGI(LOG_TAG.c_str(),"create system fault json root fail\n");
		return  std::string();
	}
	cJSON_AddStringToObject(pJsonRoot, "type", errType.c_str());
	cJSON_AddStringToObject(pJsonRoot, "desc", desc.c_str());
	//device type
	cJSON_AddStringToObject(pJsonRoot, "devicetype","AIR_CONDITIONER");

	cJSON_AddStringToObject(pJsonRoot, "id", sessionID.c_str());
	cJSON_AddStringToObject(pJsonRoot, "engine", engineType.c_str());
	cJSON_AddNumberToObject(pJsonRoot, "cost", cost);
	cJSON_AddStringToObject(pJsonRoot, "deviceid", SystemStatusData::getSystemStatusDataInstance()->getJHKDeviceID().c_str());
	cJSON_AddStringToObject(pJsonRoot, "pos", SystemStatusData::getSystemStatusDataInstance()->getPosition().c_str());
	cJSON_AddStringToObject(pJsonRoot, "vver", VIDAA_VERSION.c_str()); 
	cJSON_AddStringToObject(pJsonRoot, "cver", "1");

	cJSON_AddNumberToObject(pJsonRoot, "timestamp", SystemInterface::getSystemInterfaceInstance()->getCurrentTimeMs());

	std::string p = cJSON_Print(pJsonRoot);
	if ( p.empty()){
		//convert json list to string faild, exit
		//because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
		cJSON_Delete(pJsonRoot);
		LOGI(LOG_TAG.c_str(),"no fault json\n");
		return  std::string();
	}
	cJSON_Delete(pJsonRoot);
	LOGI(LOG_TAG.c_str(),"music json::%s\n", p.c_str());
	return p;
}
void 
ASRProcess::uploadCustomID()
{
	if(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		return;
	}
	std::string paraCustomID;
	paraCustomID+="customerid=" +SystemStatusData::getSystemStatusDataInstance()->getCustomerID();
	paraCustomID+="&requestversion=" + SystemStatusData::getSystemStatusDataInstance()->getRequestVersion();
	paraCustomID+="&coreversion=" + SystemStatusData::getSystemStatusDataInstance()->getPackageVersion();
	paraCustomID+="&deviceid=" + SystemStatusData::getSystemStatusDataInstance()->getJHKDeviceID();
	paraCustomID+="&smarthomedeviceid=" + SystemStatusData::getSystemStatusDataInstance()->getJHLDeviceID();
	paraCustomID+="&featurecode=" + SystemStatusData::getSystemStatusDataInstance()->getTvFeatureCode();
	http_request_data_t custom_id_request_data;
	memset(&custom_id_request_data,0,sizeof(http_request_data_t));
	custom_id_request_data._url=SEMANTIC_URL_CUSTOMID;
	custom_id_request_data._method=RequestMethod_POST;
	//First: creat post data
	custom_id_request_data._post_data=HttpPostDataCreate(paraCustomID.c_str(), DATA_TYPE_CUSTOM_ID);
	custom_id_request_data._header=std::string();
	custom_id_request_data._response_func=ASRProcess::uploadCustomIDCallBack;
	//Second: requst to hisense http server
	HttpClientInterface::getHttpClientInstance()->httpRequest(custom_id_request_data);
}
void 
ASRProcess::uploadCustomIDCallBack(const std::string& response,int response_code)
{
	LOGI(LOG_TAG.c_str(),">>>Response content: %s;code: %d<<<",response.c_str(),response_code);
}
/*upload music info*/
void  
ASRProcess::uploadMusicInfo(const std::string& upload_json)
{
	if(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		//wifi is off,play hint audio 
		return;
	}
	http_request_data_t music_info_request_data;
	memset(&music_info_request_data,0,sizeof(http_request_data_t));
	music_info_request_data._url=SEMANTIC_URL_MUSIC;
	music_info_request_data._method=RequestMethod_POST;
	//First: creat post data
	music_info_request_data._post_data=HttpPostDataCreate(upload_json.c_str(), DATA_TYPE_MUSIC_INFO);
	music_info_request_data._header=std::string();
	music_info_request_data._response_func=ASRProcess::uploadMusicInfoCallBack;
	//Second: requst to hisense http server
	HttpClientInterface::getHttpClientInstance()->httpRequest(music_info_request_data);
}
void 
ASRProcess::uploadMusicInfoCallBack(const std::string& response,int response_code)
{
	LOGI(LOG_TAG.c_str(),">>>Response content: %s;code: %d<<<",response.c_str(),response_code);
}
/*upload fault info*/
void 
ASRProcess::uploadSystemFault(const std::string& upload_fault_json)
{
	if(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		// wifi is off,play hint audio 
		return;
	}
	http_request_data_t system_fault_request_data;
	memset(&system_fault_request_data,0,sizeof(http_request_data_t));
	system_fault_request_data._url=SEMANTIC_URL_FAULT;
	system_fault_request_data._method=RequestMethod_POST;
	//First: creat post data
	system_fault_request_data._post_data=HttpPostDataCreate(upload_fault_json.c_str(), DATA_TYPE_FAULT_LOGCAT);
	system_fault_request_data._header=std::string();
	system_fault_request_data._response_func=ASRProcess::uploadSystemFaultCallBack;
	//Second: requst to hisense http server
	HttpClientInterface::getHttpClientInstance()->httpRequest(system_fault_request_data);
}
void 
ASRProcess::uploadSystemFaultCallBack(const std::string& response,int response_code)
{
	LOGI(LOG_TAG.c_str(),">>>Response content: %s;code: %d<<<",response.c_str(),response_code);
}
std::string
ASRProcess::HttpPostDataCreate(const std::string& strText, int dataType)
{
    std::string outAESData=std::string();
	//First:encrypt
	switch(dataType)
	{
		case DATA_TYPE_FAULT_LOGCAT:
			if (SystemInterface::getSystemInterfaceInstance()->aesEncrypt(strText, SystemBase::SEMANTIC_AES_KEY_FAULT, outAESData) <= 0){
				LOGI(LOG_TAG.c_str(),"Fault logcat fail\n");
				return std::string();
			}
			break;
		case DATA_TYPE_MUSIC_INFO:
			if (SystemInterface::getSystemInterfaceInstance()->aesEncrypt(strText, SystemBase::SEMANTIC_AES_KEY_MUSIC, outAESData)<= 0){
				LOGI(LOG_TAG.c_str(),"Music malloc fail!\n");
				return std::string();
			}
			break;
#ifdef ENABLE_CUSTOMID_UPLOAD
		case DATA_TYPE_CUSTOM_ID:
			if (SystemInterface::getSystemInterfaceInstance()->aesEncrypt(strText, SystemBase::SEMANTIC_AES_KEY_CUSTOMID, outAESData)<= 0){
				LOGI(LOG_TAG.c_str(),"CustomID malloc fail!\n");
				return std::string();
			}
			break;
#endif
		default:
			{
				std::string strHttpData;
				strHttpData+="deviceid=" + SystemStatusData::getSystemStatusDataInstance()->getJHKDeviceID();
				strHttpData+="&devicetype=AIR_CONDITIONER";
				strHttpData+="&wifiid=" + SystemStatusData::getSystemStatusDataInstance()->getWifiID();
				strHttpData+="&extdeviceid=" + SystemStatusData::getSystemStatusDataInstance()->getACDeviceID();
				strHttpData+="&requestversion=" + SystemStatusData::getSystemStatusDataInstance()->getRequestVersion();
				strHttpData+="&vidaaversion=" + VIDAA_VERSION;
				strHttpData+="&packagename=" + PACKAGE_NAME;
				strHttpData+="&micarrayfac=" + _mic_arrayfac;
				strHttpData+="&wakeupwords="+ SystemStatusData::getSystemStatusDataInstance()->getWakeUpKey(); //the wakeup word
#ifdef USE_COMPRESS_FUNCTION
				strHttpData+="&responsecompress=1";
#endif
				strHttpData+="&featurecode=" + SystemStatusData::getSystemStatusDataInstance()->getTvFeatureCode();
				strHttpData+="&subscribeid=" + SystemStatusData::getSystemStatusDataInstance()->getSubscriberID();
				strHttpData+="&coreversion=";

				strHttpData+=SystemStatusData::getSystemStatusDataInstance()->getPackageVersion();
				strHttpData+="&customerid="+ SystemStatusData::getSystemStatusDataInstance()->getCustomerID();

				strHttpData+="&jhktoken="+ SystemStatusData::getSystemStatusDataInstance()->getJHKToken();

				strHttpData+="&childmode=";
				strHttpData+=SystemStatusData::getSystemStatusDataInstance()->getChildModeState()? "1": "0";
				strHttpData+="&scene=0";
				/*if(SystemStatusData::getSystemStatusDataInstance()->getScene().compare("scene")!=0){
					strHttpData+="1";
					SystemStatusData::getSystemStatusDataInstance()->resetScene();  //reset the scene
				}else{
					strHttpData+="0";
				}*/

				strHttpData+="&smarthomedeviceid=" + SystemStatusData::getSystemStatusDataInstance()->getJHLDeviceID();
				strHttpData+="&position="+ SystemStatusData::getSystemStatusDataInstance()->getPosition();
				strHttpData+="&retext=" + strText;

				strHttpData+="&oneshotmode=";
				if(SemanticParse::getSemanticParseInstance()->getOneShotMode()){
					strHttpData+="true";
				}else{
					strHttpData+="false";
				}
				LOGI(LOG_TAG.c_str(),"http Post content:%s\n", strHttpData.c_str());
				if (SystemInterface::getSystemInterfaceInstance()->aesEncrypt(strHttpData.c_str(), SystemBase::SEMANTIC_AES_KEY, outAESData)<= 0){
				    LOGI(LOG_TAG.c_str(),"sematic malloc fail\n");
					return NULL;
				}
			}
			break;
	}
//Second:base64 encode
	std::string after_base64=std::string();
	if(SystemInterface::getSystemInterfaceInstance()->base64Encode(outAESData,after_base64)!=0){
        LOGI(LOG_TAG.c_str(),"Base64 encode err..");
        return std::string();
    }
	std::string outURLData=std::string();
	if(SystemInterface::getSystemInterfaceInstance()->encodeURL(after_base64,outURLData)<0){
        LOGI(LOG_TAG.c_str(),"URL encode err..");
        return std::string();
    }

//Third: out post data
	std::string postContent;
	postContent+="p=";
	switch(dataType)
	{
		case DATA_TYPE_FAULT_LOGCAT:
			postContent+=outURLData;
			postContent+="&k=";
			postContent+= SystemBase::SEMANTIC_KEY_FAULT;
			break;
		case DATA_TYPE_MUSIC_INFO:
			postContent+=outURLData;
			postContent+="&k=";
			postContent+= SystemBase::SEMANTIC_KEY_MUSIC;
			break;
#ifdef ENABLE_CUSTOMID_UPLOAD
		case DATA_TYPE_CUSTOM_ID:
			postContent+=outURLData;
			postContent+="&k=";
			postContent+= SystemBase::SEMANTIC_KEY_CUSTOMID;
			break;
#endif
		default:
			postContent+=outURLData;
			postContent+="&k=";
			postContent+= SystemBase::SEMANTIC_KEY_TV;
			break;
	}
	return postContent;
}
void 
ASRProcess::ProcessASRResult(const std::string& asr_result,const std::string& wakeup_sessionid)
{
	
	if(!SystemStatusData::getSystemStatusDataInstance()->getWifiStatus()._connect_status){
		//wifi is off,play hint audio 
		return ;
	}
	http_request_data_t asr_request_data;
	memset(&asr_request_data,0,sizeof(http_request_data_t));
	asr_request_data._url=SEMANTIC_URL_PROJECT;
	asr_request_data._method=RequestMethod_POST;
	//First: creat post data
	asr_request_data._post_data=HttpPostDataCreate(asr_result.c_str(), DATA_TYPE_SEMANTIC_ANALYSIS);
	asr_request_data._header=std::string();
	asr_request_data._response_func=ASRProcess::semanticResponseCallBack;
	//Second: requst to hisense http server
	HttpClientInterface::getHttpClientInstance()->httpRequest(asr_request_data);
}
void 
ASRProcess::semanticResponseCallBack(const std::string& response,int response_code)
{
	LOGI(LOG_TAG.c_str(),">>>Response: %s<<<",response.c_str());
	if(response_code>=300){
		//TODO: add request err upload function
		LOGI(LOG_TAG.c_str(),"http response err!!\n");
		return;
	}
	std::string after_base64decode;
	LOGI(LOG_TAG.c_str(),"Base64 decode!!\n");
	//base64 decode 
	SystemInterface::getSystemInterfaceInstance()->base64Decode(response,after_base64decode);
	LOGI(LOG_TAG.c_str(),"AES Decrypt!!\n");
	//aes 
	std::string after_aesDecrypt;
	SystemInterface::getSystemInterfaceInstance()->aesDecrypt(after_base64decode,SystemBase::SEMANTIC_AES_KEY,after_aesDecrypt);
	LOGI(LOG_TAG.c_str(),"Parse Semantic=%s,\n",after_aesDecrypt.c_str());

	//add Semantic Parse
	ParseStatusFlags parse_status_flags;
	memset(&parse_status_flags,0,sizeof(ParseStatusFlags));
	std::string answer=SemanticParse::getSemanticParseInstance()->parseJsonStr(after_aesDecrypt,&parse_status_flags);
	//TODO: add tts play and function
}