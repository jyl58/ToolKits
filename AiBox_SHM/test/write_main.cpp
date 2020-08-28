#include "AiBox_SHM.h"
#include <iostream>
#include <unistd.h>
int main(int argc,char** argv)
{
    std::cout<<"heredddd"<<std::endl;
    AiBox::AiBoxSHM::getInstance()->writeValueToSHM("test1",1);
    AiBox::AiBoxSHM::getInstance()->writeValueToSHM("test2",2.2f);
    AiBox::AiBoxSHM::getInstance()->writeValueToSHM("test3",2.2);
    AiBox::AiBoxSHM::getInstance()->writeValueToSHM("test4","hello,boost");
    std::cout<<"kkkkkkkkgf"<<std::endl;
    AiBox::AiBoxSHM::getInstance()->notify();
    while(1){
        std::cout<<"here"<<std::endl;
        sleep(1);
        AiBox::AiBoxSHM::getInstance()->writeValueToSHM("test2",5.5f);
        AiBox::AiBoxSHM::getInstance()->notify();
    }
}