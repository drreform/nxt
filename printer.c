#pragma config(Sensor, S2, colorSensor, sensorCOLORFULL)
#pragma config(Sensor, S4, touchTop, sensorTouch)
#pragma config(Sensor, S1, touchOrigin, sensorTouch)
//#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */

#include "constants.h"
#include "ascii.c"
#include "motor.c"

// Constants
#define LiftMotor motorA
#define MoveMotor motorC
const float LiftGear = 8/3.0;
const float MoveGear = 24;

void moveToOrigin();
void startPrint(int asciiCode);

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
				case PRINTER_PRINT:
				// print the letter
				startPrint(payload);
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

// Check if brick is under sensor
bool haveBrick(){
	if ( SensorValue[colorSensor] == REDCOLOR){
		PlaySound(soundBeepBeep);
		return true;
	} else {
		PlaySound(soundException);
		return false;
	}
}

// Plug the brick to the plate
void plugInBrick(float down){
	// loading
	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);

	driveNipple(5.2,30,MoveMotor);
	bool have = haveBrick();
	driveNipple(5.2,-30,MoveMotor);
	if(!have){
		sendMessageWithParm(WEBSERVER, ERR_BRICK_NOT_PLUGGED, 0);
		// loading
		driveGear(down,15,LiftMotor, LiftGear);
		wait1Msec(500);
		driveGear(down,-30,LiftMotor, LiftGear);
		wait1Msec(500);
		driveNipple(5.2,30,MoveMotor);
		bool have = haveBrick();
		if(!have){
			// find another way
		}
	  driveNipple(5.2,-30,MoveMotor);
	}

	// Try few times to make sure it is plugged in firmly
	//driveGear(down,15,LiftMotor, LiftGear);
	//float vibr = 1;
	//int speed = 100;
	//driveGear(vibr,-speed,LiftMotor, LiftGear);
	//wait1Msec(50);
	//driveGear(vibr,speed,LiftMotor, LiftGear);
	//driveGear(vibr,-speed,LiftMotor, LiftGear);
	//wait1Msec(50);
	//driveGear(vibr,speed,LiftMotor, LiftGear);
}

// Pick a brick from warehouse and plug it in
void setBrick(int i, int j)
{
	// how deep should it go
	const float down = 3;

	driveNipple(5.2,30,MoveMotor);
	// If no brick wait 2,5 sec and check again
	while (!haveBrick()){
		// send to server error message
	 	sendMessageWithParm(WEBSERVER, ERR_NO_BRICKS, 0);
		wait1Msec(2500);
	}
	driveNipple(5.2,-30,MoveMotor);

	// loading
	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);
	moveToOrigin(); // calibrate

	// move to plate
	driveNipple(i+5 ,20,MoveMotor);
	driveGear(0.8,10,MoveMotor, MoveGear);
	//driveGear(1,-20,MoveMotor, MoveGear);
	wait1Msec(500);

	// printing
	//driveGear(down,15,LiftMotor, LiftGear);
	//wait1Msec(10);
	plugInBrick(down);

	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);
	//haveBrick();
}

// Go through the vector, set bricks, and move conveyor after each row
void writeLetter(char* letter, int size)
{
	for(int i=0; i<size; i++)
	{
		// move the conveyor
		if (i!=0 && i%5==0){
			sendMessageWithParm(CONVEYOR, CONVEYOR_MOVE, 1);
			PlaySound(soundBeepBeep);
			wait1Msec(500);
		}

		if (letter[i]=='1'){
			sendMessageWithParm(WEBSERVER, STT_PRINTING, i);
			setBrick(i%5,0);
			// Calibrate after each brick placement
			moveToOrigin();
		}
	}

}



// Move printer head to origin
void moveToOrigin(){
	// make sure touch sensor is untriggered
	driveGear(5,30,MoveMotor, MoveGear);
	while(true)
	{
		if(SensorValue[touchOrigin] == 0)
			motor[MoveMotor] = -20;
		else
			break;
	}
	motor[MoveMotor] = 0;

	// Shift head to bricks inventory
	driveGear(0.5,10,MoveMotor, MoveGear);
	PlaySound(soundShortBlip);
}

// Move head to idle position
void moveToTop(){
	while(true)
	{
		if(SensorValue[touchTop] == 0)
			motor[LiftMotor] = -10;
		else
			break;
	}
	motor[LiftMotor] = 0;
	// To keep the distance(height) at minimum
	driveGear(5,30,LiftMotor, LiftGear);
	PlaySound(soundShortBlip);
}

void startPrint(int asciiCode){
	//char* letter = vectorLetter(asciiCode);
	char* letter = "100001";
	nxtDisplayTextLine(1,"Printing: %d", asciiCode);
	writeLetter(letter, sizeof(letter));
}


task main()
{
	StartTask(listenToBluetooth);

	moveToTop();
	moveToOrigin();
	wait1Msec(500);

	while(true){wait10Msec(100);}
}
