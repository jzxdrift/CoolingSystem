#ifndef CAN_H
#define CAN_H

#include <stdint.h>

#define CAN_ID_TEMP_SENSOR		(0x100)		/* CAN ID for temperature sensor data */
#define CAN_ID_IGNITION_STATUS		(0x101)		/* CAN ID for ignition status data */
#define CAN_ID_PUMP_CONTROL		(0x200)		/* CAN ID for controlling the pump */
#define CAN_ID_FAN_CONTROL		(0x201)		/* CAN ID for controlling the fan */

typedef struct
{
	uint16_t id;
	float data;
} CAN;

void initializeCAN(CAN*);
CAN receiveCANData(void);
void sendCANData(const CAN*);

#endif
