#ifndef PLC_H
#define PLC_H

#include "can.h"
#include "pid.h"

#define MAX_SAFE_TEMP			(80)		/* maximum safe operating temperature in Celsius */
#define IGNITION_ON			(1)		/* ignition switch is on */
#define IGNITION_OFF			(0)		/* ignition switch is off */
#define TEMP_SENSOR_ERROR		(-1)		/* error code for sensor malfunction */

#define MIN_DUTY_CYCLE			(0)		/* minimum pump/fan PWM duty cycle */
#define MAX_DUTY_CYCLE			(100)		/* maximum pump/fan PWM duty cycle */

typedef enum
{
	INIT,
	NORMAL,
	OVER_TEMP,
	SENSOR_ERROR,
	IDLE			/* ignition off */
} States;

typedef struct plc_t
{
	int ignitionState;
	States systemState;
	float desiredTemp;
	float coolantTemp;
	float pumpOutput;
	float fanOutput;
} PLC;

void initializePLC(PLC*, float);
void runPLC(PLC*, PID*, CAN*);

#endif
