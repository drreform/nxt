#pragma config(Sensor, S1,     touchSensor1,   sensorTouch)
#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */


#include "../motor.c"
#include "conveyor_bt.c"

#define UnloadMotor motorA
#define TransportMotor motorB

// Move conveyor to origin
void moveToStock(){
	while(true)
	{
		if(SensorValue[touchSensor1] == 0){
			motor[motorB] = 40;
		}
		else {
			break;
		}
	}
	motor[motorB] = 0;
}

// Unload the plate
void unload(int power)
{
	nMotorEncoder[UnloadMotor] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[UnloadMotor] = 20; //set the target stoping position
	motor[UnloadMotor] = -power;

	while (nMotorRunState[UnloadMotor] != runStateIdle){}
	motor[UnloadMotor] = 0; //turn motor off
	wait1Msec(100);

	// go back
	nMotorEncoder[UnloadMotor] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[UnloadMotor] = 20; //set the target stoping position
	motor[UnloadMotor] = power;

	while (nMotorRunState[UnloadMotor] != runStateIdle){}
	motor[UnloadMotor] = 0; //turn motor off
}

// Procedure called by remotely by printer
void remote_move(ubyte data)
{
	int gearTeeth = data;
	//driveDistance(gearTeeth, -30, motorB);
}


task main()
{
	bNxtLCDStatusDisplay = true;
	memset(nRcvHistogram,  0, sizeof(nRcvHistogram));
	memset(nXmitHistogram, 0, sizeof(nXmitHistogram));
	wait1Msec(2000);
	//StartTask(sendMessages);

	moveToStock();

	const float Origin2Printer = 26;

	driveNipple(Origin2Printer ,-20,TransportMotor);


	//while(true){
	//	if(SensorValue[touchSensor1] == 1){
	//		unload(3);
	//		//driveDistance(20, -30, motorB);
	//	}
	//	wait1Msec(1);
	//}

	return;
}
