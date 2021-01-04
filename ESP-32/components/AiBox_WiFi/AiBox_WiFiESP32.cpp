/****************************************
*   esp wifi  .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.02.28
*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_event_legacy.h"
#include "AiBox_WiFiESP32.hpp"
#include "esp_event_loop.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_sntp.h"
#include "nvs.h"
#include "lwip/dns.h"
using namespace AiBox;
const std::string LOG_TAG="ESPWiFi";
//wifi event call back function
esp_err_t 
wiFiEventHandler(void *param, system_event_t *event)
{
	ESPWiFiManage*wifi_manager=(ESPWiFiManage*)param;
	if(event==nullptr){
		ESP_LOGI(LOG_TAG.c_str(),"--wifi event is empty.");
		return ESP_FAIL;
	}
	bool need_send_wifi_status=false;
	switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
		ESP_LOGI(LOG_TAG.c_str(),"--WIFI start,next to connect WiFi.");
        esp_wifi_connect();
		//start success, info system for creat deviceid
		wifi_manager->_wifi_status._connect_status=false;
		wifi_manager->_wifi_status._ip=std::string();
		need_send_wifi_status=true;
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
		{
			ESP_LOGI(LOG_TAG.c_str(), "connect wifi the ip :%s",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			//send the status to system manage
			//step1: inform system status
			wifi_manager->_wifi_status._ip=ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip);
			need_send_wifi_status=true;
			//step2: set the connect event
			xEventGroupSetBits(wifi_manager->_wifi_event_group, CONNECTED_BIT);
			//step3: start the ping timer,for health check
			if(wifi_manager->_network_xTimers!=nullptr){
				//set first ping server index
				wifi_manager->_ping_server_index=0;
				//first connect wifi so ping fre is set 100ms
				xTimerChangePeriod(wifi_manager->_network_xTimers,100/portTICK_PERIOD_MS,0);
				xTimerStart( wifi_manager->_network_xTimers, 0 );
			}
			//step4: set backup dns server
			ip_addr_t backup_dns_sever;
			backup_dns_sever.type=IPADDR_TYPE_V4;
			backup_dns_sever.u_addr.ip4.addr=IPV4(114,114,114,114);
			dns_setserver(1, &backup_dns_sever);
			break;
		}
    case SYSTEM_EVENT_STA_DISCONNECTED:
        {
			ESP_LOGI(LOG_TAG.c_str(),"--Wifi Disconnected, so try connect agian.");
			//step1:stop the ping timer
			xTimerStop( wifi_manager->_network_xTimers, 0 );
			//step2:disconnect, so retry connect,again and angain
			xEventGroupClearBits(wifi_manager->_wifi_event_group, CONNECTED_BIT);
			//++ the connect fail count
			wifi_manager->_ap_connect_fail_count++;
			//if connect so disconnect first
			wifi_manager->reconnect();
			//step3: reset the systeminterface wifi status
			wifi_manager->_wifi_status._connect_status=false;
			//clear the ping status
			wifi_manager->_ping_success=false;
			wifi_manager->_ping_fail_count=0;
			wifi_manager->_wifi_status._ip=std::string(); // must use std::string(), do not use ""
			need_send_wifi_status=true;
			break;
        }
	case SYSTEM_EVENT_STA_CONNECTED:
		{
			wifi_manager->_ap_connect_fail_count=0;
			break;
		}
    default:
        break;
    }
	//check if send wifi status to system maintain
	if(need_send_wifi_status){
		//send wifi status to system maintain
		auto wifi_status_queue_handle=SystemInterface::getSystemInterfaceInstance()->getSystemWiFiStatusHandle();
		if(wifi_status_queue_handle!=nullptr){
			xQueueSend(wifi_status_queue_handle,&wifi_manager->_wifi_status,0);
		}else{
			ESP_LOGI(LOG_TAG.c_str(),"--System Wifi queue handle is empty!!");
		}
	}
    return ESP_OK;
}
/** Arguments used by 'join' function */
static struct {
    struct arg_int *timeout;
    struct arg_str *ssid;
    struct arg_str *password;
    struct arg_end *end;
} join_args;
/*wifi cmd*/
static int ifConfigCMD(int argc, char **argv)
{
	wifi_config_t wifi_config;
	memset(&wifi_config,0,sizeof(wifi_config_t));
	ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&wifi_config))
	wifi_status_t wifi_status;
	WiFiInterface::getWiFiInterfaceInstance()->getWiFiStatus(wifi_status);
	ESP_LOGI(LOG_TAG.c_str(),"AP:%s  MAC: %X:%X:%X:%X:%X:%X  IP:%s",wifi_config.sta.ssid,
																	wifi_status._mac[0],wifi_status._mac[1],wifi_status._mac[2],
																	wifi_status._mac[3],wifi_status._mac[4],wifi_status._mac[5],
																	wifi_status._ip.c_str());
	return 0;
}

