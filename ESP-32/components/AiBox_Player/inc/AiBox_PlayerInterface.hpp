/****************************************
*   player Interface  .hpp
*   author: jiyingliang
*   email: jiyingliang@hisense.com
*   date: 2020.03.21
*****************************************/
#pragma once
#include <string>
#include "AiBox_BasePlayer.hpp"
#include "AiBox_PlayerStatus.hpp"
namespace AiBox
{
    class PlayerInterface
    {
        public:
            PlayerInterface();
            PlayerInterface(const PlayerInterface&)=delete;
            PlayerInterface& operator=(const PlayerInterface&)=delete;
            static PlayerInterface* getPlayerInterfaceInstance();
            ~PlayerInterface(){}
            bool initPlayer();
            player_err_t stop();
            player_err_t pause();
            player_err_t resume();
            player_err_t play(const media_node_t& media_msg);
            player_err_t setVol(const int vol);
            player_err_t getVol(int& vol);
            player_err_t getCurrentPlayTime(int& time);
            player_err_t getCurrentPlayPos(int& pos);
            player_err_t getCurrentPlayerStatus(player_status_show_t& show_data);
            player_err_t getCurrentPlayerMediaInfo(media_node_t& media_msg);
            player_err_t getCurrentPlayerBackInfo(player_backup_msg_t& back_info);
            player_err_t setCurrentPlayerBackInfo(const player_backup_msg_t& back_info);
            player_err_t setPlayTime(int _target_time_sec);
            media_node_t& getCurrentMediaCite();
            void writeRawMediaData(char* buffer,int length);
            void clearRawStreamQueue();
            bool isLocalRawMediaStoped();
            QueueHandle_t getPlayerStatusQueueHandle();
        private:
            BasePlayer* _player{nullptr};
            static PlayerInterface* _singleton_instance;
    };
};