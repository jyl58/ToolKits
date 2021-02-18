#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <iostream>
#include <memory>
#include <string>
#include <string.h>
namespace AiBox
{
class UDPBroadcast
{
    public:
        UDPBroadcast(const UDPBroadcast&)=delete;
        void operator=(const UDPBroadcast&)=delete;
        static std::shared_ptr<UDPBroadcast> getInstance();
        bool init();
        bool isInited(){return _inited;}
        bool sendInit();
        bool recvInit();
        int send(const std::string& payload);
        bool handleRecv(std::string& payload );
    private:
        UDPBroadcast();
        int read(char* buf,int len);
        static std::shared_ptr<UDPBroadcast> _singleton;
        struct sockaddr_in _addr_send;
        struct sockaddr_in _addr_recv;
        std::string _raw_data;
        int _udp_send_fd=-1;
        int _udp_recv_fd=-1;
        bool _inited=false;
};
};