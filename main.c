#include <stdlib.h>

#include "plc.h"
#include "pid.h"
#include "can.h"
#include "utility.h"

void main(int argc, char *argv[])
{
	/* using command line argument as temperature setpoint */
	PLC plc;
	initializePLC(&plc, argc > 1 ? atof(argv[1]) : 70.0);

	PID pid;
	initializePID(&pid);

	CAN can;
	initializeCAN(&can);

	while(1)
	{
		runPLC(&plc, &pid, &can);

		/* delay for the next cycle in ms */
		wait(1000);
	}
}