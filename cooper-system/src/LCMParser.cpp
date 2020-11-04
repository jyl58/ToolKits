/****************************************
*   LCM Parser .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.03
*****************************************/
#include <iostream>
#include <time.h> 
#include <fstream>
#include <sys/time.h>
#include <time.h> 
#include <unistd.h>
#include <netdb.h> /* struct hostent */
#include <arpa/inet.h> /* inet_ntop */
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include "LCMParser.h"
#include "cJSON.h"
#include <algorithm>
#include <chrono>
using namespace AiBox;
#ifndef BUILD_VERSION
#define BUILD_VERSION 
#endif 
#define CURRENT_TIME std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define CURRENT_TIME_MS std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define DEBUGE_WAKEUP 1
std::string LCMParser::_udp_multicast_group{"udpm://224.0.0.88:8876?ttl=1"};
std::shared_ptr<lcm::LCM> LCMParser::_lcm_ptr;
std::shared_ptr<LCMParser> LCMParser::_singleton_instance;

std::shared_ptr<LCMParser> LCMParser::getLCMInstance()
{
	if(_singleton_instance.get()==nullptr){
		_singleton_instance=std::shared_ptr<LCMParser>(new LCMParser());
	}
	return _singleton_instance;
}
LCMParser::LCMParser()
:_topic_wakeup_threshold("/MQTT_WAKEUP_TH/&"),
_topic_wakeup_status("/MQTT_WAKEUP_ST/&")
{
}
LCMParser::~LCMParser()
{
	resetLCMParser();
}
bool 
LCMParser::initLCMParser(const std::string& device_id,int deviceMode,int wakeup_time_ms)
{
	if(device_id.empty()){
		std::cout<<"LCM Parser need a unique device id!!"<<std::endl;
		return false;
	}
	_device_id=device_id;
	//creat a lcm ptr
	if(_lcm_ptr.get()!=nullptr){
		_lcm_ptr.reset();  //free the memory
	}
	_lcm_ptr=std::make_shared<lcm::LCM>(_udp_multicast_group);
#ifdef DEBUG
	std::cout<<"*************CooperSystem Info*************"<<std::endl;
    std::cout<<"version: "<<COOPERSYSTEM_MAJOR<<"."<<COOPERSYSTEM_MINOR<<"."<<COOPERSYSTEM_REVISION<<std::endl;
	std::cout<<"build time: "<<BUILD_VERSION<<std::endl;
	std::cout<<"address:"<<_udp_multicast_group<<std::endl;
	std::cout<<"*******************************************"<<std::endl;
#endif	
	if(_lcm_ptr.get()==nullptr){
#ifdef DEBUG
        std::cout<<"LCM creat err!!"<<std::endl;
#endif
		return false;		
	}
	if(_singleton_instance.get()==nullptr){
#ifdef DEBUG
        std::cout<<"LCM instance ptr is nullptr!!"<<std::endl;
#endif 
		return false;
	}
	if(!_lcm_ptr->good()){
#ifdef DEBUG
        std::cout<<"LCM init err!!"<<std::endl;
#endif 		
		return false;
	}
	//topic
	_lcm_ptr->subscribe("WAKEUP_TH", &LCMParser::handleWakeUpThresholdMsg, _singleton_instance.get());
	_lcm_ptr->subscribe("WAKEUP_ST", &LCMParser::handleWakeUpStatusMsg, _singleton_instance.get());
	_lcm_ptr->subscribe("SERVER_IP", &LCMParser::handleServerIPMsg,_singleton_instance.get());
	_lcm_ptr->subscribe("DEVICE_INFO",&LCMParser::handleDeviceInfo,_singleton_instance.get());
	//init the random 
    std::srand((unsigned)time(NULL));

	_thread_should_exit=false;
	_udp_thread_ptr=std::make_shared<std::thread>(&LCMParser::LCMHandle,this);
	if(_udp_thread_ptr.get()==nullptr){
#ifdef DEBUG
        std::cout<<"LCM thread creat err!!"<<std::endl;
#endif
		return false;
	}
	_lcmParser_init_finished=true;
	_device_info.wakeupTime=wakeup_time_ms;
	_device_info.uniqueId=0;
	_device_info.deviceID=_device_id;
	_device_info.deviceMode=deviceMode;
	return true;
}
void 
LCMParser::resetLCMParser()
{
	if(_udp_thread_ptr.get()!=nullptr){
		_thread_should_exit=true;
		_udp_thread_ptr->join();
		_udp_thread_ptr.reset();
		_udp_thread_ptr=nullptr;
	}
	if(_lcm_ptr.get()!=nullptr){
		_lcm_ptr.reset();
		_lcm_ptr=nullptr;
	}
	_lcmParser_init_finished=false;
	mqttDisconnect();
}
void 
LCMParser::LCMHandle()
{
	std::cout<<"Run LCM thread."<<std::endl;
	int pub_count=0;
	while(!_thread_should_exit){
		pub_count++;
		if(pub_count>=100){
			//pub device info
			pub_count=0;
			uDPPubDeviceInfo();
		}
		if(handleTimeout(100)<0){
			std::cout<<"LCM handle err."<<std::endl;
		}
	}
	std::cout<<"Exit LCM handle thread."<<std::endl;
}
bool 
LCMParser::shouldACKWakeup(double threshold,wakeup_policy_t wakeup_policy,int method,int pub_times,int timeout_ms)
{
	clearParserBuffer();
	int send_time=pub_times<1? 1: pub_times;
	int waiting_time=timeout_ms+(_device_info.wakeupTime >=  _device_max_wakeup_time_ms? 0 :_device_max_wakeup_time_ms-_device_info.wakeupTime);
	std::cout<<"All waiting time"<<waiting_time<<std::endl;
	_wakeup_policy=wakeup_policy;
	//step1: do cooper
	uint8_t send_count=0;
	while(send_count<send_time){
		//step1: rand sleep 
		int rand_sleep_time=std::rand()%15+5;//(5,20) rand sleep ms
		usleep(rand_sleep_time*1000);//sleep (5,20)ms
		//step2: public the threshold
		if(method==0){
			uDPPublishWakeupThreshold(threshold);
		}else if(method==1){
			mQTTPublishWakeupThreshold(threshold);
		}else{
			uDPPublishWakeupThreshold(threshold);
			mQTTPublishWakeupThreshold(threshold);
		}
		//step3: waiting some ms to receive message
		usleep((waiting_time/send_time-rand_sleep_time)*1000);
		send_count++;
	}
#ifdef DEBUGE_WAKEUP
    //record the wakeup msg
	std::ofstream wakeup_log_out("/userdata/wake_snr.txt",std::ios::app);
	wakeup_log_out<<CURRENT_TIME<<":M Y ID:="<<_device_id <<";SNR="<<threshold<<std::endl;
	std::vector<wakeup_threshold_t>::iterator maxelement;
	if(!_wakeup_threshold_vector.empty()){
		for(auto& device:_wakeup_threshold_vector){
			wakeup_log_out<<"           "<<"RCV ID:="<<device._deviceid<<";SNR="<<device._threshold<<std::endl;
		}
		maxelement=std::max_element(_wakeup_threshold_vector.begin(),_wakeup_threshold_vector.end(),[](const wakeup_threshold_t& first,const wakeup_threshold_t& last){
				return first._threshold<last._threshold;
			});
		if(threshold<(*maxelement)._threshold){
			wakeup_log_out<<"           "<<"MAX ID:="<<(*maxelement)._deviceid<<std::endl;
		}else{
			wakeup_log_out<<"           "<<"MAX ID:="<<_device_id<<std::endl;
		}
	}else{
		wakeup_log_out<<"           "<<"MAX ID:="<<_device_id<<std::endl;
	}
	
#endif
	//step2: check wether has other device already wake up
	if(hsOtherDeviceWakeUped()){
#ifdef DEBUGE_WAKEUP		
		for(auto &other_wakeuped: _wakeup_status_vector){
			if(other_wakeuped._status==1){
				wakeup_log_out<<"           "<<"ACK ID:="<<other_wakeuped._deviceid<<std::endl;
			}
		}
		wakeup_log_out.close();
#endif
		return false;
	}
	//step3: check wether this device need anwser? 
	bool ret=wakeupACKDecide(threshold);
	if(ret){
#ifdef DEBUGE_WAKEUP
		wakeup_log_out<<"           "<<"ACK ID:="<<_device_id<<std::endl;
		wakeup_log_out.close();
#endif
		int send_status_count=0;
		while(send_status_count<2){
			int sleep_time=std::rand()%5+5;//(5,10) rand sleep ms
			usleep(sleep_time*1000);//sleep (5,10)ms	
			if(method==0){
				uDPPublishWakeupStatus();
			}else if(method==1){
				mQTTpublishWakeupStatus();
			}else{
				uDPPublishWakeupStatus();
				mQTTpublishWakeupStatus();
			}
			send_status_count++;
		}
	}else{
#ifdef DEBUGE_WAKEUP
		wakeup_log_out<<"           "<<"ACK ID:="<<(*maxelement)._deviceid<<std::endl;
		wakeup_log_out.close();
#endif
	}
	return ret;
}
int
LCMParser::handleTimeout(int timeout_ms)
{
	return _lcm_ptr->handleTimeout(timeout_ms);
}
bool 
LCMParser::wakeupACKDecide(double threshold)
{
	std::lock_guard<std::mutex> slock(_mutex);
	if(_wakeup_policy==WAKEUP_POLICY_NEAR){
		for(auto &other_threshold: _wakeup_threshold_vector){
			if(other_threshold._threshold > threshold){
				return false;
			}
		}
	}else if(_wakeup_policy==WAKEUP_POLICY_PRIORITY){
		for(auto &other_threshold: _wakeup_threshold_vector){
			if(other_threshold._threshold < threshold){
				return false;
			}
		}
	}else if(_wakeup_policy==WAKEUP_POLICY_TIME){
		for(auto &other_threshold: _wakeup_threshold_vector){
			if(other_threshold._threshold < threshold){
				return false;
			}
		}
	}else{
		return false;
	}
	return true;
}

