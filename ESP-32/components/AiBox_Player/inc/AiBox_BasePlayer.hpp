/****************************************
*   player base  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#pragma once
#include <string>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "AiBox_PlayerStatus.hpp"
#include "AiBox_SystemInterface.hpp"
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
    #include "esp_audio.h"
    typedef esp_audio_info_t player_backup_t;
#endif
#define TTS_STREAM_QUEUE_LENGTH 256
namespace AiBox
{
    typedef struct{
        player_backup_t _player_backup;
        media_node_t _media_info;
    }player_backup_msg_t;
    class BasePlayer
    {
        public:
            BasePlayer(){}
            ~BasePlayer(){}
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            virtual bool init()=0;
            virtual player_err_t stop()=0;
            virtual player_err_t pause()=0;
            virtual player_err_t resume()=0;
            virtual player_err_t play(const media_node_t& media_msg)=0;
            virtual player_err_t setVol(const int vol)=0;
            virtual player_err_t getVol(int& vol)=0;
            virtual player_err_t getCurrentPlayTime(int& time)=0;
            virtual player_err_t getCurrentPlayPos(int& pos)=0;
            virtual player_err_t getCurrentPlayerStatus(player_status_show_t& show_data)=0;
            virtual player_err_t getCurrentPlayerMediaInfo(media_node_t& media_msg)=0;
            virtual player_err_t getCurrentPlayerBackInfo(player_backup_msg_t& back_info)=0;
            virtual player_err_t setCurrentPlayerBackInfo(const player_backup_msg_t& back_info)=0;
            virtual player_err_t setPlayTime(int _target_time_sec)=0;
            void clearRawStreamQueue();
            /*current plaiing media msg*/
            media_node_t _media_info;
            QueueHandle_t _play_status_queue_handle=nullptr;
            QueueHandle_t _tts_stream_queue_handle=nullptr;
            /*local raw media,stop need report the finished status,so use the flag temporary*/
            bool is_local_media_stoped=false;
        protected:
            bool _initialized=false;
    };

    
};