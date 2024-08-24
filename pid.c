#include <stdint.h>

#include "pid.h"
#include "plc.h"

static float clamp(float, int8_t, int8_t);

void initializePID(PID *pid)
{
	/* initialize PID */
	pid->previousError = 0.0;
	pid->integral = 0.0;
}

float calculatePID(PID *pid, const PLC *plc)
{
	/* PID control output */
	float error = plc->desiredTemp - plc->coolantTemp;
	pid->integral += error;

	float derivative = error - pid->previousError;

	float output = (float)KP * error + (float)KI * pid->integral + (float)KD * derivative;
	pid->previousError = error;

	/* adjust pump and fan output based on PID (0-100% PWM duty cycle) */
	output = clamp(output, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

	return output;
}

static float clamp(float value, int8_t min, int8_t max)
{
	/* clamp value between minimum and maximum */
	if((int8_t)value <= min)
		value = min;
	else if((int8_t)value >= max)
		value = max;

	return value;
}