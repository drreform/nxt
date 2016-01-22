#pragma config(Sensor, S2,		 touchSensor1,	 sensorTouch)
#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */


#include "motor.c"
#include "constants.h"

#define UnloadMotor motorB
#define TransportMotor motorC

void conveyor_move(int payload);
void moteToPrinterAndSendJob(int letter);

task listenToBluetooth(){
	int receiver, method, payload;
	while(true)
	{
		receiver = messageParm[0];
		method = messageParm[1];
		payload = messageParm[2];
		if(receiver != 0 || method	!= 0 || payload != 0){
			PlaySound(soundBlip);
			switch(method){
			case CONVEYOR_JOB_START:
				nxtDisplayBigTextLine(2,"Job: %d", payload);
				moteToPrinterAndSendJob(payload);
				break;
			case CONVEYOR_MOVE:
				conveyor_move(payload);
				break;
			default:
				PlaySound(soundException);
				// method not supported
			}
			ClearMessage();
		}
		wait1Msec(500);
	}
}

// Move conveyor to origin
void moveToStock(){
	while(true)
	{
		if(SensorValue[touchSensor1] == 0){
			motor[TransportMotor] = 40;
		}
		else {
			break;
		}
	}
	motor[TransportMotor] = 0;
}

// Unload the plate
void unload(int power)
{
	nMotorEncoder[UnloadMotor] = 0;	 //clear the LEGO motor encoders
	nMotorEncoderTarget[UnloadMotor] = 20; //set the target stoping position
	motor[UnloadMotor] = -power;

	while (nMotorRunState[UnloadMotor] != runStateIdle){}
	motor[UnloadMotor] = 0; //turn motor off
	wait1Msec(100);

	// go back
	nMotorEncoder[UnloadMotor] = 0;	 //clear the LEGO motor encoders
	nMotorEncoderTarget[UnloadMotor] = 20; //set the target stoping position
	motor[UnloadMotor] = power;

	while (nMotorRunState[UnloadMotor] != runStateIdle){}
	motor[UnloadMotor] = 0; //turn motor off
}

// Procedure called remotely by printer
void conveyor_move(int payload)
{
	int speed = -15;
	if(payload<0)
		speed = abs(speed);
	int gearTeeth = abs(payload);
	driveNipple(gearTeeth, speed, TransportMotor);
}

void moteToPrinterAndSendJob(int letter){
		// move to printer
		driveNipple(27.3, -20, TransportMotor);
		// tell printer what should be done
		sendMessageWithParm(PRINTER, PRINTER_PRINT, letter);
}


task main()
{
	StartTask(listenToBluetooth);

	moveToStock();

	const float Origin2Printer = 26;

	//driveNipple(Origin2Printer ,-20,TransportMotor);


	//while(true){
	//	if(SensorValue[touchSensor1] == 1){
	//		unload(3);
	//		//driveDistance(20, -30, motorB);
	//	}
	//	wait1Msec(1);
	//}


	while(true){wait10Msec(100);}
	return;
}
