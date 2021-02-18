#include <time.h> 
#include <sys/time.h>
#include <string>
#include <time.h> 
#include <iostream>
#include <unistd.h>
#include "LCMParser.h"
#include "UDPBroadcast.h"
void testLCM()
{
	AiBox::LCMParser::getLCMInstance()->initLCMParser("test_1234_1334",500);
	//init the random 
    std::srand((unsigned)time(NULL));
	while(1)
	{
		AiBox::LCMParser::getLCMInstance()->shouldACKWakeup((int)std::rand()%1000,AiBox::LCMParser::WAKEUP_POLICY_NEAR);
		sleep(5);
	}
}
int main(int argc,char*argv[])
{
	testLCM();
}
