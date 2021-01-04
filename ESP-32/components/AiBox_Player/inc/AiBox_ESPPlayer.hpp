/****************************************
*   ESP player .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#pragma once
#include <vector>
#include "esp_audio.h"
#include "esp_a2dp_api.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_mem.h"
#include "board.h"
#include "audio_common.h"
#include "audio_hal.h"
#include "filter_resample.h"
#include "fatfs_stream.h"
#include "raw_stream.h"
#include "i2s_stream.h"
#include "wav_decoder.h"
#include "wav_encoder.h"
#include "mp3_decoder.h"
#include "aac_decoder.h"
#include "http_stream.h"
#include "tone_stream.h"
#include "audio_error.h"
#include "flac_decoder.h"
#include "pcm_decoder.h"
#include "esp_decoder.h"
#include "recorder_engine.h"
#include "a2dp_stream.h"
#include "AiBox_BasePlayer.hpp"
#include "AiBox_BleESP.hpp"
#define LOCAL_PLAYER_TASK_SIZE 2*1024
#define PLAYER_STREAM_CAHCE_SIZE 512*1024
namespace AiBox
{
    typedef struct {
        esp_audio_handle_t     _handle;
    } esp_player_t;
    typedef struct{
        std::string _name;
        std::string _uri;
    }flash_tone_data_t; 
    class ESPPlayer: public BasePlayer
    {
        public:
            ESPPlayer();
            ~ESPPlayer(){}
            bool init()override;
            static bool resetPlayer(); /*call from system,donot need user call*/
            player_err_t stop()override;
            player_err_t pause()override;
            player_err_t resume()override;
            player_err_t play(const media_node_t& media_msg)override;
            player_err_t setVol(const int vol)override;
            player_err_t getVol(int& vol)override;
            player_err_t getCurrentPlayTime(int& time)override;
            player_err_t getCurrentPlayPos(int& pos)override;
            player_err_t getCurrentPlayerStatus(player_status_show_t& show_data)override;
            player_err_t getCurrentPlayerMediaInfo(media_node_t& media_msg)override;
            player_err_t getCurrentPlayerBackInfo(player_backup_msg_t& back_info)override;
            player_err_t setCurrentPlayerBackInfo(const player_backup_msg_t& back_info)override;
            player_err_t setPlayTime(int _target_time_sec)override;
            static void writeRawBuff(void* param);
            static void a2DPEventCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
            void execLocalPlayer();
            static esp_player_t* _esp_player;
            static audio_element_handle_t _raw_play_handle;
        private:
            bool setupPlayer();
            void registerPlayerCMD();
            int getToneMediaNum();
            std::string findToneMediaByName(const std::string& name);
            player_err_t playMedia(const std::string& meida_name,local_play_mode_t play_mode);
            player_err_t playMedia(const play_media_t& media_info);
            static const flash_tone_data_t _local_tone_media_list[];
            std::string _local_media_name;
            audio_element_handle_t _bt_stream_reader;
            StackType_t * _raw_player_task_stack=nullptr;
            StaticTask_t * _raw_player_tasktask_TCB=nullptr;
            static ESPPlayer* _singleton_instance;
    };
};