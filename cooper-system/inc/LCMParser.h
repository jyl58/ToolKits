/****************************************
*   LCM Parser .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.3
*****************************************/
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include "lcm-cpp.hpp"
#include "wakeupThreshold/wakeup_threshold.hpp"
#include "wakeupStatus/wakeup_status.hpp" 
#include "ServerIP/ServerIP.hpp"
#include "DeviceInfo/DeviceInfo.hpp"
#include "mosquitto.h"
#include "mqtt_protocol.h"
#define COOPERSYSTEM_MAJOR 1
#define COOPERSYSTEM_MINOR 1
#define COOPERSYSTEM_REVISION 0
#define MASSAGE_INVLID_TIME 500  //ms
#define MAX_DEVICE 10
namespace AiBox{
	class LCMParser{
		public:
			LCMParser(const LCMParser&)=delete;
			LCMParser& operator=(const LCMParser&)=delete;
			~LCMParser();
			static std::shared_ptr<LCMParser> getLCMInstance();
			/*init the cooper systevoidm*/
			bool initLCMParser(const std::string& device_id,int deviceMode,int wakeup_time_ms=500);
			void resetLCMParser();
			/*answer policy*/
			typedef enum WakeUpPolicy{
				WAKEUP_POLICY_NONE=100,
				WAKEUP_POLICY_TIME,
				WAKEUP_POLICY_PRIORITY,
				WAKEUP_POLICY_NEAR
			}wakeup_policy_t;
			/*get the lcm init status*/
			bool isLCMInitFinished(){return _lcmParser_init_finished;}
			/*exec the cooper communication*/
			bool shouldACKWakeup(double threshold,wakeup_policy_t wakeup_policy,int method=0,int pub_times=3,int timeout_ms=200);
		private:
			LCMParser();
			/*device wakeup threshold publish struct*/
			typedef struct WakeUpThreshold{
				int64_t timestamp;
				std::string _deviceid;
				double _threshold;
			}wakeup_threshold_t;
			/*device wakeup status struct*/
			typedef struct WakeUpStatus{
				int64_t timestamp;
				std::string _deviceid;
				int8_t _status;
			}wakeup_status_t;
			typedef struct MqttServer{
				int64_t timestamp;
				std::string _deviceid;
				std::string _ip;
			}mqtt_server_t;
			typedef struct Device{
				int64_t timestamp;
				DeviceInfo::DeviceInfo device;
			}device_info_t;
			/*lcm callback function*/
			void handleWakeUpThresholdMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const wakeupThreshold::wakeup_threshold* msg);
			void handleWakeUpStatusMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const wakeupStatus::wakeup_status* msg);
			void handleServerIPMsg(const lcm::ReceiveBuffer* rbuf, const std::string& chan,const ServerIP::ServerIP* msg);
			void handleDeviceInfo(const lcm::ReceiveBuffer* rbuf, const std::string& chan,const DeviceInfo::DeviceInfo* msg);
			bool wakeupACKDecide(double threshold);
			bool hsOtherDeviceWakeUped();
			void clearParserBuffer();
			void uDPPublishWakeupStatus();
			void uDPPublishWakeupThreshold(double threshold);
			void uDPPubDeviceInfo();
			int  handleTimeout(int ms);
			void LCMHandle();
			//device info
			std::string _device_id;
			DeviceInfo::DeviceInfo _device_info;
			int _device_max_wakeup_time_ms=0;
			static std::shared_ptr<LCMParser> _singleton_instance;
			std::vector<wakeup_threshold_t> _wakeup_threshold_vector;
			std::vector<wakeup_status_t>_wakeup_status_vector;
			std::vector<device_info_t> _devices_infor_vector;
			static std::shared_ptr<lcm::LCM> _lcm_ptr;
			static std::string _udp_multicast_group;
			bool _lcmParser_init_finished{false};
			enum WakeUpPolicy _wakeup_policy{WAKEUP_POLICY_NONE};
			/*thread*/
			std::shared_ptr<std::thread> _udp_thread_ptr;
			bool _thread_should_exit;
			/*mqtt */
			mqtt_server_t _mqtt_server;
			struct mosquitto *_mosq = NULL;
			bool _mqtt_is_connected=false;
			std::string _username;
        	std::string _password;
			std::string _local_ip;
			std::string _topic_wakeup_threshold;
			std::string _topic_wakeup_status;
			std::mutex  _mutex;
			bool initMqtt();
			void mqttDisconnect();
			void mQTTpublishWakeupStatus();
        	void mQTTPublishWakeupThreshold(double threshold);
			bool getLocalIP();
			static void subscribeCallback(struct mosquitto* mosq, void* obj, int msg_id, int qos_count, const int* granted_qos);
        	static void connectCallback(struct mosquitto* mosq, void* obj, int response);
        	static void messageCallback(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg);
        	static void disconnectCallback(struct mosquitto *mosq, void *obj, int rc);
	};
};