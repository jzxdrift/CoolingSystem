#ifndef PID_H
#define PID_H

/* PLC forward declaration */
typedef struct plc_t PLC;

#define KP	(1.0)		/* proportional gain */
#define KI	(0.1)		/* integral gain */
#define KD	(0.05)		/* derivative gain */

typedef struct
{
	float previousError;
	float integral;
} PID;

void initializePID(PID*);
float calculatePID(PID*, const PLC*);

#endif