bool 
LCMParser::hsOtherDeviceWakeUped()
{
	std::lock_guard<std::mutex> slock(_mutex);
	for(auto &other_wakeuped: _wakeup_status_vector){
		if(other_wakeuped._status==1){
			return true;
		}
	}
	return false;
}
void 
LCMParser::clearParserBuffer()
{
	std::lock_guard<std::mutex> slock(_mutex);
	_wakeup_threshold_vector.clear();
	_wakeup_status_vector.clear();
}
void 
LCMParser::handleWakeUpThresholdMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const wakeupThreshold::wakeup_threshold* msg)
{
	//std::cout<<"ddddddddddddddddd:"<<std::string((char*)rbuf->data,rbuf->data_size)<<":"<<std::endl;
	int64_t ms_from_unix_epoch=CURRENT_TIME_MS;
	bool already_record_deviceid;
	//check the udp data timeliness
	if(ms_from_unix_epoch-0.001*rbuf->recv_utime<MASSAGE_INVLID_TIME){
		int64_t    tmp_timestamp=msg->timestamp;
		for(int i=0; i<10; i++){
			already_record_deviceid=false;
			//exclude this dev message
			if((!msg->deviceId[i].empty())&&(_device_id.compare(msg->deviceId[i])!=0)){
				//get the all other message
				{
					std::lock_guard<std::mutex> slock(_mutex);
					for(auto &other_threshold: _wakeup_threshold_vector){
						if(other_threshold._deviceid.compare(msg->deviceId[i])==0){
							already_record_deviceid=true;
						}
					}
					if(!already_record_deviceid){
						wakeup_threshold_t temp_threshold;
						temp_threshold.timestamp=(int64_t)ms_from_unix_epoch;//ms
						temp_threshold._threshold=msg->threshold[i];
						temp_threshold._deviceid=msg->deviceId[i];	
						_wakeup_threshold_vector.push_back(temp_threshold);
	#ifdef DEBUG		
						std::cout<<"Received udp deviceId: "<<temp_threshold._deviceid<<" threshold: "<<temp_threshold._threshold<<std::endl;
	#endif
					}
				}
			}
		}
	}else{
#ifdef DEBUG
		std::cout<<"Delete old wakeup threshold ,time difference: "<<(ms_from_unix_epoch-0.001*rbuf->recv_utime)<<" ms"<<std::endl;
		std::cout<<"Receive Msg: device id="<<msg->deviceId[0]<<";threshold="<<msg->threshold[0]<<std::endl;
#endif
	}
}
void 
LCMParser::handleWakeUpStatusMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const wakeupStatus::wakeup_status* msg)
{
	double ms_from_unix_epoch=CURRENT_TIME_MS;
	//check the udp data timeliness
	if(ms_from_unix_epoch-0.001*rbuf->recv_utime<MASSAGE_INVLID_TIME){// <1.5s
		if((!msg->deviceId.empty())&&(_device_id.compare(msg->deviceId)!=0)){
			wakeup_status_t temp_status;
			temp_status.timestamp=(int64_t)ms_from_unix_epoch;//ms
			temp_status._deviceid=msg->deviceId;
			temp_status._status=msg->status;
			{
				std::lock_guard<std::mutex> slock(_mutex);
				_wakeup_status_vector.push_back(temp_status);
			}
#ifdef DEBUG
			std::cout<<"Receive udp deviceID: "<<temp_status._deviceid<<" status: "<<(int)msg->status<<std::endl;
#endif
		}
	}else{
#ifdef DEBUG
		std::cout<<"Delete old wakeup status ,time difference: "<<(ms_from_unix_epoch-0.001*rbuf->recv_utime)<<" ms"<<std::endl;
		std::cout<<"Receive  Msg: device id="<<msg->deviceId<<";status="<<(int)msg->status<<std::endl;
#endif
	}
}
void 
LCMParser::handleServerIPMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan,const ServerIP::ServerIP* msg)
{
	if(_mqtt_server._deviceid.compare(msg->deviceid)==0){
		return;
	}
#ifdef DEBUG
	std::cout<<"Receive New Mqtt Server deviceid="<<msg->deviceid<<"; ip="<<msg->ip<<std::endl;
#endif
	if(_mosq!=NULL){
		mqttDisconnect();
	}
	_mqtt_server._ip=msg->ip;
	_mqtt_server._deviceid=msg->deviceid;
	if(!initMqtt()){
		std::cout<<"Init mqtt err."<<std::endl;
		//need reconnect
		_mqtt_server._ip.clear();
		_mqtt_server._deviceid.clear();
	}
}
void
LCMParser::handleDeviceInfo(const lcm::ReceiveBuffer* rbuf, const std::string& chan,const DeviceInfo::DeviceInfo* msg)
{
	//update
	auto elem=std::find_if(_devices_infor_vector.begin(), _devices_infor_vector.end(), [msg](const device_info_t& item){
			return msg->deviceID.compare(item.device.deviceID)==0;
		});
	if(elem==_devices_infor_vector.end()){
		device_info_t device_info;
		device_info.timestamp=CURRENT_TIME;
		device_info.device.wakeupTime=msg->wakeupTime;
		device_info.device.uniqueId=msg->uniqueId;
		device_info.device.deviceMode=msg->deviceMode;
		device_info.device.deviceID=msg->deviceID;
		_devices_infor_vector.push_back(device_info);
	}else{
		elem->device.wakeupTime=msg->wakeupTime;
		elem->device.uniqueId=msg->uniqueId;
		elem->timestamp=CURRENT_TIME;
	}

	//delete old message
	std::remove_if(_devices_infor_vector.begin(), _devices_infor_vector.end(), [](const device_info_t& item){
			return CURRENT_TIME-item.timestamp<10;  //delete the >20s
		});

	//find the max wakeup time
	int wakeup_time_ms=std::max_element(_devices_infor_vector.begin(), _devices_infor_vector.end(), [](const device_info_t& first,const device_info_t& last){
			return first.device.wakeupTime<last.device.wakeupTime;
		})->device.wakeupTime;
	if(_device_max_wakeup_time_ms!=wakeup_time_ms){
		_device_max_wakeup_time_ms=wakeup_time_ms;
		std::cout<<"Max wakeup time change to : "<<_device_max_wakeup_time_ms<<std::endl;
	}
}
void 
LCMParser::uDPPublishWakeupStatus()
{
	wakeupStatus::wakeup_status _wakeup_status;
	_wakeup_status.timestamp=CURRENT_TIME_MS;
	_wakeup_status.status=1;
	_wakeup_status.deviceId=_device_id;
	_lcm_ptr->publish("WAKEUP_ST", &_wakeup_status);
#ifdef DEBUG
	std::cout<<"Send wakeup status : "<<"device="<<_device_id <<";Status="<<(int)_wakeup_status.status<<std::endl;
#endif
}
void 
LCMParser::uDPPublishWakeupThreshold(double threshold)
{
	wakeupThreshold::wakeup_threshold _wakeup_threshold;
	_wakeup_threshold.timestamp=CURRENT_TIME_MS;
	_wakeup_threshold.threshold[0]=threshold;
	_wakeup_threshold.deviceId[0]=_device_id;
	std::string debug_msg=std::string();
	for(int i=0;i<MAX_DEVICE;i++){
		if(_wakeup_threshold_vector.size()>i){
			auto &threshold=_wakeup_threshold_vector[i];
			if(!threshold._deviceid.empty()){
				_wakeup_threshold.threshold[i+1]=threshold._threshold;
				_wakeup_threshold.deviceId[i+1]=threshold._deviceid;
#ifdef DEBUG
				debug_msg+="device="+threshold._deviceid+";threshold="+std::to_string(threshold._threshold);
#endif
			}
		}else{
			break;
		}
	}
	_lcm_ptr->publish("WAKEUP_TH", &_wakeup_threshold);
#ifdef DEBUG
	std::cout<<"send node count:"<<_wakeup_threshold_vector.size()+1<<";device="<<_device_id<<";threshold="<<threshold<<debug_msg<<std::endl;
#endif
}
void 
LCMParser::uDPPubDeviceInfo()
{
	_lcm_ptr->publish("DEVICE_INFO", &_device_info);
}
/*mqtt*/
void 
LCMParser::subscribeCallback(struct mosquitto* mosq, void* obj, int msg_id, int qos_count, const int* granted_qos)
{
	std::cout<<"Mqtt subscribe success."<<std::endl;
}
void 
LCMParser::connectCallback(struct mosquitto* mosq, void* obj, int response)
{
	if(response==0){
        std::cout<<"Mqtt connect success."<<std::endl;
        //sub the topic
        int rc=mosquitto_subscribe(mosq,NULL,"/MQTT_WAKEUP_TH/#",2);
        if(rc!=MOSQ_ERR_SUCCESS){
            std::cout<<"Mqtt sub topic"<<"/MQTT_WAKEUP_TH/#"<<" err."<<std::endl;
        }
        rc=mosquitto_subscribe(mosq,NULL,"/MQTT_WAKEUP_ST/#",2);
        if(rc!=MOSQ_ERR_SUCCESS){
            std::cout<<"Mqtt sub topic"<<"/MQTT_WAKEUP_ST/#"<<" err."<<std::endl;
        }
        _singleton_instance->_mqtt_is_connected=true;
    }else{
        std::cout<<"Mqtt connect refused,code: "<<response<<std::endl;
    }
}
void 
LCMParser::messageCallback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg)
{
    std::string payload_str=std::string((char*)msg->payload,msg->payloadlen);
    cJSON* payload_json=cJSON_Parse(payload_str.c_str());
    if(payload_json==NULL){
        std::cout<<"Parse payload err."<<std::endl;
        return;
    }
    cJSON* deviceid_json=cJSON_GetObjectItem(payload_json,"deviceid");
    if(deviceid_json==NULL){
        std::cout<<"Parse device id err."<<std::endl;
        cJSON_Delete(payload_json);
        return;
    }
    std::string device_id=std::string(deviceid_json->valuestring);
    if(device_id.compare(_singleton_instance->_device_id)==0){
        cJSON_Delete(payload_json);
        return;
    }
    int64_t  now=CURRENT_TIME_MS; //ms
    if(_singleton_instance->_topic_wakeup_threshold.compare(msg->topic)==0){
        cJSON* wakeup_th_json=cJSON_GetObjectItem(payload_json,"threshold");
        if(wakeup_th_json==NULL){
            std::cout<<"Parse wakeup threshold err."<<std::endl;
            cJSON_Delete(payload_json);
            return;
        }
        {
            std::lock_guard<std::mutex> slock(_singleton_instance->_mutex);
            for(auto &wakeuped_th: _singleton_instance->_wakeup_threshold_vector){
                if(wakeuped_th._deviceid.compare(device_id)==0){
                    cJSON_Delete(payload_json);
                    return;
                }
            }
            std::cout<<"MQTT time: "<<now<<",device="<<device_id<<",threshold="<<wakeup_th_json->valuedouble<<std::endl;
            wakeup_threshold_t temp_th{now,device_id,wakeup_th_json->valuedouble};
            _singleton_instance->_wakeup_threshold_vector.push_back(temp_th);
        }
    }else if(_singleton_instance->_topic_wakeup_status.compare(msg->topic)==0){
        cJSON* wakeup_st_json=cJSON_GetObjectItem(payload_json,"status");
        if(wakeup_st_json==NULL){
            std::cout<<"Parse wakeup status err."<<std::endl;
            cJSON_Delete(payload_json);
            return;
        }
        
        {
            std::lock_guard<std::mutex> slock(_singleton_instance->_mutex);
            for(auto &wakeuped_th: _singleton_instance->_wakeup_status_vector){
                if(wakeuped_th._deviceid.compare(device_id)==0){
                    cJSON_Delete(payload_json);
                    return;
                }
            }
            std::cout<<"MQTT time: "<<now<<",device="<<device_id<<",status="<<wakeup_st_json->valueint<<std::endl;
            wakeup_status_t temp_st{now,device_id,(int8_t)wakeup_st_json->valueint};
           _singleton_instance->_wakeup_status_vector.push_back(temp_st);
        }
    }
    cJSON_Delete(payload_json);
    return;	
}
void 
LCMParser::disconnectCallback(struct mosquitto *mosq, void *obj, int rc)
{
	if(rc!=0){
		//reconnect if sever lost
		mosquitto_reconnect_async(mosq);
	}
	_singleton_instance->_mqtt_is_connected=false;
    std::cout<<"Mqtt disconnected,code: "<<rc<<std::endl;
}
void 
LCMParser::mqttDisconnect()
{
	if(_mosq){
		_mqtt_is_connected=false;
		mosquitto_loop_stop(_mosq,true);
		mosquitto_destroy(_mosq);
		mosquitto_lib_cleanup();
		_mosq=NULL;
	}
	_mqtt_server._deviceid.clear();
	_mqtt_server._ip.clear();
	_mqtt_server.timestamp=0;
}
bool 
LCMParser::initMqtt()
{
	//step1: get local ip
    if(!getLocalIP()){
        std::cout<<"Local IP err."<<std::endl;
        return false;
    }
 #ifdef DEBUG
    std::cout<<"Local IP :"<<_local_ip<<std::endl;
 #endif 
	//step3: init the mqtt client
    mosquitto_lib_init();
    _mosq=mosquitto_new(_device_id.c_str(), true, nullptr);
    if(!_mosq){
        std::cout<<"Error:Creat a new mosquitto err."<<std::endl;
        return false;
    }
    mosquitto_subscribe_callback_set(_mosq, &LCMParser::subscribeCallback);
	mosquitto_connect_callback_set(_mosq, &LCMParser::connectCallback);
	mosquitto_message_callback_set(_mosq, &LCMParser::messageCallback);
    mosquitto_disconnect_callback_set(_mosq,&LCMParser::disconnectCallback);
    //set the mqtt version
    mosquitto_int_option(_mosq, MOSQ_OPT_PROTOCOL_VERSION,MQTT_PROTOCOL_V311);
	//set the reconnect delay 2s, max is 20s
	mosquitto_reconnect_delay_set(_mosq,2,20,false);
    if((!_username.empty() ||!_password.empty()) && mosquitto_username_pw_set(_mosq, _username.c_str(), _password.c_str())){
		std::cout<<"Error: Problem setting username and/or password."<<std::endl;
		mosquitto_lib_cleanup();
		return false;
	}
    //connect 
    int rc = mosquitto_connect_bind(_mosq, _mqtt_server._ip.c_str(), 36699, 30,_local_ip.c_str());
    if(rc!=MOSQ_ERR_SUCCESS){
        std::cout<<"Mqtt connect bind err,code: "<<rc<<std::endl;
        mosquitto_lib_cleanup();
        return false;
    }
    if(mosquitto_loop_start(_mosq)!=MOSQ_ERR_SUCCESS){
        std::cout<<"Mqtt start err,code:"<<std::endl;
        mosquitto_destroy(_mosq);
        mosquitto_lib_cleanup();
        return false;
    }
    return true;
}

