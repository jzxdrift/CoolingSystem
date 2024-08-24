#include <stdint.h>

#include "plc.h"
#include "pid.h"
#include "utility.h"

static void sendControlSignalPLCtoCAN(uint16_t, float, CAN*);

void initializePLC(PLC *plc, float temp)
{
	/* initialize state machine */
	plc->ignitionState = IGNITION_OFF;
	plc->systemState = INIT;

	plc->desiredTemp = temp;
	plc->coolantTemp = 0.0;			/* sensor data */

	/* control outputs (0-100% PWM pump and fan duty cycle) */
	plc->pumpOutput = MIN_DUTY_CYCLE;
	plc->fanOutput = MIN_DUTY_CYCLE;
}

void runPLC(PLC *plc, PID *pid, CAN *canPacket)
{
	/* receive data from CAN bus */
	*canPacket = receiveCANData();

	/* process received data */
	if(canPacket->id == CAN_ID_TEMP_SENSOR)
		plc->coolantTemp = canPacket->data;
	else if(canPacket->id == CAN_ID_IGNITION_STATUS)
		plc->ignitionState = (int)canPacket->data;

	/* state machine operating logic */
	switch(plc->systemState)
	{
		case INIT:
			plc->systemState = plc->ignitionState == IGNITION_ON ? NORMAL : IDLE;
			break;

		case NORMAL:
		{
			if(plc->coolantTemp == TEMP_SENSOR_ERROR)		/* sensor malfunction safety check */
				plc->systemState = SENSOR_ERROR;
			else if(plc->coolantTemp > MAX_SAFE_TEMP)		/* over-temperature safety check */
				plc->systemState = OVER_TEMP;
			else
			{
				/* assume fan output is proportional to pump output */
				plc->pumpOutput = plc->fanOutput = calculatePID(pid, plc);

				/* send control signals over CAN bus */
				sendControlSignalPLCtoCAN(CAN_ID_PUMP_CONTROL, plc->pumpOutput, canPacket);
				sendControlSignalPLCtoCAN(CAN_ID_FAN_CONTROL, plc->fanOutput, canPacket);
			}

			/* switch to IDLE if ignition is off */
			if(plc->ignitionState == IGNITION_OFF)
				plc->systemState = IDLE;

			break;
		}

		case OVER_TEMP:
		{
			/* immediate cooling action required */
			plc->pumpOutput = plc->fanOutput = MAX_DUTY_CYCLE;

			/* send control signals over CAN bus */
			sendControlSignalPLCtoCAN(CAN_ID_PUMP_CONTROL, plc->pumpOutput, canPacket);
			sendControlSignalPLCtoCAN(CAN_ID_FAN_CONTROL, plc->fanOutput, canPacket);

			setAlarm("Over-Temperature Warning");

			/* switch back to NORMAL if temperature is under control */
			if(plc->coolantTemp <= plc->desiredTemp)
				plc->systemState = NORMAL;

			break;
		}

		case SENSOR_ERROR:
		{
			/* shut down all operations */
			plc->pumpOutput = plc->fanOutput = MIN_DUTY_CYCLE;

			/* send control signals over CAN bus */
			sendControlSignalPLCtoCAN(CAN_ID_PUMP_CONTROL, plc->pumpOutput, canPacket);
			sendControlSignalPLCtoCAN(CAN_ID_FAN_CONTROL, plc->fanOutput, canPacket);

			setAlarm("Temperature Sensor Failure");

			/* remain in SENSOR_ERROR state until manual reset or condition change */
			if(plc->coolantTemp != TEMP_SENSOR_ERROR)
				plc->systemState = NORMAL;

			break;
		}

		case IDLE:
		{
			/* ignition OFF control logic */
			plc->pumpOutput = plc->fanOutput = MIN_DUTY_CYCLE;

			/* send control signals over CAN bus */
			sendControlSignalPLCtoCAN(CAN_ID_PUMP_CONTROL, plc->pumpOutput, canPacket);
			sendControlSignalPLCtoCAN(CAN_ID_FAN_CONTROL, plc->fanOutput, canPacket);

			/* switch to NORMAL if ignition is turned on */
			if(plc->ignitionState == IGNITION_ON)
				plc->systemState = NORMAL;

			break;
		}
	}
}

static void sendControlSignalPLCtoCAN(uint16_t id, float data, CAN *packet)
{
	/* send control signals over CAN bus */
	packet->id = id;
	packet->data = data;
	sendCANData(packet);
}