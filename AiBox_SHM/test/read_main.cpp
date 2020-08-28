#include "AiBox_SHM.h"
#include <iostream>
#include <unistd.h>
int main(int argc,char** argv)
{
    int int_value=0;
    float float_value=0.0f;
    double double_value=0.0;
    std::string string_value;
    while(1){
        if(AiBox::AiBoxSHM::getInstance()->readable()<=0){
            sleep(1);
            continue;
        }
        if(AiBox::AiBoxSHM::getInstance()->readValueFromSHM("test1",int_value)==0){
            std::cout<<"get int value:"<<int_value<<std::endl;
        }else{
            std::cout<<"read int value err:"<<std::endl;
        }
        if(AiBox::AiBoxSHM::getInstance()->readValueFromSHM("test2",float_value)==0){
            std::cout<<"get float value:"<<float_value<<std::endl;
        }else{
            std::cout<<"read float value err:"<<std::endl;
        }
        if(AiBox::AiBoxSHM::getInstance()->readValueFromSHM("test3",double_value)==0){
            std::cout<<"get double value:"<<double_value<<std::endl;
        }else{
            std::cout<<"read double value err:"<<std::endl;
        }
        if(AiBox::AiBoxSHM::getInstance()->readValueFromSHM("test4",string_value)==0){
            std::cout<<"get string value:"<<string_value<<std::endl;
        }else{
            std::cout<<"read string value err:"<<std::endl;
        }
        if(AiBox::AiBoxSHM::getInstance()->readValueFromSHM("test5",string_value)==0){
            std::cout<<"get string value:"<<string_value<<std::endl;
        }else{
            std::cout<<"read string value err test5:"<<std::endl;
        }
        usleep(100*1000);
        //sleep(1);
    }
}