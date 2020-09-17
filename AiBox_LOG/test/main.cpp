#include "AiBox_LOG.h"
#include "src.h"
#include "AiBox_LogManage.h"
int main(int argc,char**argv)
{
    AiBox::LogManage::getInstance()->init(true);
    int a=3;
    AIBOX_LOGE("err=",a);
    AIBOX_LOGI("info=",4);
    AIBOX_LOGW("war",5);
    AiBox::LogManage::getInstance()->logToFile("test_log.txt");
    AIBOX_LOGD("debug",8);
    test();
    AiBox::LogManage::getInstance()->stopLogToFile();
    AIBOX_LOGD("debug",444444);
    return 0;
}