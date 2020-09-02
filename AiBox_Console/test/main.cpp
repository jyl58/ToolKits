#include <iostream>
#include <unistd.h>
#include "AiBox_Console.h"
class testConsole1
{
    public:
        testConsole1(int b){
            AiBox::Console::registeConsoleFunc("test1",&testConsole1::test,this);
        }

        void test(const std::string& param){
            std::cout<<"test1 console"<<std::endl;
        }
};
class testConsole2
{
    public:
        testConsole2(int b){
            AiBox::Console::registeConsoleFunc("test2",&testConsole2::test,this);
        }

        void test(const std::string& param){
            std::cout<<"test2 console"<<std::endl;
        }
};
int main(int argc,char** argv)
{
    testConsole1 a(1);
    testConsole2 b(3);
    AiBox::Console::getInstance()->run();
    while(1){
        sleep(2);
    }
}