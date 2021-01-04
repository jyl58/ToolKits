/****************************************
*   ESP play manage .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.04.16
*****************************************/
#pragma once
#include <string>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "AiBox_PlayerInterface.hpp"
#define MEDIA_QUEUE_LENGTH 10
extern "C" int playManageCMD(int argc, char **argv);
namespace AiBox
{
    class MediaVehicle
    {
        public:
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            MediaVehicle(const MediaVehicle&)=delete;
            static MediaVehicle* getMediaVehicleInstance();
            ~MediaVehicle(){}
            static void getMediaData(MediaVehicle* media_vehicle_ptr,media_node_t& media_data);
            void setMediaData(const media_node_t& media_data);
        private:
            MediaVehicle(){}
            media_node_t _media_data;
            static MediaVehicle* _singleton_instance;
    };
    
    typedef struct MediaVehicleData{
       MediaVehicle* _media_vehicle; 
    }media_vehicle_data_t;

    class PlayManage
    {
        public:
            void* operator new(std::size_t size);
            void  operator delete(void *ptr);
            PlayManage();
            ~PlayManage();
            PlayManage(const PlayManage& )=delete;
            PlayManage& operator=(const PlayManage&)=delete;
            static PlayManage* getPlayManageInstance();
            bool initPlayManage();
            const QueueHandle_t& getMediaQueueHandle(){return _media_queue_handle;}
            static void mediaListMaintainTask(void* param);
            const std::vector<player_backup_msg_t>& getBackupList(){return _pause_media_list;}
            /*command used for user directive*/
            void pause();
            void stop();
            void resume();
            void setVol(int vol);
            void seek(int seek_time_s,bool absolute_time=true);
        private:
            void exec();
            QueueSetHandle_t _player_queue_set_handle=nullptr;
            QueueHandle_t _media_queue_handle=nullptr;
            static PlayManage* _singleton_instance;
            std::vector<player_backup_msg_t> _pause_media_list;
            StackType_t* _task_buffer=nullptr;
            StaticTask_t* _task_TCB=nullptr;
    };
};