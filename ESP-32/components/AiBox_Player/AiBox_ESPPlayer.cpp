 /****************************************
*   ESP player .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#include "esp_log.h"
#include "esp_console.h"
#include "AiBox_ESPPlayer.hpp"
#include "AiBox_BoardESP.hpp"
#include "AiBox_PlayerInterface.hpp"
#include "AiBox_PlayManage.hpp"
#include "esp_http_client.h"
using namespace AiBox;
const static std::string LOG_TAG="ESPPlayer";

const flash_tone_data_t ESPPlayer::_local_tone_media_list[]={
    {"wakeup","flash://tone/0_WakeUpWoZai.mp3"},
    {"test2","flash://tone/1_unlinked.mp3"}
};
static int playerCMD(int argc, char **argv)
{
    if(argc<2){
        ESP_LOGI(LOG_TAG.c_str(), "player cmd need a subcmd!!");
        return 0;
    }
    if(std::string(argv[1]).compare("start")==0){
        if(argc < 4){
            ESP_LOGI(LOG_TAG.c_str(), "start subcmd need at least 2 param!!");
            return 0;
        }
        int start_time=0;
        if(argc==5){
            start_time=atoi(argv[4]);//s
        }
        media_node_t media_node;
        memset(&media_node,0,sizeof(media_node_t));
		media_node._media_info._media_type=(media_source_t)atoi(argv[2]);
		media_node._media_info._uri=argv[3];
        media_node._media_info._start_pos=0;
        media_node._not_allow_seek=media_node._media_info._media_type==MEDIA_TYPE_MUSIC_HTTP? false: true;
        media_node._media_info._start_time=start_time;
		media_node._func=nullptr;
		MediaVehicle::getMediaVehicleInstance()->setMediaData(media_node);
		media_vehicle_data_t media_vehicle;
		media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
		auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
		if(media_queue_handle!=nullptr){
			xQueueSend(media_queue_handle,&media_vehicle,0);
        }else{
            ESP_LOGI(LOG_TAG.c_str(), "Play Manage queue is empty!!");
        }
    }else if(std::string(argv[1]).compare("stop")==0){
        PlayerInterface::getPlayerInterfaceInstance()->stop();
    }else if(std::string(argv[1]).compare("resume")==0){
        PlayerInterface::getPlayerInterfaceInstance()->resume();
    }else if(std::string(argv[1]).compare("pause")==0){
        PlayerInterface::getPlayerInterfaceInstance()->pause();
    }else if(std::string(argv[1]).compare("vol")==0){
        if(argc!=3){
            ESP_LOGI(LOG_TAG.c_str(), "vol subcmd need a param!!");
            return 0;
        }
        int vol= atoi(argv[2]);
        PlayManage::getPlayManageInstance()->setVol(vol);
    }else if(std::string(argv[1]).compare("time")==0){
       if(argc!=3){
            ESP_LOGI(LOG_TAG.c_str(), "time subcmd need a param!!");
            return 0;
        }
        int time=atoi(argv[2]);
        PlayManage::getPlayManageInstance()->seek(time);
    }else if(std::string(argv[1]).compare("status")==0){
        player_status_show_t show_data;
        PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerStatus(show_data);
        ESP_LOGI(LOG_TAG.c_str(), "Player current status: status=%d,meida=%d,vol=%d,pos=%d,total time=%d,time=%d\n",
                                    show_data._status,
                                    show_data._media_src,
                                    show_data._vol,
                                    show_data._pos,
                                    show_data._total_time,
                                    show_data._time);
    }
    return 0;
}
extern "C" void espAudioPlayerCallback(esp_audio_state_t *state, void *ctx)
{
    ESP_LOGI(LOG_TAG.c_str(), "player callback, status:%d, err_msg:%d, src:%d",state->status, state->err_msg, state->media_src);
    ESPPlayer* esp_player_tmp=(ESPPlayer*)ctx;
    player_status_show_t player_show_data;
    esp_player_tmp->getCurrentPlayerStatus(player_show_data);
    //step2: handle the player flag
    switch(state->status){
        case AUDIO_STATUS_RUNNING:
            break;
        case AUDIO_STATUS_PAUSED:{
            }
            break;
        case AUDIO_STATUS_STOPPED:
        case AUDIO_STATUS_FINISHED:
            // if http player need delete the heap
            break;
        case AUDIO_STATUS_ERROR:
            /*player occur err status*/
            break;
        default :
            break;
    }
    
    //send player status to system main task
    auto player_status_queue_handle=SystemInterface::getSystemInterfaceInstance()->getSystemPlayerStatusHandle();
    if(player_status_queue_handle!=nullptr){
        xQueueSend(player_status_queue_handle,&player_show_data,0);
    }else{
        ESP_LOGI(LOG_TAG.c_str(),"Get system player status handle err!!!");
    }
}
#define HTTP_SWP_CACHE_SIZE 2*1024
static int httpStreamEventHandle(http_stream_event_msg_t *msg)
{
    ESPPlayer* esp_player_tmp=(ESPPlayer*)msg->user_data;
    ESP_LOGI(LOG_TAG.c_str(), "http stream, event_id=%d,len:%d",msg->event_id,msg->buffer_len);
    return ESP_OK;
}

