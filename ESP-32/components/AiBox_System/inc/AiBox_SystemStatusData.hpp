/****************************************
*   system status data for app .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.09
*****************************************/
#pragma once
#include <string>
#include "AiBox_WiFiData.hpp"
#include "AiBox_BleData.hpp"
#include "AiBox_PlayerStatus.hpp"
#include "AiBox_SystemInterface.hpp"
#define JHK_FEATURE_CODE "861020000001003000000618"
#define WIFI_ID          "86100c000001001000000618"
namespace AiBox{
    class SystemInterface;
    class SystemStatusData{
        public:
            //friend class for system interface to write the data to this data
            friend SystemInterface;
            SystemStatusData();
            SystemStatusData(const SystemStatusData& system_status)=delete;
            void operator=(const SystemStatusData& system_status)=delete;
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            ~SystemStatusData(){}
            static SystemStatusData* getSystemStatusDataInstance();
            const wifi_status_t& getWifiStatus(){return _wifi;}
            bool  getWifiConfig(wifi_connect_config_t& wifi_connect_config,int prio=0);
            uint8_t getWifiConfigMaxNum(){return sizeof(_wifi_flash_data._wifi_config)/sizeof(_wifi_flash_data._wifi_config[0]);}
            const ble_status_t& getBlueStatus(){return _ble;}
            const player_status_show_t& getPlyerStatus(){return _player;}
            const std::string& getJHKDeviceID(){return _JHKDeviceID;}
            const std::string& getWifiID(){return _wifiID;}
            const std::string& getACDeviceID(){return _acDeviceID;}
            const std::string& getRequestVersion(){return _curRequestVersion;}
            const std::string& getWakeUpKey(){return _wakeupKey;}
            const std::string& getTvFeatureCode(){return _curTvFeatureCode;}
            const std::string& getSubscriberID(){return _curSubscriberID;}
            const std::string& getPackageVersion(){return _PackageVersion;}
            const std::string& getCustomerID(){return _curCustomerID;}
            const std::string& getJHLDeviceID(){return _localJHLDeviceID;}
            const std::string getPosition(){return _latitude+_longitude;}
            const std::string& getJHKToken(){return _jhkToken;}
            bool getChildModeState(){return isChildMode;}

        private:
            wifi_status_t _wifi;
            wifi_flash_data_t _wifi_flash_data;
            ble_status_t _ble;
            player_status_show_t _player;
            std::string _JHKDeviceID{std::string()};
            std::string _localJHLDeviceID{std::string()};
            std::string _curRequestVersion{std::string()};
            std::string _PackageVersion{std::string()};
            std::string _curSubscriberID{std::string()};
            std::string _curCustomerID{std::string()};
            std::string _jhkToken{std::string()};
            std::string _curTvFeatureCode{std::string()};
            std::string _wifiID{std::string()};
            std::string _acDeviceID{std::string()};
            //use to make sure which wakeup key
            std::string _wakeupKey{std::string()};
            //geo 
            std::string _latitude{std::string()};
            std::string _longitude{std::string()};
            short _curPlayStatus;
            short _curTTSStatus;
            short _prePlayStatus;
            int _curVolume;
            bool _isMicOn;
            bool _isWifiOn;
            bool _isUpdating;
            bool _isBlueToothOn;
            bool _isTvPartner;
            bool _isNoRecord;
            bool _isPowerOn;
            bool isMultirounds{false};
            bool isChildMode{false};

            static SystemStatusData* _singleton_instance;
    };
};