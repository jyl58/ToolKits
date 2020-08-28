#include <iostream>
#include "AiBox_SHM.h"
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#define AIBOX_SHM_NAME  "hisense_aibox_shm"
#define AIBOX_SHM_MUTEX "hisense_aibox_shm_mutex"
#define AIBOX_SHM_CON   "hisense_aibox_shm_con"
#define AIBOX_SHM_READY "hisense_aibox_shm_ready_read"
using namespace AiBox;
using namespace boost::interprocess;
typedef allocator<char,managed_shared_memory::segment_manager> CharAllocator;
typedef basic_string<char, std::char_traits<char>, CharAllocator> shm_string;
std::shared_ptr<AiBoxSHM> AiBoxSHM::_single=nullptr;
static managed_shared_memory _managed_shm;
static interprocess_mutex *_named_mtx=nullptr;
void 
AiBoxSHM::clearSHM()
{
    shared_memory_object::remove(AIBOX_SHM_NAME);
}
std::shared_ptr<AiBoxSHM> AiBoxSHM::getInstance()
{
    if(_single.get()==nullptr){
        new AiBoxSHM();
    }
    return _single;
}
AiBoxSHM::AiBoxSHM()
{
    _managed_shm=managed_shared_memory(open_or_create, AIBOX_SHM_NAME, 1024*8);
    _named_mtx=_managed_shm.find_or_construct<interprocess_mutex>(AIBOX_SHM_MUTEX)();
    if(!_named_mtx){
        std::cout<<"find mtx err."<<std::endl;
    }
    if(_managed_shm.find_or_construct<int>(AIBOX_SHM_READY)(0)==nullptr){
        std::cout<<"creat ready flag err."<<std::endl;
    }
   _single=std::shared_ptr<AiBoxSHM>(this); 
}
AiBoxSHM::~AiBoxSHM()
{
    _named_mtx=nullptr;
}

int 
AiBoxSHM::writeValueToSHM(const std::string& key,const int& value)
{
    int* value_ptr = _managed_shm.find_or_construct<int>(key.c_str())(value);
    if(value_ptr==nullptr){
        return -1;
    }
    _named_mtx->lock();
    *value_ptr=value;
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::writeValueToSHM(const std::string& key,const float& value)
{
    float* value_ptr = _managed_shm.find_or_construct<float>(key.c_str())(value);
    if(value_ptr==nullptr){
        return -1;
    }
    _named_mtx->lock();
    *value_ptr=value;
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::writeValueToSHM(const std::string& key,const double& value)
{
    double* value_ptr = _managed_shm.find_or_construct<double>(key.c_str())(value);
    if(value_ptr==nullptr){
        return -1;
    }
    _named_mtx->lock();
    *value_ptr=value;
    _named_mtx->unlock();    
    return 0;
}
int 
AiBoxSHM::writeValueToSHM(const std::string& key,const std::string& value)
{
    shm_string* value_ptr = _managed_shm.find_or_construct<shm_string>(key.c_str())(value.c_str(),_managed_shm.get_segment_manager());
    if(value_ptr==nullptr){
        return -1;
    }
    _named_mtx->lock();
    *value_ptr=value.c_str();
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::readValueFromSHM(const std::string& key,int& value)
{
    std::pair<int*, std::size_t> value_ptr= _managed_shm.find<int>(key.c_str());
    if(value_ptr.first==nullptr){
        return -1;
    }
    _named_mtx->lock();
    value=*value_ptr.first;
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::readValueFromSHM(const std::string& key,float& value)
{
    std::pair<float*, std::size_t> value_ptr= _managed_shm.find<float>(key.c_str());
    if(value_ptr.first==nullptr){
        return -1;
    }
    _named_mtx->lock();
    value=*value_ptr.first;
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::readValueFromSHM(const std::string& key,double& value)
{
    std::pair<double*, std::size_t> value_ptr= _managed_shm.find<double>(key.c_str());
    if(value_ptr.first==nullptr){
        return -1;
    }
    _named_mtx->lock();
    value=*value_ptr.first;
    _named_mtx->unlock();
    return 0;
}
int 
AiBoxSHM::readValueFromSHM(const std::string& key,std::string& value)
{
    std::pair<shm_string*, std::size_t> value_ptr = _managed_shm.find<shm_string>(key.c_str());
    if(value_ptr.first==nullptr){
        return -1;
    }
    _named_mtx->lock();
    value=std::string((*value_ptr.first).c_str());
    _named_mtx->unlock();
    return 0;
}
int
AiBoxSHM::notify()
{
    int* value_ptr = _managed_shm.find_or_construct<int>(AIBOX_SHM_READY)(1);
    if(value_ptr==nullptr){
        return -1;
    }
    _named_mtx->lock();
    *value_ptr=1;
    _named_mtx->unlock();
    return 1;
}
int 
AiBoxSHM::readable()
{
    std::pair<int*, std::size_t> ready_ptr= _managed_shm.find<int>(AIBOX_SHM_READY);
    if(ready_ptr.first==nullptr){
        return -1;
    }
    _named_mtx->lock();
    int ready_flag=*ready_ptr.first;
    if(ready_flag>0){
        *ready_ptr.first=0;
    }
    _named_mtx->unlock();
    return ready_flag;
}