/****************************************
*   AiBox player status .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#pragma once
#include <functional>
namespace AiBox
{
    typedef enum {
        PLAYER_STATUS_UNKNOWN    = 0,
        PLAYER_STATUS_RUNNING    = 1,
        PLAYER_STATUS_PAUSED     = 2,
        PLAYER_STATUS_STOPPED    = 3,
        PLAYER_STATUS_FINISHED   = 4,
        PLAYER_STATUS_ERROR      = 5,
    } player_status_t;
    typedef enum {
        MEDIA_TYPE_NULL          = 0,
        MEDIA_TYPE_MUSIC_BASE    = 0x100,
        MEDIA_TYPE_MUSIC_SD      = MEDIA_TYPE_MUSIC_BASE + 1,
        MEDIA_TYPE_MUSIC_HTTP    = MEDIA_TYPE_MUSIC_BASE + 2,
        MEDIA_TYPE_MUSIC_FLASH   = MEDIA_TYPE_MUSIC_BASE + 3,
        MEDIA_TYPE_MUSIC_A2DP    = MEDIA_TYPE_MUSIC_BASE + 4,
        MEDIA_TYPE_MUSIC_DLNA    = MEDIA_TYPE_MUSIC_BASE + 5,
        MEDIA_TYPE_MUSIC_RAW     = MEDIA_TYPE_MUSIC_BASE + 6,
        MEDIA_TYPE_MUSIC_TTS     = MEDIA_TYPE_MUSIC_BASE + 7, //jyl
        MEDIA_TYPE_MUSIC_MAX     = 0x1FF,

        MEDIA_TYPE_TONE_BASE     = 0x200,
        MEDIA_TYPE_TONE_SD       = MEDIA_TYPE_TONE_BASE + 1,
        MEDIA_TYPE_TONE_HTTP     = MEDIA_TYPE_TONE_BASE + 2,
        MEDIA_TYPE_TONE_FLASH    = MEDIA_TYPE_TONE_BASE + 3,
        MEDIA_TYPE_TONE_MAX      = 0x2FF,

        MEDIA_TYPE_RESERVE_BASE  = 0x800,
        MEDIA_TYPE_RESERVE_MAX   = 0xFFF,
    } media_source_t;
#define ERR_AUDIO_BASE                  (0x80000 + 0x1000) /*!< Starting number of ESP audio error codes */
    typedef enum {
        ERR_AUDIO_NO_ERROR                  = 0,
        ERR_AUDIO_FAIL                      = -1,

        /* esp audio errors */
        ERR_AUDIO_NO_INPUT_STREAM           = ERR_AUDIO_BASE + 1,  // 0x0x81001
        ERR_AUDIO_NO_OUTPUT_STREAM          = ERR_AUDIO_BASE + 2,
        ERR_AUDIO_NO_CODEC                  = ERR_AUDIO_BASE + 3,
        ERR_AUDIO_HAL_FAIL                  = ERR_AUDIO_BASE + 4,
        ERR_AUDIO_MEMORY_LACK               = ERR_AUDIO_BASE + 5,
        ERR_AUDIO_INVALID_URI               = ERR_AUDIO_BASE + 6,
        ERR_AUDIO_INVALID_PATH              = ERR_AUDIO_BASE + 7,
        ERR_AUDIO_INVALID_PARAMETER         = ERR_AUDIO_BASE + 8,
        ERR_AUDIO_NOT_READY                 = ERR_AUDIO_BASE + 9,
        ERR_AUDIO_NOT_SUPPORT               = ERR_AUDIO_BASE + 10,
        ERR_AUDIO_TIMEOUT                   = ERR_AUDIO_BASE + 11,
        ERR_AUDIO_ALREADY_EXISTS            = ERR_AUDIO_BASE + 12,
        ERR_AUDIO_LINK_FAIL                 = ERR_AUDIO_BASE + 13,
        ERR_AUDIO_UNKNOWN                   = ERR_AUDIO_BASE + 14,
        ERR_AUDIO_OUT_OF_RANGE              = ERR_AUDIO_BASE + 15,

        ERR_AUDIO_OPEN                      = ERR_AUDIO_BASE + 0x100,// 0x81100
        ERR_AUDIO_INPUT                     = ERR_AUDIO_BASE + 0x101,
        ERR_AUDIO_PROCESS                   = ERR_AUDIO_BASE + 0x102,
        ERR_AUDIO_OUTPUT                    = ERR_AUDIO_BASE + 0x103,
        ERR_AUDIO_CLOSE                     = ERR_AUDIO_BASE + 0x104,

        ERR_AUDIO_LAST_STATUS_ERR             = ERR_AUDIO_BASE + 0x300, //
    } player_err_t;

    typedef enum{
        LOCAL_PLAY_MODE_BLOCK,
        LOCAL_PLAY_MODE_UNBLOCK
    }local_play_mode_t;
    //call back function param
    typedef struct {
        player_status_t     _status;         /*!< Status of esp_audio*/
        media_source_t      _media_src;      /*!< Media source type*/
        player_err_t        _err_msg;
    } audio_state_t;
    //play messge 
    typedef struct{
        std::string _uri;
        int _start_pos;     //pos, unusable now
        int _start_time;
        media_source_t _media_type;
    }play_media_t; 
    //show the player status on cli
    typedef struct{
        player_status_t     _status;
        media_source_t      _media_src;
        int _vol;
        int _time;
        int _pos;
        int _total_time;
    }player_status_show_t;
    //user callback
    using PlayEndCallback = std::function<void (const player_status_show_t&)>;
    typedef enum Business{
        Business_Type_NONE=0,
        Business_Type_ALARM,
        Business_Type_WAKEUP
    }Business_type_t;
    typedef struct MediaNode{
        play_media_t _media_info;
        bool _not_allow_auto_resume;                //if true,not allow auto resume by prev media
        bool _not_allow_seek;
        Business_type_t  _business_type;
        PlayEndCallback _func;
    }media_node_t;
    typedef struct TTSStream{
        char* data;
        int length;
    }tts_stream_ptr_t;
};