static int connectCMD(int argc, char **argv)
{
	if(argc!=3){
		ESP_LOGI(LOG_TAG.c_str(),"Connect err: input the param cont of connect is err!");
		return 0;
	}
	//parse the input param
	arg_parse(argc, argv, (void **) &join_args);
	//get the input param
	wifi_connect_config_t connect_config;
	connect_config._ssid=join_args.ssid->sval[0];
	connect_config._passwd=join_args.password->sval[0];
	connect_config._timeout_ms=join_args.timeout->count==0?  5000 : join_args.timeout->ival[0];
	WiFiInterface::getWiFiInterfaceInstance()->wiFiConnect(connect_config);
    return 0;
}
static void pingDisplayCallback(ping_target_id_t found_id, esp_ping_found *found_val)
{
	ESP_LOGI(LOG_TAG.c_str(),"ping recv ip=%s time=%d err=%d",ESPWiFiManage::_singleton_instance->_ping_ip.c_str(),found_val->resp_time,found_val->ping_err? 0:1);
	if(found_val->send_count==ESPWiFiManage::_singleton_instance->_ping_times){
		//restart the ping check timer
		xTimerStart( ESPWiFiManage::_singleton_instance->_network_xTimers, 0 );
	}
}
static int pingCMD(int argc, char **argv)
{
	if(argc!=2){
		ESP_LOGI(LOG_TAG.c_str(),"ping err: need server param");
		return 0;
	}
	if(ESPWiFiManage::_singleton_instance->_wifi_status._ip.empty()){
		ESP_LOGI(LOG_TAG.c_str(),"ping err: Device's ip is empty !!");
		return 0;
	}
	//step1: stop the check timer
	xTimerStop( ESPWiFiManage::_singleton_instance->_network_xTimers, 0 );
	//step2: exec ping
	ESPWiFiManage::_singleton_instance->_ping_times=10; //set ping show times
	ESPWiFiManage::_singleton_instance->pingByHostName(argv[1],&pingDisplayCallback);
	return 0;
}
/*ESP wifi class*/
EventGroupHandle_t ESPWiFiManage::_wifi_event_group=nullptr;
ESPWiFiManage* ESPWiFiManage::_singleton_instance=nullptr;

