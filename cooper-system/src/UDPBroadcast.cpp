#include "UDPBroadcast.h"
#include <fcntl.h>
using namespace AiBox;
std::shared_ptr<UDPBroadcast> UDPBroadcast::_singleton=nullptr;
#define UDP_BROACAST_ADDR 56889
std::shared_ptr<UDPBroadcast> 
UDPBroadcast::getInstance()
{
    if(_singleton.get()==nullptr){
        new UDPBroadcast();
    }
    return _singleton;
}
UDPBroadcast::UDPBroadcast()
{
    _singleton=std::shared_ptr<UDPBroadcast>(this);
}
bool 
UDPBroadcast::init()
{
    if(sendInit()&&recvInit()){
        _inited=true;
    }
    return _inited;
}
bool 
UDPBroadcast::sendInit()
{
    //step1: creat
    _udp_send_fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(_udp_send_fd==-1){
        return false;
    }
    memset(&_addr_send,0,sizeof(struct sockaddr_in));
    _addr_send.sin_family=AF_INET;
    _addr_send.sin_addr.s_addr=htonl(INADDR_BROADCAST);//address
    _addr_send.sin_port=htons(UDP_BROACAST_ADDR);//port 
    //step2: set
    const int opt=-1;
    if(setsockopt(_udp_send_fd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt))==-1){
        return false;
    }
    return true;
}
int 
UDPBroadcast::send(const std::string& payload)
{
    if(!_inited){
        std::cout<<"UDP broadcast init err!!"<<std::endl;
        return -1;
    }
    std::string msg="HEAD"+payload+"END";
    int ret=sendto(_udp_send_fd,msg.c_str(),msg.size(),0,(sockaddr*)&_addr_send,sizeof(_addr_send));//send message
    if(ret<0){
        return -1;
    }
    return ret;
}
bool 
UDPBroadcast::recvInit()
{
    //step1: creat
    _udp_recv_fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(_udp_recv_fd==-1){
        return false;
    }
    memset(&_addr_recv,0,sizeof(struct sockaddr_in));
    _addr_recv.sin_family=AF_INET;
    _addr_recv.sin_addr.s_addr=htonl(INADDR_ANY);//address
    _addr_recv.sin_port=htons(UDP_BROACAST_ADDR);//
    //step2: set
    const int opt=-1;
    if(setsockopt(_udp_recv_fd,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt))==-1){
        return false;
    }
    //bind
    if(bind(_udp_recv_fd,(struct sockaddr*)&(_addr_recv),sizeof(_addr_recv))==-1){
        return false;
    }
    int flag = fcntl(_udp_recv_fd, F_GETFL, 0);
    if (flag < 0) {
        return false;
    }
    if (fcntl(_udp_recv_fd, F_SETFL, flag | O_NONBLOCK) < 0) {
       return false;
    }
    return true;
}
int 
UDPBroadcast::read(char* buf,int len)
{
    if(!_inited){
        std::cout<<"UDP broadcast init err!!"<<std::endl;
        return -1;
    }
    socklen_t add_len=sizeof(_addr_recv);
    return recvfrom(_udp_recv_fd,buf,len,0,(struct sockaddr*)&_addr_recv,&add_len);
}
bool 
UDPBroadcast::handleRecv(std::string& payload )
{
    char buf[1024];
    int read_len=read(buf,1024);
    if(read_len>0){
        std::string(buf,read_len);
        _raw_data+=std::string(buf,read_len);
        //std::cout<<"UDP BroadCast read::"<<_raw_data<<std::endl;
    }
    std::string::size_type pos1=_raw_data.find("HEAD");
    std::string::size_type pos2=_raw_data.find("END");
    payload.clear();
    if(pos2!=std::string::npos&&pos2>pos1){
        payload=_raw_data.substr(pos1+4,pos2-pos1-4);
        //std::cout<<"UDP BroadCast Payload>>pos1="<<pos1<<";pos2="<<pos2<<";payload==="<<payload<<";<<"<<std::endl;
        //earse the used data
        _raw_data=_raw_data.substr(pos2+3);//last 
    }
    return !payload.empty();
}