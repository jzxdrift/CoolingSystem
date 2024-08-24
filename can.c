#include "can.h"

void initializeCAN(CAN *canPacket)
{
	/* initialize CAN packet */
	canPacket->id = 0;
	canPacket->data = 0.0;
}

CAN receiveCANData(void)
{
	CAN canPacket = {0, 0.0};
	/* code to receive data over CAN bus */
	return canPacket;
}

void sendCANData(const CAN *canPacket)
{
	/* code to send data over CAN bus */
}