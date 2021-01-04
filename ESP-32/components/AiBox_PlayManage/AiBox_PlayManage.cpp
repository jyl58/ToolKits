/****************************************
*   ESP play manage .cpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.16
*****************************************/
#include "LogInterface.hpp"
#include "AiBox_PlayManage.hpp"
#include "AiBox_ESPPlayer.hpp"
using namespace AiBox;
static const std::string LOG_TAG="PlayManage";
int playManageCMD(int argc, char **argv)
{
    if(argc<2){
        LOGI(LOG_TAG.c_str(),"playManage Need 2 param!!");
        return 0;
    }
    std::string subcmd=std::string(argv[1]);
    if(subcmd.compare("resume")==0){
        PlayManage::getPlayManageInstance()->resume();
    }else if(subcmd.compare("pause")==0){
        PlayManage::getPlayManageInstance()->pause();
    }else if(subcmd.compare("stop")==0){
        PlayManage::getPlayManageInstance()->stop();
    }
    return 0;
}
PlayManage* PlayManage::_singleton_instance=nullptr;
PlayManage* 
PlayManage::getPlayManageInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new PlayManage();
    }
    return _singleton_instance;
}
void* 
PlayManage::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
PlayManage::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}
PlayManage::PlayManage()
{
    _singleton_instance=this;
}
PlayManage::~PlayManage()
{
    if(_task_buffer!=nullptr){
        sysMemFree(_task_buffer);
    }
    if(_task_TCB!=NULL){
        sysMemFree(_task_TCB);
    }
}
bool 
PlayManage::initPlayManage()
{
    _pause_media_list.reserve(10);
    //step1: creat a queue set
    _player_queue_set_handle=xQueueCreateSet(2);
    if(_player_queue_set_handle==nullptr){
        return false;
    }
    //step2: creat a media vehicle queue 
    _media_queue_handle=xQueueCreate(MEDIA_QUEUE_LENGTH,sizeof(media_vehicle_data_t));
    if(_media_queue_handle==nullptr){
        return false;
    }
    //add media vehicle queue hand to queue set
    xQueueAddToSet(_media_queue_handle,_player_queue_set_handle);
    //add player status queue handle to queue set 
    xQueueAddToSet(PlayerInterface::getPlayerInterfaceInstance()->getPlayerStatusQueueHandle(),_player_queue_set_handle);
    _task_buffer=(StackType_t*)sysMemMalloc(1024*4);
    if(_task_buffer==NULL){
        return false;
    }
    _task_TCB   =(StaticTask_t*)sysMemMallocInner(1,sizeof(StaticTask_t));
    if(_task_TCB==NULL){
        sysMemFree(_task_buffer);
        return false;
    }
    //creat a task to maintain play list, use cpu core 1 
    if(xTaskCreateStaticPinnedToCore(&PlayManage::mediaListMaintainTask, "media_maintain", 1024*4, this, 10, _task_buffer ,_task_TCB,1)==NULL){
        LOGI(LOG_TAG.c_str(),"Creat media handle task err!!!");
        return false;
    }
    return true;
}
void 
PlayManage::mediaListMaintainTask(void* param)
{
    ((PlayManage*)param)->exec();
    vTaskDelete(NULL);
}
void 
PlayManage::exec()
{
    QueueSetMemberHandle_t xActivatedMember;
    media_node_t temp_media;
    media_vehicle_data_t media_vehicle;
    while(1){
        xActivatedMember = xQueueSelectFromSet( _player_queue_set_handle,100 / portTICK_PERIOD_MS ); //200ms
        if(xActivatedMember==_media_queue_handle){
            //play  media command
            player_status_show_t player_current_status;
            memset(&player_current_status,0,sizeof(player_status_show_t));
            PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerStatus(player_current_status);
            //receive the media
            if (xQueueReceive(_media_queue_handle, &media_vehicle, 0) == pdPASS){
                LOGI(LOG_TAG.c_str(),"Received Media !!!,time:%llu (ms)",SystemInterface::getSystemInterfaceInstance()->getCurrentTimeMs());
                MediaVehicle::getMediaData(media_vehicle._media_vehicle,temp_media);
                bool allow_play=true;
                switch(temp_media._media_info._media_type){
                    case MEDIA_TYPE_MUSIC_A2DP:
                    case MEDIA_TYPE_MUSIC_HTTP:
                        {
                            //step1: new media need clear the 
                            if(!_pause_media_list.empty()){
                                _pause_media_list.clear();
                            }
                            //step2: if already running,need stop first
                            if( player_current_status._status==PLAYER_STATUS_RUNNING||
                                player_current_status._status==PLAYER_STATUS_PAUSED
                              )
                            {
                                if(PlayerInterface::getPlayerInterfaceInstance()->stop()!=ERR_AUDIO_NO_ERROR){
                                    LOGI(LOG_TAG.c_str(), "[http] Stop current media err!!");
                                }
                            }
                        }
                        break;
                    case MEDIA_TYPE_MUSIC_RAW:
                    case MEDIA_TYPE_TONE_FLASH:
                    case MEDIA_TYPE_MUSIC_TTS: //maybe local,maybe http tts
                        {
                            if( player_current_status._status==PLAYER_STATUS_RUNNING||
                                player_current_status._status==PLAYER_STATUS_PAUSED
                            )
                            {
                                if( player_current_status._media_src==MEDIA_TYPE_MUSIC_HTTP||
                                    player_current_status._media_src==MEDIA_TYPE_MUSIC_A2DP
                                )
                                {
                                    player_backup_msg_t back_media_msg;
                                    memset(&back_media_msg,0,sizeof(esp_audio_info_t));
                                    PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerBackInfo(back_media_msg);
                                    //store the prev the media info and player backup msg
                                    _pause_media_list.push_back(back_media_msg);
                                    if(PlayerInterface::getPlayerInterfaceInstance()->pause()!=ERR_AUDIO_NO_ERROR){
                                        LOGI(LOG_TAG.c_str(), "Pause current media err!!");
                                    }
                                }else{
                                    if(PlayerInterface::getPlayerInterfaceInstance()->stop()!=ERR_AUDIO_NO_ERROR){
                                        LOGI(LOG_TAG.c_str(), "Stop current media err!!");
                                    }
                                }
                            }
                        }
                        break;
                    default :
                        LOGI(LOG_TAG.c_str(), "unknow media type");
                        allow_play=false;
                        break;
                }
                //play
                if(allow_play){
                    PlayerInterface::getPlayerInterfaceInstance()->play(temp_media);
                }
            }
        }else if(xActivatedMember==PlayerInterface::getPlayerInterfaceInstance()->getPlayerStatusQueueHandle()){
            //player status
            audio_state_t player_current_status;
            memset(&player_current_status,0,sizeof(audio_state_t));
            media_node_t& current_media_msg=PlayerInterface::getPlayerInterfaceInstance()->getCurrentMediaCite();
            if(xQueueReceive(PlayerInterface::getPlayerInterfaceInstance()->getPlayerStatusQueueHandle(), &player_current_status, 0) == pdPASS){
                LOGI(LOG_TAG.c_str(), "Player status, status:%d, err_msg:%d, src:%d",player_current_status._status, player_current_status._err_msg, player_current_status._media_src);
                switch(player_current_status._status){
                    case AUDIO_STATUS_RUNNING:
                        if(current_media_msg._media_info._start_time>0){
                            seek(current_media_msg._media_info._start_time);
                            //reset the start time
                            current_media_msg._media_info._start_time=0;
                        }
                        break;
                    case AUDIO_STATUS_PAUSED:
                        break;
                    case AUDIO_STATUS_STOPPED:
                        break;
                    case AUDIO_STATUS_FINISHED:{
                            /*local raw media stoped  need report a finished status,so use the flag temporarily identify is stoped*/
                            if(PlayerInterface::getPlayerInterfaceInstance()->isLocalRawMediaStoped()){
                                LOGI(LOG_TAG.c_str(), "Local raw media stoped!!");
                                break;
                            }
                            //step1: run the current media call back
                            if(current_media_msg._func!=nullptr){
                                LOGI(LOG_TAG.c_str(), "Run the user's callback func!!");
                                player_status_show_t player_status;
                                memset(&player_status,0,sizeof(player_status_show_t));
                                PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerStatus(player_status);
                                current_media_msg._func(player_status);
                            }
                            //step2: current media allow rusume the last interrupt media ?
                            if(current_media_msg._business_type==Business_Type_NONE){
                                if(!_pause_media_list.empty()){
                                    player_backup_msg_t back_media_data=_pause_media_list.back();
                                    media_node_t prev_media_info=back_media_data._media_info;
                                    if(!prev_media_info._not_allow_auto_resume){
                                        _pause_media_list.pop_back();
                                        //resume the interrupt media
                                        LOGI(LOG_TAG.c_str(), "Resume the interrupt media!!");
                                        PlayerInterface::getPlayerInterfaceInstance()->setCurrentPlayerBackInfo(back_media_data);
                                    }
                                }
                            }
                        }
                        break;
                    default :
                        break;
                }
            }
        }
    }
}
void 
PlayManage::pause()
{
    //pause command by user,then do not allow resume auto
    PlayerInterface::getPlayerInterfaceInstance()->pause();
    //if cach is not empty
    if(!_pause_media_list.empty()){
        //user pause,do not allow the auto resume
        _pause_media_list.back()._media_info._not_allow_auto_resume=true;
    }
}
void 
PlayManage::stop()
{
    PlayerInterface::getPlayerInterfaceInstance()->stop();
}
void 
PlayManage::resume()
{
    player_status_show_t player_current_status;
    memset(&player_current_status,0,sizeof(player_status_show_t));
    PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerStatus(player_current_status);
    if(player_current_status._status==PLAYER_STATUS_PAUSED){
        //at pause status,so resume 
        PlayerInterface::getPlayerInterfaceInstance()->resume();
    }else{
        //need resume backup media
        if(!_pause_media_list.empty()){
            player_backup_msg_t back_media_data=_pause_media_list.back();
            //allow auto resume
            back_media_data._media_info._not_allow_auto_resume=false;
            //delete from cache
            _pause_media_list.pop_back();
            //resume the interrupt media
            LOGI(LOG_TAG.c_str(), "Resume the interrupt media by user!!");
            PlayerInterface::getPlayerInterfaceInstance()->setCurrentPlayerBackInfo(back_media_data);
        }
    }
}
void 
PlayManage::setVol(int vol)
{
    PlayerInterface::getPlayerInterfaceInstance()->setVol(vol);
}
void 
PlayManage::seek(int seek_time_s,bool absolute_time)
{
    player_status_show_t player_current_status;
    memset(&player_current_status,0,sizeof(player_status_show_t));
    PlayerInterface::getPlayerInterfaceInstance()->getCurrentPlayerStatus(player_current_status);
    if(player_current_status._status!=PLAYER_STATUS_PAUSED&&player_current_status._status!=PLAYER_STATUS_RUNNING){
        //TODO::add play hint message
        LOGI(LOG_TAG.c_str(),"Current player not in playing or pause status!!!");
        return;
    }
    if(PlayerInterface::getPlayerInterfaceInstance()->getCurrentMediaCite()._not_allow_seek){
        //TODO::add play hint message
        LOGI(LOG_TAG.c_str(),"Current Media not allow seek operation!!!");
        return ;
    }
    int current_time_s=0;
    if(absolute_time){
        current_time_s=0;
    }else{
        //relative current time
        current_time_s=(int)(player_current_status._time*0.001); //s
    }

    int target_time_s=seek_time_s+current_time_s;
    int total_time_s=(int)(player_current_status._total_time*0.001);
    if(target_time_s>total_time_s){
        LOGI(LOG_TAG.c_str(),"Target time=%d(s) greater than the total time=%d(s)",target_time_s,total_time_s);
        return;
    }else  if(target_time_s<0){
        LOGI(LOG_TAG.c_str(),"Target time=%d(s) less than the 0",target_time_s);
        return;
    }
    PlayerInterface::getPlayerInterfaceInstance()->setPlayTime(target_time_s);
}

/*media vehicle*/
MediaVehicle* MediaVehicle::_singleton_instance=nullptr;
MediaVehicle* 
MediaVehicle::getMediaVehicleInstance()
{
    if(_singleton_instance==nullptr){
        _singleton_instance=new MediaVehicle();
    }
    return _singleton_instance;
}
void 
MediaVehicle::getMediaData(MediaVehicle* media_vehicle_ptr,media_node_t& media_data)
{
    if(media_vehicle_ptr!=nullptr){
        media_data=media_vehicle_ptr->_media_data;
    }else{
        LOGI(LOG_TAG.c_str(),"Media Vehicle is empty!!!");
    }
}
void 
MediaVehicle::setMediaData(const media_node_t& media_data)
{
    _media_data=media_data;
}
void* 
MediaVehicle::operator new(std::size_t size)throw(std::bad_alloc)
{
    return sysMemMalloc(size);
}
void 
MediaVehicle::operator delete(void *ptr)throw()
{
    if(ptr==nullptr) return;
    sysMemFree(ptr);
    ptr=nullptr;
}