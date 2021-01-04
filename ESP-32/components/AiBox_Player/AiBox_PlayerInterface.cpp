/****************************************
*   player Interface  .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#include "LogInterface.hpp"
#ifdef ESP32_PLATFORM
    #include "AiBox_ESPPlayer.hpp"
#endif
#include "AiBox_PlayerInterface.hpp"
using namespace AiBox;
static std::string LOG_TAG="PlayerInterface";
PlayerInterface* PlayerInterface::_singleton_instance=nullptr;
PlayerInterface* 
PlayerInterface::getPlayerInterfaceInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new PlayerInterface();
    }
    return _singleton_instance;
}
PlayerInterface::PlayerInterface()
{
    _singleton_instance=this;
}
bool 
PlayerInterface::initPlayer()
{
#ifdef ESP32_PLATFORM
    _player=new ESPPlayer();
#endif
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"Creat player err!!");
        return false;
    }
    if(!_player->init()){
        LOGI(LOG_TAG.c_str(),"Init player err!!");
        return false;
    }
    return true;
}
player_err_t 
PlayerInterface::stop()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->stop();
}
player_err_t 
PlayerInterface::pause()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->pause();
}
player_err_t 
PlayerInterface::resume()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->resume();
}
player_err_t 
PlayerInterface::play(const media_node_t& media_msg)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->play(media_msg);
}
player_err_t 
PlayerInterface::setVol(const int vol)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->setVol(vol);
}
player_err_t 
PlayerInterface::getVol(int& vol)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getVol(vol);
}
player_err_t 
PlayerInterface::getCurrentPlayTime(int& time)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getCurrentPlayTime(time);
}
player_err_t 
PlayerInterface::getCurrentPlayPos(int& pos)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getCurrentPlayPos(pos);
}
player_err_t 
PlayerInterface::getCurrentPlayerStatus(player_status_show_t& show_data)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getCurrentPlayerStatus(show_data);
}
player_err_t 
PlayerInterface::getCurrentPlayerMediaInfo(media_node_t& media_msg)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getCurrentPlayerMediaInfo(media_msg);
}
player_err_t 
PlayerInterface::getCurrentPlayerBackInfo(player_backup_msg_t& back_info)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->getCurrentPlayerBackInfo(back_info);
}
player_err_t 
PlayerInterface::setCurrentPlayerBackInfo(const player_backup_msg_t& back_info)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->setCurrentPlayerBackInfo(back_info);
}
player_err_t 
PlayerInterface::setPlayTime(const int _target_time_sec)
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return ERR_AUDIO_FAIL;
    }
    return _player->setPlayTime(_target_time_sec);
}
QueueHandle_t 
PlayerInterface::getPlayerStatusQueueHandle()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return nullptr;
    }
    return _player->_play_status_queue_handle;
}
media_node_t& 
PlayerInterface::getCurrentMediaCite()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        abort();
        media_node_t tmp;
        return tmp;
    }
    return _player->_media_info;
}

bool 
PlayerInterface::isLocalRawMediaStoped()
{
    if(_player==nullptr){
        LOGI(LOG_TAG.c_str(),"player is nullptr!");
        return true;
    }
    if(_player->is_local_media_stoped){
        _player->is_local_media_stoped=false;
        return true;
    }
    return false;
}
void 
PlayerInterface::writeRawMediaData(char* buffer,int length)
{
    if(_player!=nullptr){
        tts_stream_ptr_t tts_stream_info;
        tts_stream_info.length=length;
        tts_stream_info.data=(char*)sysMemMalloc(length);
        memcpy(tts_stream_info.data,buffer,length);
        xQueueSend(_player->_tts_stream_queue_handle,(void *)&tts_stream_info,0);
    }
}
void 
PlayerInterface::clearRawStreamQueue()
{
    if(_player!=nullptr){
        _player->clearRawStreamQueue();
    }
}