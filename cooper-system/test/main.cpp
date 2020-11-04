#include <time.h> 
#include <sys/time.h>
#include <time.h> 
#include <unistd.h>
#include "LCMParser.h"
int main(int argc,char*argv[])
{
	AiBox::LCMParser::getLCMInstance()->initLCMParser("test_1234_1334",500);
	while(1)
	{
		AiBox::LCMParser::getLCMInstance()->shouldACKWakeup(111,AiBox::LCMParser::WAKEUP_POLICY_NEAR);
		sleep(5);
	}
}