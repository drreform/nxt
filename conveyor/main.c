#pragma config(Sensor, S1,     touchSensor1,   sensorTouch)
#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */

#include "conveyor.c"
#include "conveyor_bt.c"

#define unloadMotor motorA

void unload(int power)
{
	nMotorEncoder[unloadMotor] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[unloadMotor] = 20; //set the target stoping position
	motor[unloadMotor] = -power;

	while (nMotorRunState[unloadMotor] != runStateIdle){}
	motor[unloadMotor] = 0; //turn motor off
	wait1Msec(100);

	// go back
	nMotorEncoder[unloadMotor] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[unloadMotor] = 20; //set the target stoping position
	motor[unloadMotor] = power;

	while (nMotorRunState[unloadMotor] != runStateIdle){}
	motor[unloadMotor] = 0; //turn motor off
}

task main()
{
	bNxtLCDStatusDisplay = true;
	memset(nRcvHistogram,  0, sizeof(nRcvHistogram));
	memset(nXmitHistogram, 0, sizeof(nXmitHistogram));
	wait1Msec(2000);
	StartTask(sendMessages);


	while(true){
		if(SensorValue[touchSensor1] == 1){
			unload(3);
			//driveDistance(20, -30, motorB);
		}
		wait1Msec(1);
	}

	return;
}
