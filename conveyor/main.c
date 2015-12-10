#pragma config(Sensor, S1,     touchSensor1,   sensorTouch)
#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */

#include "conveyor.c"
#include "conveyor_bt.c"

task main()
{
	bNxtLCDStatusDisplay = true;
	memset(nRcvHistogram,  0, sizeof(nRcvHistogram));
	memset(nXmitHistogram, 0, sizeof(nXmitHistogram));
	wait1Msec(2000);
	sendMessages();
	return;
}