void 
LCMParser::mQTTpublishWakeupStatus()
{
    std::string wakeup_st_json="{\"deviceid\":\""+ _device_id+"\",\"status\":1}";
    if(_mqtt_is_connected&&_mosq!=NULL){
        int ret=mosquitto_publish(_mosq,NULL,_topic_wakeup_status.c_str(),wakeup_st_json.size()+1,(void*)wakeup_st_json.c_str(),2,false);
        if(ret!=MOSQ_ERR_SUCCESS){
            std::cout<<"Publish wakeup status err."<<std::endl;
        }
    }else{
        std::cout<<"Mqtt isn't connect."<<std::endl;
    }
}
void 
LCMParser::mQTTPublishWakeupThreshold(double threshold)
{
    std::string wakeup_th_json="{\"deviceid\":\""+ _device_id+"\",\"threshold\":"+std::to_string(threshold) +"}";
    if(_mqtt_is_connected&&_mosq!=NULL){
        int ret=mosquitto_publish(_mosq,NULL,_topic_wakeup_threshold.c_str(),wakeup_th_json.size()+1,(void*)wakeup_th_json.c_str(),2,false);
        if(ret!=MOSQ_ERR_SUCCESS){
            std::cout<<"Publish wakeup threshold err."<<std::endl;
        }
    }else{
        std::cout<<"Mqtt isn't connect."<<std::endl;
    }
}
bool 
LCMParser::getLocalIP()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
 
    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct==NULL){
        return false;
    }
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
        if (!ifa->ifa_addr){
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) // check it is IP4
		{
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            _local_ip=addressBuffer;
            if(_local_ip.compare("127.0.0.1")!=0){
                return true;   
            }
        }
    }
	freeifaddrs(ifAddrStruct);
    return false;
}