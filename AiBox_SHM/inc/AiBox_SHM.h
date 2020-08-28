#pragma once
#include <string>
#include <memory>
namespace AiBox
{
class AiBoxSHM
{
    public:
        ~AiBoxSHM();
        AiBoxSHM(const AiBoxSHM&)=delete;
        AiBoxSHM& operator=(const AiBoxSHM&)=delete;
        static std::shared_ptr<AiBoxSHM> getInstance();
        static void clearSHM();
        int writeValueToSHM(const std::string& key,const int& value);
        int writeValueToSHM(const std::string& key,const float& value);
        int writeValueToSHM(const std::string& key,const double& value);
        int writeValueToSHM(const std::string& key,const std::string& value);
        int readValueFromSHM(const std::string& key,int& value);
        int readValueFromSHM(const std::string& key,float& value);
        int readValueFromSHM(const std::string& key,double& value);
        int readValueFromSHM(const std::string& key,std::string& value);
        int notify();
        int readable();
    private:
        AiBoxSHM();
        static std::shared_ptr<AiBoxSHM> _single;
};
};