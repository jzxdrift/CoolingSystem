#include <stdint.h>

void setAlarm(const char message[])
{
	/* code to trigger an alarm with the message */
}

void wait(uint64_t delayTime)
{
	for(uint64_t i = 0; i < delayTime; ++i);
}