bool 
ESPWiFiManage::init(const wifi_connect_config_t& connect_config)
{
	if(_wifi_driver_initialized){
		ESP_LOGI(LOG_TAG.c_str(),"WiFi driver already init\n");
		return true;
	}
	ESP_LOGI(LOG_TAG.c_str(),"Execute the ESP WiFi init\n");
	//step1: creat a wifi connect event
	_wifi_event_group = xEventGroupCreate();
	if(_wifi_event_group==nullptr){
		ESP_LOGI(LOG_TAG.c_str(),"Creat wifi event group err!!\n");
		return false;
	}
	//step2: creat a network check timer
	_network_xTimers=xTimerCreate( "NetworkTimer",100/portTICK_PERIOD_MS, pdFALSE, (void*)0, &ESPWiFiManage::networkTimerCallBack);
	if(_network_xTimers==nullptr){
		ESP_LOGI(LOG_TAG.c_str(),"--Creat WiFi network timer err!!!");
		return false;
	}
	//step3: start wifi
	ESP_ERROR_CHECK(esp_event_loop_init(&wiFiEventHandler, this));
	//wifi default config
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
	//init connect message 
	setWiFiConfig(connect_config);
    ESP_ERROR_CHECK(esp_wifi_start());
	esp_wifi_get_mac(ESP_IF_WIFI_STA, _wifi_status._mac);
	//register CMD
	registerWiFiCMD();
	_wifi_driver_initialized=true;
	return true; 
}
bool 
ESPWiFiManage::connect(const wifi_connect_config_t& connect_config)
{
	if(!_wifi_driver_initialized){
	 	ESP_LOGI(LOG_TAG.c_str(),"Connect err WiFi Do not init!!");
		return false;
	}
	//First: set Wifi Configure
	if(!setWiFiConfig(connect_config)){
		ESP_LOGI(LOG_TAG.c_str(),"Set Connect param err!!");
		return false;
	}
	//Second: need disconnect first
	ESP_ERROR_CHECK(esp_wifi_disconnect());
	//Third: start wifi 
    ESP_ERROR_CHECK(esp_wifi_connect());

	int bits = xEventGroupWaitBits(_wifi_event_group, CONNECTED_BIT,pdFALSE, pdTRUE, connect_config._timeout_ms / portTICK_PERIOD_MS);

	if((bits & CONNECTED_BIT)==0){
		ESP_LOGI(LOG_TAG.c_str(),"WiFi connect timeout");
		return false;
	}
	return true;
}
bool 
ESPWiFiManage::reconnect()
{
	//get the next wifi config try
	if(_ap_connect_fail_count>MAX_CONNECT_TRY_TIMES){
		wifi_connect_config_t wifi_connect_data;
		memset(&wifi_connect_data,0,sizeof(wifi_connect_config_t));
		wifi_connect_data._ssid=std::string();
		wifi_connect_data._passwd=std::string();
		wifi_connect_data._timeout_ms=5000; //5s
		_ap_wifi_config_index++;
		if(_ap_wifi_config_index>= SystemStatusData::getSystemStatusDataInstance()->getWifiConfigMaxNum()){
			_ap_wifi_config_index=0;	
		}
		SystemStatusData::getSystemStatusDataInstance()->getWifiConfig(wifi_connect_data,_ap_wifi_config_index);
		ESP_LOGI(LOG_TAG.c_str(),"Change ssid=%s to try reconnect",wifi_connect_data._ssid.c_str());
		setWiFiConfig(wifi_connect_data);
	}
	//Second: need disconnect first
	ESP_ERROR_CHECK(esp_wifi_disconnect());
	//Third: start wifi 
    ESP_ERROR_CHECK(esp_wifi_connect());
	int bits = xEventGroupWaitBits(_wifi_event_group, CONNECTED_BIT,pdFALSE, pdTRUE, 2000 / portTICK_PERIOD_MS);
	if((bits & CONNECTED_BIT)==0){
		ESP_LOGI(LOG_TAG.c_str(),"WiFi reconnect timeout");
		return false;
	}
	return true;
}
bool
ESPWiFiManage::setWiFiConfig(const wifi_connect_config_t& connect_config)
{
	//clear the wifi status
	_wifi_status._connect_config._ssid=connect_config._ssid;
	_wifi_status._connect_config._passwd=connect_config._passwd;
	_wifi_status._ip=std::string();
	_wifi_status._connect_status=false;
	//clear the fail count var
	_ap_connect_fail_count=0;
	wifi_config_t wifi_config;
	memset(&wifi_config,0,sizeof(wifi_config_t));
    if(_wifi_status._connect_config._ssid.empty()){
		ESP_LOGI(LOG_TAG.c_str(),"wifi ssid length is empty!!");
		return false;
	}
	if(_wifi_status._connect_config._ssid.size()>31){
		ESP_LOGI(LOG_TAG.c_str(),"wifi ssid length must <32!");
		return false;
	}else{
		memcpy(wifi_config.sta.ssid,_wifi_status._connect_config._ssid.c_str(),_wifi_status._connect_config._ssid.size()+1);
	}
	if(!_wifi_status._connect_config._passwd.empty()){
		if(_wifi_status._connect_config._passwd.size()>63){
			ESP_LOGI(LOG_TAG.c_str(),"wifi ssid length must <64!");
			return false;
		}else{
			memcpy(wifi_config.sta.password,_wifi_status._connect_config._passwd.c_str(),_wifi_status._connect_config._passwd.size()+1);
		}
	}
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	return true;
}
void 
ESPWiFiManage::pingByHostName(const std::string& host_name,esp_ping_found_fn ping_call_back)
{
	//get ip by host name
	ip_addr_t ip;
	if(dns_gethostbyname(host_name.c_str(),&ip,&ESPWiFiManage::dnsCallBack,(void*)ping_call_back)==ERR_OK){
		pingConfig(&ip,_ping_times,ping_call_back);
	}
}
void 
ESPWiFiManage::pingConfig(const ip_addr_t *ipaddr,int ping_time,esp_ping_found_fn ping_call_back)
{
	uint8_t ip1[4];
    uint32_t tem_ip=ipaddr->u_addr.ip4.addr;
    if(!_ping_ip.empty()){
		_ping_ip.clear();
	}
	for(int i=0;i<4;i++){
        ip1[i]=tem_ip&0x000000FF;
        tem_ip=tem_ip>>8;
		char temp[10];
		_ping_ip+=itoa(ip1[i],temp,10);
		if(i<3){
			_ping_ip+=".";	
		}
    }
	ESP_LOGI(LOG_TAG.c_str(),"ping server ip: %s\n",_ping_ip.c_str());
	//step1: set ping param
	esp_ping_set_target(PING_TARGET_IP_ADDRESS,(void*)&ipaddr->u_addr.ip4.addr,sizeof(uint32_t));
	uint32_t ping_count=ping_time;
	esp_ping_set_target(PING_TARGET_IP_ADDRESS_COUNT,(void*)&ping_count,sizeof(uint32_t));
	uint32_t ping_delay=1000; //1s timeout
	esp_ping_set_target(PING_TARGET_DELAY_TIME, (void*)&ping_delay, sizeof(ping_delay));
	uint32_t ping_timeout=1000;
	esp_ping_set_target(PING_TARGET_RCV_TIMEO, &ping_timeout, sizeof(ping_timeout));
	//set ping call back
	esp_ping_set_target(PING_TARGET_RES_FN,(void*)ping_call_back,4);
	//step2: run the ping
	ping_init();
}
void 
ESPWiFiManage::pingCallback(ping_target_id_t found_id, esp_ping_found *found_val)
{	
	//step1: check network status,and set the timer period,
	if(found_val->recv_count==_singleton_instance->_ping_times){
		//network form disconnect to connect,so set the check period to NETWORK_HEALTH_CHECK_PERIOD
		if(!_singleton_instance->_ping_success){
			xTimerChangePeriod(_singleton_instance->_network_xTimers,NETWORK_HEALTH_CHECK_PERIOD/portTICK_PERIOD_MS,0);
		}
		_singleton_instance->_ping_fail_count=0;
		_singleton_instance->_ping_success=true;
		_singleton_instance->_wifi_status._connect_status=_singleton_instance->_ping_success;
	}else{
		//step1:network from connect to disconnect,so set the check period to NETWORK_HEALTH_CHECK_PERIOD/2
		if(_singleton_instance->_ping_success){
			//set check period
			xTimerChangePeriod(_singleton_instance->_network_xTimers,(NETWORK_HEALTH_CHECK_PERIOD/2)/portTICK_PERIOD_MS,0);
		}
		//step2: set the wifi status
		_singleton_instance->_ping_fail_count++;
		_singleton_instance->_ping_success=false;
		if(_singleton_instance->_ping_fail_count>5){
			//ping fail count >5, set the wifi status to disconnect
			_singleton_instance->_wifi_status._connect_status=_singleton_instance->_ping_success;
		}
	}
	//step2: send status to system interface
	if( found_val->send_count==_singleton_instance->_ping_times&&
		found_val->ping_err==PING_RES_FINISH
	  ){
		//step1: send queue
		auto wifi_status_queue_handle=SystemInterface::getSystemInterfaceInstance()->getSystemWiFiStatusHandle();
		if(wifi_status_queue_handle!=nullptr){
			xQueueSend(wifi_status_queue_handle,&_singleton_instance->_wifi_status,0);
		}else{
			ESP_LOGI(LOG_TAG.c_str(),"--System Wifi queue handle is empty!!");
		}
		//step2:restart the check timer
		xTimerStart( _singleton_instance->_network_xTimers, 0 );
	}
}
void 
ESPWiFiManage::dnsCallBack(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
	if(ipaddr==NULL){
		ESP_LOGI(LOG_TAG.c_str(),"--DNS Do not find the host name: %s",name);
		//restart the check timer
		_singleton_instance->_ping_success=false;
		_singleton_instance->_ping_fail_count++;
		xTimerStart( _singleton_instance->_network_xTimers, 0);
		return;
	}
	_singleton_instance->pingConfig(ipaddr,_singleton_instance->_ping_times,(esp_ping_found_fn)callback_arg);
}
void 
ESPWiFiManage::networkTimerCallBack( TimerHandle_t xTimer )
{
	//step1:set ping time
	_singleton_instance->_ping_times=1;
	//step2:if first ping do not success ,so change ping server and try
	if(!_singleton_instance->_ping_success){
		_singleton_instance->_ping_server_index++;
		if(_singleton_instance->_ping_server_index > (sizeof(_singleton_instance->_ping_server_list)/sizeof(_singleton_instance->_ping_server_list[0])-1)){
			_singleton_instance->_ping_server_index=0;
		}
	}
	//step3:check ping ip or host name
	if(_singleton_instance->_ping_server_list[_singleton_instance->_ping_server_index]._type==ServerType_HOSTNAME){
		_singleton_instance->pingByHostName(_singleton_instance->_ping_server_list[_singleton_instance->_ping_server_index]._host_name,&ESPWiFiManage::pingCallback);
	}else if(_singleton_instance->_ping_server_list[_singleton_instance->_ping_server_index]._type==ServerType_IP){
		ip_addr_t ip;
		ip.u_addr.ip4.addr=_singleton_instance->_ping_server_list[_singleton_instance->_ping_server_index]._ip;
		_singleton_instance->pingConfig(&ip,_singleton_instance->_ping_times,&ESPWiFiManage::pingCallback);
	}
}
void 
ESPWiFiManage::registerWiFiCMD()
{
	//ifconfig cmd
	esp_console_cmd_t join_cmd;
    join_cmd.command = "ifconfig";
    join_cmd.help = "print the ip and mac";
    join_cmd.hint = NULL;
    join_cmd.func = &ifConfigCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
	//connect cmd
	join_args.timeout = arg_int0(NULL, "timeout", "<t>", "Connection timeout, ms");
    join_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID of AP");
    join_args.password = arg_str0(NULL, NULL, "<pass>", "PSK of AP");
    join_args.end = arg_end(2);

	esp_console_cmd_t join_cmd2;
    join_cmd2.command = "connect";
    join_cmd2.help = "connect to AP,EX: connect ssid passwd";
    join_cmd2.hint = NULL;
    join_cmd2.func = &connectCMD;
    join_cmd2.argtable=&join_args;
	ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd2));

	esp_console_cmd_t join_cmd3;
    join_cmd3.command = "ping";
    join_cmd3.help = "ping the target server,ex: ping xxxxx.com";
    join_cmd3.hint = NULL;
    join_cmd3.func = &pingCMD;
    join_cmd3.argtable=NULL;
	ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd3));

	esp_console_cmd_t join_cmd4;
    join_cmd4.command = "wificonfig";
    join_cmd4.help = "print the wifi config in flash";
    join_cmd4.hint = NULL;
    join_cmd4.func = &wiFiConfigCMD;
    join_cmd4.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd4));
}