audio_element_handle_t ESPPlayer::_raw_play_handle=nullptr;
esp_player_t* ESPPlayer::_esp_player=nullptr;
ESPPlayer* ESPPlayer::_singleton_instance=nullptr;
ESPPlayer::ESPPlayer()
:BasePlayer()
{
    memset(&_media_info,0,sizeof(media_node_t));
    _singleton_instance=this;
}
bool 
ESPPlayer::setupPlayer()
{
    if(ESPBoard::_board_handle==nullptr){
        ESP_LOGW(LOG_TAG.c_str(), "board handle is nullptr!!");
        return false;
    }
    //step1:Create player
    esp_audio_cfg_t cfg = DEFAULT_ESP_AUDIO_CONFIG();
    audio_board_handle_t board_handle =ESPBoard::_board_handle;
    cfg.in_stream_buf_size=PLAYER_STREAM_CAHCE_SIZE;
    cfg.task_prio = 15;
    cfg.vol_handle = board_handle->audio_hal;
    cfg.vol_set = (audio_volume_set)audio_hal_set_volume;
    cfg.vol_get = (audio_volume_get)audio_hal_get_volume;
    cfg.resample_rate = 48000;
    cfg.prefer_type = ESP_AUDIO_PREFER_MEM;
    cfg.cb_func =espAudioPlayerCallback;
    cfg.cb_ctx = (void*)this;
    cfg.evt_que = nullptr;
    _esp_player->_handle=esp_audio_create(&cfg);

    //step3:Create readers and add to esp_audio
    tone_stream_cfg_t tn_reader;
    memset(&tn_reader,0,sizeof(tone_stream_cfg_t));
    tn_reader.task_prio = TONE_STREAM_TASK_PRIO;
    tn_reader.task_core = TONE_STREAM_TASK_CORE;
    tn_reader.task_stack = TONE_STREAM_TASK_STACK;
    tn_reader.out_rb_size = TONE_STREAM_RINGBUFFER_SIZE;
    tn_reader.buf_sz = TONE_STREAM_BUF_SIZE;
    tn_reader.type = AUDIO_STREAM_READER;
    int ret=esp_audio_input_stream_add(_esp_player->_handle, tone_stream_init(&tn_reader));
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init tone err: %s!!",esp_err_to_name(ret));
        return false;
    }
    //step2: add stream to audio 
    raw_stream_cfg_t raw_cfg ;
    memset(&raw_cfg,0,sizeof(raw_stream_cfg_t));
    raw_cfg.out_rb_size = RAW_STREAM_RINGBUFFER_SIZE;
    raw_cfg.type = AUDIO_STREAM_WRITER;
    _raw_play_handle = raw_stream_init(&raw_cfg);
    ret=esp_audio_input_stream_add(_esp_player->_handle, _raw_play_handle);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init raw play err: %s!!",esp_err_to_name(ret));
        return false;
    }
    http_stream_cfg_t http_cfg;
    memset(&http_cfg,0,sizeof(http_stream_cfg_t));
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.task_prio = HTTP_STREAM_TASK_PRIO;
    http_cfg.task_core = HTTP_STREAM_TASK_CORE;
    http_cfg.task_stack = HTTP_STREAM_TASK_STACK;
    http_cfg.stack_in_ext =true;
    http_cfg.out_rb_size = HTTP_STREAM_RINGBUFFER_SIZE;
    http_cfg.multi_out_num = 0;
    http_cfg.event_handle = httpStreamEventHandle;
    http_cfg.user_data=(void*)this;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    http_cfg.auto_connect_next_track= true;
    audio_element_handle_t http_stream_reader = http_stream_init(&http_cfg);
    ret=esp_audio_input_stream_add(_esp_player->_handle, http_stream_reader);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init http stream  err: %s!!",esp_err_to_name(ret));
        return false;
    }

    /*bt stream*/
    ESP_LOGI(LOG_TAG.c_str(), "init Get Bluetooth A2DP stream");
    a2dp_stream_config_t a2dp_config;
    memset(&a2dp_config,0,sizeof(a2dp_stream_config_t));
    a2dp_config.type = AUDIO_STREAM_READER;
    a2dp_config.user_callback.user_a2d_cb=ESPPlayer::a2DPEventCallback;
    //a2dp_config.user_callback.user_a2d_sink_data_cb=ESPPlayer::a2DPSinkDataCallback;
    _bt_stream_reader= a2dp_stream_init(&a2dp_config);
    ret=esp_audio_input_stream_add(_esp_player->_handle, _bt_stream_reader);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init BT stream  err: %s!!",esp_err_to_name(ret));
        return false;
    }
    // step3:decoder,Add decoders and encoders to esp_audio
    wav_decoder_cfg_t wav_dec_cfg = DEFAULT_WAV_DECODER_CONFIG();
    wav_dec_cfg.task_prio = 12;
    wav_dec_cfg.task_core = 1;
    ret=esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, wav_decoder_init(&wav_dec_cfg));
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init wav decoder err: %s!!",esp_err_to_name(ret));
        return false;
    }
    mp3_decoder_cfg_t mp3_dec_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_dec_cfg.task_core = 1;
    mp3_dec_cfg.task_prio = 12;
    ret=esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, mp3_decoder_init(&mp3_dec_cfg));
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init mp3 decoder err: %s!!",esp_err_to_name(ret));
        return false;
    }
    
    pcm_decoder_cfg_t pcm_dec_cfg = DEFAULT_PCM_DECODER_CONFIG();
    esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, pcm_decoder_init(&pcm_dec_cfg));

    audio_decoder_t auto_decode[] = {
        DEFAULT_ESP_AMRNB_DECODER_CONFIG(),
        DEFAULT_ESP_AMRWB_DECODER_CONFIG(),
        DEFAULT_ESP_MP3_DECODER_CONFIG(),
        DEFAULT_ESP_WAV_DECODER_CONFIG(),
        DEFAULT_ESP_AAC_DECODER_CONFIG(),
        DEFAULT_ESP_M4A_DECODER_CONFIG(),
        DEFAULT_ESP_TS_DECODER_CONFIG(),
        DEFAULT_ESP_PCM_DECODER_CONFIG(),
    };
    esp_decoder_cfg_t auto_dec_cfg = DEFAULT_ESP_DECODER_CONFIG();
    auto_dec_cfg.task_core=1;
    auto_dec_cfg.task_prio=12;
    esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, esp_decoder_init(&auto_dec_cfg, auto_decode, sizeof(auto_decode) / sizeof(audio_decoder_t)));
    
    aac_decoder_cfg_t aac_cfg = DEFAULT_AAC_DECODER_CONFIG();
    aac_cfg.task_core = 1;
    aac_cfg.task_prio = 12;
    ret=esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, aac_decoder_init(&aac_cfg));
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init aac decoder err: %s!!",esp_err_to_name(ret));
        return false;
    }
    audio_element_handle_t m4a_dec_cfg = aac_decoder_init(&aac_cfg);
    ret= audio_element_set_tag(m4a_dec_cfg, "m4a");
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "m4a Init decoder err: %s!!",esp_err_to_name(ret));
        return false;
    }
    ret=esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, m4a_dec_cfg);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init m4a decoder err: %s!!",esp_err_to_name(ret));
        return false;
    }

    audio_element_handle_t ts_dec_cfg = aac_decoder_init(&aac_cfg);
    ret=audio_element_set_tag(ts_dec_cfg, "ts");
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Ts init err: %s!!",esp_err_to_name(ret));
        return false;
    }
    ret=esp_audio_codec_lib_add(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, ts_dec_cfg);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init ts codec err: %s!!",esp_err_to_name(ret));
        return false;
    }

    // step4: out stream, Create writers and add to esp_audio
    i2s_stream_cfg_t _i2s_writer;
    memset(&_i2s_writer,0,sizeof(i2s_stream_cfg_t));
    _i2s_writer.type = AUDIO_STREAM_WRITER;                                          
    _i2s_writer.task_prio = I2S_STREAM_TASK_PRIO;                                          
    _i2s_writer.task_core = I2S_STREAM_TASK_CORE;                                        
    _i2s_writer.task_stack = I2S_STREAM_TASK_STACK;                                  
    _i2s_writer.out_rb_size = I2S_STREAM_RINGBUFFER_SIZE;
    _i2s_writer.stack_in_ext =true;                                                                        
    _i2s_writer.i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX);                    
    _i2s_writer.i2s_config.sample_rate = 44100;                                                  
    _i2s_writer.i2s_config.bits_per_sample = (i2s_bits_per_sample_t)16;                                                
    _i2s_writer.i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;                           
    _i2s_writer.i2s_config.communication_format = I2S_COMM_FORMAT_I2S;                           
    _i2s_writer.i2s_config.dma_buf_count = 3;                            
    _i2s_writer.i2s_config.dma_buf_len = 300;                                                     
    _i2s_writer.i2s_config.use_apll = 1;                                                         
    _i2s_writer.i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2;                               
    _i2s_writer.i2s_config.tx_desc_auto_clear = true;                                                                                                       
    _i2s_writer.i2s_port = (i2s_port_t)0;                                      
    _i2s_writer.use_alc = false;                                                           
    _i2s_writer.volume = 0;                                                           
    _i2s_writer.multi_out_num = 0;                                                         
    _i2s_writer.uninstall_drv = true;                                  
    _i2s_writer.i2s_config.sample_rate = 48000;
    audio_element_handle_t  i2s_stream_writer=i2s_stream_init(&_i2s_writer);
    ret= esp_audio_output_stream_add(_esp_player->_handle,i2s_stream_writer);
    if(ret!=ESP_ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "Init IIS err: %s!!",esp_err_to_name(ret));
        return false;
    }

    // Set default volume
    if(esp_audio_vol_set(_esp_player->_handle, 30)!=ESP_ERR_AUDIO_NO_ERROR){
        return false;
    }
    //stp5: set the player status queue handle
    esp_audio_event_que_set(_esp_player->_handle,_play_status_queue_handle);
    AUDIO_MEM_SHOW(LOG_TAG.c_str());
    ESP_LOGI(LOG_TAG.c_str(), "esp_audio instance is:%p", _esp_player->_handle);
    return true;
}
bool 
ESPPlayer::init()
{
    ESP_LOGI(LOG_TAG.c_str(),"Execute the ESP player init\n");
    //step1: creat a player status queue 
    _play_status_queue_handle = xQueueCreate(1, sizeof(esp_audio_state_t));
    if(_play_status_queue_handle==nullptr){
        ESP_LOGW(LOG_TAG.c_str(), "Creat status queue handle err!!");
        return false;
    }
    //step2: new player memory
    _esp_player = (esp_player_t*)audio_calloc(1, sizeof(esp_player_t));
    if(_esp_player==nullptr){
        ESP_LOGI(LOG_TAG.c_str(), "Calloc player mem err!!!");
        return false;
    }
    //step3: setup player
    if(!setupPlayer()){
        ESP_LOGI(LOG_TAG.c_str(), "Setup player err!!!");
        return false;
    }
    //step4: creat the tts stream queue
    _tts_stream_queue_handle=xQueueCreate(TTS_STREAM_QUEUE_LENGTH,sizeof(tts_stream_ptr_t));
    if(_tts_stream_queue_handle==NULL){
        ESP_LOGI(LOG_TAG.c_str(), "Creat tts stream err!!!");
        return false;
    }
    //raw player task stack
    _raw_player_task_stack=(StackType_t *)sysMemMalloc(LOCAL_PLAYER_TASK_SIZE);
    _raw_player_tasktask_TCB=(StaticTask_t *) audio_calloc_inner(1, sizeof(StaticTask_t));
    //step5: register player
    registerPlayerCMD();
    _initialized=true;
    return true;
}
bool 
ESPPlayer::resetPlayer()
{
    //step1: destory the player
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ESP_ERR_AUDIO_MEMORY_LACK);
    esp_audio_destroy(_esp_player->_handle);
    //step2: setup player
    if(!_singleton_instance->setupPlayer()){
        ESP_LOGI(LOG_TAG.c_str(), "Resetup player err!!!");
        return false;
    }
    _singleton_instance->stop();
    return true;
}
player_err_t
ESPPlayer::stop()
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    //step1: clear the queue cache
    esp_audio_state_t player_current_status;
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    if (uxQueueSpacesAvailable(_play_status_queue_handle)==0) {
        xQueueReceive(_play_status_queue_handle, &player_current_status, 0);
        ESP_LOGW(LOG_TAG.c_str(),"Clear the play statue queue");
    }
    //step2:check the last status
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    int ret =esp_audio_state_get(_esp_player->_handle, &player_current_status);
    switch(player_current_status.status){
        case AUDIO_STATUS_RUNNING:
        case AUDIO_STATUS_PAUSED:
            break;
        case AUDIO_STATUS_STOPPED:
        case AUDIO_STATUS_FINISHED:
        case AUDIO_STATUS_UNKNOWN:
            return ERR_AUDIO_NO_ERROR;
        case AUDIO_STATUS_ERROR:
            ESP_LOGI(LOG_TAG.c_str(), "stop cmd :Player status is err!!");
            return ERR_AUDIO_FAIL;
        default :
            break;
    }
    //step3:check the media src is raw or bt
    if(player_current_status.media_src==MEDIA_SRC_TYPE_MUSIC_RAW){
        audio_element_set_ringbuf_done(_raw_play_handle);
        audio_element_finish_state(_raw_play_handle);
        is_local_media_stoped=true;
    }else if (player_current_status.media_src == MEDIA_SRC_TYPE_MUSIC_A2DP) {
        audio_element_set_ringbuf_done(_bt_stream_reader);
        audio_element_stop(_bt_stream_reader);
        periph_bt_stop(ESPBleManage::_bt_periph);
    }
    ESP_LOGI(LOG_TAG.c_str(), "stop player");
    //step4: stop the player
    ret |= esp_audio_stop(_esp_player->_handle, TERMINATION_TYPE_NOW);
    return (player_err_t)ret;
}
player_err_t
ESPPlayer::pause()
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    //step1: clear the queue cache
    esp_audio_state_t player_current_status;
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    if (uxQueueSpacesAvailable(_play_status_queue_handle)==0) {
        xQueueReceive(_play_status_queue_handle, &player_current_status, 0);
        ESP_LOGW(LOG_TAG.c_str(),"Clear the play statue queue");
    }
    int ret = ESP_ERR_AUDIO_NO_ERROR;
    //step2:check the last status
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    ret =esp_audio_state_get(_esp_player->_handle, &player_current_status);
    switch(player_current_status.status){
        case AUDIO_STATUS_RUNNING:
            break;
        case AUDIO_STATUS_PAUSED:
            return ERR_AUDIO_NO_ERROR;
        case AUDIO_STATUS_STOPPED:
        case AUDIO_STATUS_FINISHED:
        case AUDIO_STATUS_UNKNOWN:
            ESP_LOGI(LOG_TAG.c_str(), "pause cmd :last status is finished or stop status");
            return ERR_AUDIO_LAST_STATUS_ERR ;
        case AUDIO_STATUS_ERROR:
            ESP_LOGI(LOG_TAG.c_str(), "pause cmd :player status is err!!");
            return ERR_AUDIO_FAIL;
        default :
            break;
    }
    //step3: pause the player
    if (player_current_status.media_src == MEDIA_SRC_TYPE_MUSIC_A2DP) {
        periph_bt_stop(ESPBleManage::_bt_periph);
    }
    ret |=esp_audio_pause(_esp_player->_handle);
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    int timeout=0;
    while (esp_audio_state_get(_esp_player->_handle, &player_current_status)==ESP_ERR_AUDIO_NO_ERROR){
        if(player_current_status.status == AUDIO_STATUS_PAUSED||timeout>10){
            break;
        }else{
            ESP_LOGI(LOG_TAG.c_str(), "%s expected running state, recv:%d", __func__, player_current_status.status);
        }
        timeout++;
        //sleep 50ms
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    return (player_err_t)ret;
}
player_err_t
ESPPlayer::resume()
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    //step1: clear the queue cache
    esp_audio_state_t player_current_status;
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    if (uxQueueSpacesAvailable(_play_status_queue_handle)==0){
        xQueueReceive(_play_status_queue_handle, &player_current_status, 0);
        ESP_LOGW(LOG_TAG.c_str(),"Clear the play statue queue");
    }
    int ret = ESP_ERR_AUDIO_NO_ERROR;
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    ret |=esp_audio_state_get(_esp_player->_handle, &player_current_status);
    switch(player_current_status.status){
        case AUDIO_STATUS_RUNNING:
            return ERR_AUDIO_NO_ERROR;
        case AUDIO_STATUS_PAUSED:
            break;
        case AUDIO_STATUS_STOPPED:
        case AUDIO_STATUS_FINISHED:
        case AUDIO_STATUS_UNKNOWN:
            ESP_LOGI(LOG_TAG.c_str(), "Resume cmd :last status is finished or stop!!");
            return ERR_AUDIO_LAST_STATUS_ERR ;
        case AUDIO_STATUS_ERROR:
            ESP_LOGI(LOG_TAG.c_str(), "Resume cmd :last status is err!!");
            return ERR_AUDIO_FAIL;
        default :
            break;
    }
    if (player_current_status.media_src == MEDIA_SRC_TYPE_MUSIC_A2DP) {
        ret|=periph_bt_play(ESPBleManage::_bt_periph);
    }
    ret |= esp_audio_resume(_esp_player->_handle);
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    int timeout=0;
    while (esp_audio_state_get(_esp_player->_handle, &player_current_status)==ESP_ERR_AUDIO_NO_ERROR) {
        if (player_current_status.status == AUDIO_STATUS_RUNNING||timeout>10) {
            break;
        }else{
            ESP_LOGI(LOG_TAG.c_str(), "%s expected running state, recv:%d", __func__, player_current_status.status);
        }
        //sleep 50ms
        timeout++;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::setVol(const int vol)
{
    if(_esp_player->_handle==nullptr){
        ESP_LOGI(LOG_TAG.c_str(), "player handler is nullptr!!");
        return ERR_AUDIO_FAIL;
    }
    return (player_err_t)esp_audio_vol_set(_esp_player->_handle, vol);
}
player_err_t 
ESPPlayer::getVol(int& vol)
{
    if(_esp_player->_handle==nullptr){
        ESP_LOGI(LOG_TAG.c_str(), "player handler is nullptr!!");
        return ERR_AUDIO_FAIL;
    }
    return (player_err_t)esp_audio_vol_get(_esp_player->_handle, &vol);
}
player_err_t 
ESPPlayer::getCurrentPlayTime(int& time)
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    return (player_err_t)esp_audio_time_get(_esp_player->_handle, &time);
}
player_err_t 
ESPPlayer::setPlayTime(int target_time_sec)
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    esp_audio_state_t audio_state;
    memset(&audio_state,0,sizeof(esp_audio_state_t));
    int ret=esp_audio_state_get(ESPPlayer::_esp_player->_handle,&audio_state);
    if( audio_state.media_src!=MEDIA_SRC_TYPE_MUSIC_HTTP){ 
        ESP_LOGW(LOG_TAG.c_str(),"Only Http src support seek!!");
        return (player_err_t)ret;
    }
    ret |=esp_audio_seek(_esp_player->_handle, target_time_sec);
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::getCurrentPlayPos(int& pos)
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    return (player_err_t)esp_audio_pos_get(_esp_player->_handle, &pos);
}
player_err_t 
ESPPlayer::getCurrentPlayerStatus(player_status_show_t& show_data)
{
    esp_audio_state_t audio_data;
    memset(&audio_data,0,sizeof(esp_audio_state_t));
    int ret=esp_audio_state_get(ESPPlayer::_esp_player->_handle,&audio_data);
    show_data._media_src=(media_source_t)audio_data.media_src;
    // case the tts or http
    if(_media_info._media_info._media_type==MEDIA_TYPE_MUSIC_TTS){
        show_data._media_src=MEDIA_TYPE_MUSIC_TTS;
    }
    show_data._status=(player_status_t)audio_data.status;
    ret |= esp_audio_pos_get(ESPPlayer::_esp_player->_handle, &show_data._pos);
    ret |= esp_audio_vol_get(ESPPlayer::_esp_player->_handle, &show_data._vol);
    ret |= esp_audio_time_get(ESPPlayer::_esp_player->_handle, &show_data._time);
    ret |= esp_audio_duration_get(ESPPlayer::_esp_player->_handle,&show_data._total_time);
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::getCurrentPlayerMediaInfo(media_node_t& media_msg)
{
    media_msg=_media_info;
    return getCurrentPlayTime(_media_info._media_info._start_pos);
}
player_err_t 
ESPPlayer::getCurrentPlayerBackInfo(player_backup_msg_t& back_info)
{
    //step1:get the media info
    back_info._media_info=_media_info;
    //step2: get the player info
    esp_audio_prefer_t type;
    esp_audio_prefer_type_get(_esp_player->_handle, &type);
    esp_audio_state_t st;
    memset(&st,0,sizeof(esp_audio_state_t));
    int ret=esp_audio_state_get(_esp_player->_handle, &st);
    if (type == ESP_AUDIO_PREFER_MEM) {
        if ((st.status == AUDIO_STATUS_RUNNING)|| (st.status == AUDIO_STATUS_PAUSED)){
            ret|=esp_audio_info_get(_esp_player->_handle,&back_info._player_backup);
        }
    }else if(type == ESP_AUDIO_PREFER_SPEED){
        ESP_LOGW(LOG_TAG.c_str(), "Call esp_audio_pause before BACKUP,%d", __LINE__);
        ret|=esp_audio_pause(_esp_player->_handle);
        if ((st.status == AUDIO_STATUS_RUNNING)|| (st.status == AUDIO_STATUS_PAUSED)) {
            ESP_LOGE(LOG_TAG.c_str(), "BACKUP audio info, ESP_AUDIO_PREFER_SPEED");
            esp_audio_info_get(_esp_player->_handle, &back_info._player_backup);
        }
    }
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::setCurrentPlayerBackInfo(const player_backup_msg_t& back_info)
{
    //step1: set the media msg
    _media_info=back_info._media_info;
    if (_media_info._media_info._media_type == MEDIA_TYPE_MUSIC_A2DP) {
        periph_bt_play(ESPBleManage::_bt_periph);
    }
    //step2: set the player backup msg
    player_backup_msg_t tmp_info=back_info;
    esp_audio_info_set(_esp_player->_handle, &tmp_info._player_backup);
    esp_audio_prefer_t type;
    esp_audio_state_t st;
    memset(&st,0,sizeof(esp_audio_state_t));
    int ret = esp_audio_prefer_type_get(_esp_player->_handle, &type);
    esp_audio_state_get(_esp_player->_handle, &st);
    ESP_LOGI(LOG_TAG.c_str(), "Resume Audio, prefer:%d, status:%d, media_src:%x", type, st.status, st.media_src);
    if (type == ESP_AUDIO_PREFER_MEM) {
        if (st.status == AUDIO_STATUS_RUNNING) {
            ret |= esp_audio_play(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER, NULL, 0);
        }
    }else if (type == ESP_AUDIO_PREFER_SPEED) {
        if (st.status == AUDIO_STATUS_RUNNING) {
            ret |= esp_audio_resume(_esp_player->_handle);
        }
    }
    return (player_err_t)ret;
}

player_err_t 
ESPPlayer::playMedia(const play_media_t& media_info)
{
    AUDIO_MEM_CHECK(LOG_TAG.c_str(), _esp_player, return ERR_AUDIO_MEMORY_LACK);
    //step1: clear the queue cache
    esp_audio_state_t player_current_status;
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    if (uxQueueSpacesAvailable(_play_status_queue_handle)==0) {
        xQueueReceive(_play_status_queue_handle, &player_current_status, 0);
        ESP_LOGW(LOG_TAG.c_str(),"Clear the play statue queue");
    }
    //step2: get the current status
    memset(&player_current_status,0,sizeof(esp_audio_state_t));
    switch(player_current_status.status){
        case AUDIO_STATUS_RUNNING:
            ESP_LOGI(LOG_TAG.c_str(), "PLAY cmd :last status is running,need stop !!");
            return ERR_AUDIO_LAST_STATUS_ERR ;
        case AUDIO_STATUS_PAUSED:
        case AUDIO_STATUS_STOPPED:
        case AUDIO_STATUS_FINISHED:
        case AUDIO_STATUS_UNKNOWN:
            break;
        case AUDIO_STATUS_ERROR:
            ESP_LOGI(LOG_TAG.c_str(), "PLAY cmd :last status is err!!");
            return ERR_AUDIO_FAIL;
        default :
            break;
    }
    //step3: set the player type
    int ret =  esp_audio_media_type_set(_esp_player->_handle, (media_source_type_t)media_info._media_type);
    //bt play set param
    if(media_info._media_type==MEDIA_TYPE_MUSIC_A2DP){
        periph_bt_play(ESPBleManage::_bt_periph);
    }
    //step4: play the uri
    ret |=  esp_audio_play(_esp_player->_handle, AUDIO_CODEC_TYPE_DECODER,  media_info._uri.c_str(), media_info._start_pos);
    if(ret!=ERR_AUDIO_NO_ERROR){
        ESP_LOGE(LOG_TAG.c_str(), "%s play err, code=%d", __func__, ret);
        return (player_err_t)ret;
    }
    //step5: waiting the player status--> runing;     raw and BT mode do not need waiting
    if( media_info._media_type!= MEDIA_TYPE_MUSIC_RAW&&
        media_info._media_type!= MEDIA_TYPE_MUSIC_A2DP)
    {
        memset(&player_current_status,0,sizeof(esp_audio_state_t));
        int timeout=0;
        while (esp_audio_state_get(_esp_player->_handle, &player_current_status)==ESP_ERR_AUDIO_NO_ERROR) {
            if (player_current_status.status == AUDIO_STATUS_RUNNING ||timeout>10) {
                break;
            }else{
                ESP_LOGI(LOG_TAG.c_str(), "%s expected running state, recv:%d", __func__, player_current_status.status);
            }
            timeout++;
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::play(const media_node_t& media_msg)
{
    //step1:store the current play media
    _media_info=media_msg;
    if(_media_info._media_info._media_type==MEDIA_TYPE_MUSIC_TTS){
        _media_info._media_info._media_type=MEDIA_TYPE_MUSIC_HTTP;
    }
    //step2: play the media
    int ret=0;
    switch(_media_info._media_info._media_type){
        case MEDIA_TYPE_MUSIC_RAW:
            ret=playMedia(_media_info._media_info._uri,LOCAL_PLAY_MODE_UNBLOCK);
            break;
        case MEDIA_TYPE_TONE_FLASH:{
            std::string tone_uri=findToneMediaByName(_media_info._media_info._uri);
            if(tone_uri.empty()){
                ret=ERR_AUDIO_INVALID_URI;
                ESP_LOGW(LOG_TAG.c_str(), "Flash tone uri is enmty");
            }else{
                play_media_t flash_media= _media_info._media_info;
                flash_media._uri=tone_uri;
                ret=playMedia(flash_media);
            }
            break;
        }
        default :
            ret=playMedia(_media_info._media_info);
            break;
    }
    return (player_err_t)ret;
}
player_err_t 
ESPPlayer::playMedia(const std::string& media_name,local_play_mode_t play_mode)
{
    //step1: init the raw player
    play_media_t _media_data;
    _media_data._uri="raw://sdcard/ut/test.mp3";
    _media_data._media_type=MEDIA_TYPE_MUSIC_RAW;
    _media_data._start_pos=0;
    int ret=playMedia(_media_data);
    if(ret!=ERR_AUDIO_NO_ERROR){
        ESP_LOGW(LOG_TAG.c_str(), "set raw src err: %s!!",esp_err_to_name(ret));
        return (player_err_t)ret;
    }
    //step2: check block play or unblock
    _local_media_name=media_name;
    if(play_mode==LOCAL_PLAY_MODE_BLOCK){
        execLocalPlayer();
    }else if(play_mode==LOCAL_PLAY_MODE_UNBLOCK){
        memset(_raw_player_task_stack,0,sizeof(LOCAL_PLAYER_TASK_SIZE));
        memset(_raw_player_tasktask_TCB,0,sizeof(StaticTask_t));
        if(xTaskCreateStaticPinnedToCore(&ESPPlayer::writeRawBuff, "RawPlayer", LOCAL_PLAYER_TASK_SIZE, this, 12, _raw_player_task_stack, _raw_player_tasktask_TCB,1)==NULL){
            ESP_LOGI(LOG_TAG.c_str(),"Creat loacal Player Task err, errcode=%d!",ret);
            return ERR_AUDIO_FAIL;
        }
    }
    return (player_err_t)ret;
}
void 
ESPPlayer::writeRawBuff(void* param)
{
    ESPPlayer* espPlayer=(ESPPlayer*)param;
    espPlayer->execLocalPlayer();
    vTaskDelete(NULL);
}
void
ESPPlayer::execLocalPlayer()
{
    int waiting_timeout=0;
    tts_stream_ptr_t tts_stream_info;
    memset(&tts_stream_info,0,sizeof(tts_stream_ptr_t));
    while(1){
        if(TTS_STREAM_QUEUE_LENGTH== uxQueueSpacesAvailable(_tts_stream_queue_handle)){
            if(waiting_timeout<10){
                ESP_LOGI(LOG_TAG.c_str(), "Queue is empty waiting !!");
                vTaskDelay(10 / portTICK_PERIOD_MS);
                waiting_timeout++;
                continue;
            }else{
                ESP_LOGI(LOG_TAG.c_str(), "TTS finished!!");
                audio_element_set_ringbuf_done(_raw_play_handle);
                audio_element_finish_state(_raw_play_handle);
                break;
            }
        }
        if(xQueueReceive(_tts_stream_queue_handle,&tts_stream_info, 0)== pdPASS){
            int ret=raw_stream_write(_raw_play_handle,tts_stream_info.data, tts_stream_info.length);
            //free the tts data
            sysMemFree(tts_stream_info.data);
            if(ret!=tts_stream_info.length){
                ESP_LOGI(LOG_TAG.c_str(), "Raw player was stoped!!");
                break;
            }
        }
    }
}
int 
ESPPlayer::getToneMediaNum()
{
    return (sizeof(_local_tone_media_list)/sizeof(_local_tone_media_list[0]));
}
std::string
ESPPlayer::findToneMediaByName(const std::string& name)
{
    for(int i=0; i<getToneMediaNum();i++){
        if(name.compare(_local_tone_media_list[i]._name)==0){
            return _local_tone_media_list[i]._uri;
        }
    }
    return std::string();
}
void 
ESPPlayer::registerPlayerCMD()
{
    esp_console_cmd_t join_cmd;
    join_cmd.command ="play";
    join_cmd.help = "player cmd.";
    join_cmd.hint = NULL;
    join_cmd.func = &playerCMD;
    join_cmd.argtable=NULL;
    ESP_ERROR_CHECK(esp_console_cmd_register(&join_cmd));
}
void 
ESPPlayer::a2DPEventCallback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param)
{
    ESP_LOGI(LOG_TAG.c_str(), "A2DP EVENT=%d",event);
    switch(event){
        case ESP_A2D_CONNECTION_STATE_EVT:          /*!< connection state changed event */
            if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED){
            }else if(param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){  //connected 
            }
            break;  
        case ESP_A2D_AUDIO_STATE_EVT:                   /*!< audio stream transmission state changed event */
            //check the player current status 
            esp_audio_state_t current_player_status;
            esp_audio_state_get(ESPPlayer::_esp_player->_handle,&current_player_status);
            if(  ( current_player_status.media_src != MEDIA_SRC_TYPE_MUSIC_A2DP&&
                   param->audio_stat.state==ESP_A2D_AUDIO_STATE_STARTED
                 )|| //CURRENT IS NOT a2dp or is a2dp but stoped by user
                 ( current_player_status.media_src == MEDIA_SRC_TYPE_MUSIC_A2DP&&
                   param->audio_stat.state==ESP_A2D_AUDIO_STATE_STARTED&&
                   current_player_status.status!=AUDIO_STATUS_RUNNING
                 )
              )// need play the a2dp media
            {
                if( current_player_status.media_src == MEDIA_SRC_TYPE_MUSIC_A2DP&&
                    current_player_status.status==AUDIO_STATUS_PAUSED
                  )
                {
                    _singleton_instance->resume();
                }else{
                    //step1: play the bt media
                    media_node_t bt_media_data;
                    memset(&bt_media_data,0,sizeof(media_node_t));
                    bt_media_data._media_info._uri="aadp://44100:2@bt/sink/stream.pcm";
                    bt_media_data._media_info._media_type=AiBox::MEDIA_TYPE_MUSIC_A2DP;
                    bt_media_data._media_info._start_pos=0;
                    bt_media_data._not_allow_seek=true;
                    bt_media_data._func=nullptr;
                    bt_media_data._business_type=Business_Type_NONE;
                    ESP_LOGI(LOG_TAG.c_str(), "Play the BT media.");
                    MediaVehicle::getMediaVehicleInstance()->setMediaData(bt_media_data);
                    media_vehicle_data_t media_vehicle;
                    media_vehicle._media_vehicle=MediaVehicle::getMediaVehicleInstance();
                    auto media_queue_handle=PlayManage::getPlayManageInstance()->getMediaQueueHandle();
                    if(media_queue_handle!=nullptr){
                        xQueueSend(media_queue_handle,&media_vehicle,0);
                    }else{
                        ESP_LOGI(LOG_TAG.c_str(), "Media handle is nullptr.");
                    }
                }
            }
            break;  
        case ESP_A2D_AUDIO_CFG_EVT:                     /*!< audio codec is configured, only used for A2DP SINK */
            break;  
        case ESP_A2D_MEDIA_CTRL_ACK_EVT:
            ESP_LOGI(LOG_TAG.c_str(), "A2DP EVENT=%d;Param.cmd=%d",event,param->media_ctrl_stat.cmd);
            break;
        default :
            break;  
    }
}