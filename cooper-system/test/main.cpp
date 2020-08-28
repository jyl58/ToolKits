#include <time.h> 
#include <sys/time.h>
#include <time.h> 
#include <unistd.h>
#include "LCMParser.h"
int main(int argc,char*argv[])
{
	AiBox::LCMParser::getLCMInstance()->initLCMParser("test_1234_test");
	while(1)
	{
		struct timeval tvdd;
		gettimeofday(&tvdd,NULL);
		AiBox::LCMParser::getLCMInstance()->shouldACKWakeup(tvdd.tv_sec+tvdd.tv_usec*0.000001,AiBox::LCMParser::WAKEUP_POLICY_TIME);
		sleep(5);
	}
}