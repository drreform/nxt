#pragma config(Sensor, S2, colorSensorDown, sensorCOLORFULL)
#pragma config(Sensor, S3, colorSensorUp, sensorCOLORFULL)
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
void moveToTop();
void startPrint(int asciiCode);

task listenToBluetooth(){
	int receiver, method, payload;
	while(true)
	{
		receiver = messageParm[0];
		method = messageParm[1];
		payload = messageParm[2];
		ClearMessage();
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
			//ClearMessage();
		}
		wait1Msec(500);
	}
}

// Check if brick is under sensor
bool haveBrick(){
	if ( SensorValue[colorSensorDown] == REDCOLOR){
		PlaySound(soundBeepBeep);
		return true;
		} else {
		PlaySound(soundException);
		return false;
	}
}

bool brickInHead(){
	if ( SensorValue[colorSensorUp] == REDCOLOR){
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

void waitBeforeContinue(){
	PlaySound(soundException);
	PlaySound(soundException);
	while(SensorValue[touchOrigin] == 0)
	{
		wait1Msec(10);
	}
	PlaySound(soundBeepBeep);
	wait1Msec(500);
}

// Pick a brick from warehouse and plug it in
void setBrick(int i, int j)
{
	// how deep should it go
	const float down = 4;

	driveNipple(5.2,30,MoveMotor);

	short brickNotAvailable = 0;
	// If no brick wait and check again
	while (!haveBrick()){
		brickNotAvailable++;
		if(brickNotAvailable>=3){
			sendMessageWithParm(WEBSERVER, ERR_NO_BRICKS, 0);
			waitBeforeContinue();
			brickNotAvailable = 0;
			continue;
		}
		wait1Msec(5000);
	}

	// Loading
	short tryToLoad = 0;
LOADBRICK:
	moveToOrigin(); // calibrate

	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	moveToTop(); // calibrate
	wait1Msec(500);
	moveToOrigin(); // calibrate

	// check if brick is in head
	driveNipple(3.5 ,20,MoveMotor);
	if(!brickInHead()){
		tryToLoad++;
		if(tryToLoad >= 3){
			sendMessageWithParm(WEBSERVER, ERR_BRICK_NOT_PICKED, 0);
			waitBeforeContinue();
			tryToLoad = 0;
		}
		goto LOADBRICK;
	}

	///////////////////////////////////////////
	// move to plate
	driveNipple(i+4.5 ,20,MoveMotor);
	driveGear(1.2,10,MoveMotor, MoveGear);
	//driveGear(1,-20,MoveMotor, MoveGear);
	wait1Msec(500);

	// printing
	short tryToPlug = 0;
PRINT:
	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	moveToTop(); // calibrate
	wait1Msec(500);

	const float sensorToHead = 5.2;
	driveNipple(sensorToHead,30,MoveMotor);
	if(!haveBrick()){
		tryToPlug++;
		if (tryToPlug>=3){
			sendMessageWithParm(WEBSERVER, ERR_BRICK_NOT_PLUGGED, 0);
			waitBeforeContinue();
			tryToPlug = 0;
		}
		driveNipple(sensorToHead,-30,MoveMotor);
		goto PRINT;
	}

}

// Go through the vector, set bricks, and move conveyor after each row
void writeLetter(char* letter, int size)
{
	for(int i=0; i<size; i++)
	{
		nxtDisplayTextLine(2,"Index: %d", i);
		// move the conveyor
		if (i!=0 && i%5==0){
			sendMessageWithParm(CONVEYOR, CONVEYOR_MOVE, 1);
			PlaySound(soundBeepBeep);
			wait1Msec(500);
		}

		if (letter[i]=='1'){
			sendMessageWithParm(WEBSERVER, STT_PRINTING, i);
			setBrick(4-i%5,0);
			// Calibrate after each brick placement
			moveToOrigin();
		}
	}

}



// Move printer head to origin
void moveToOrigin(){
	// make sure touch sensor is untriggered
	//driveGear(5,30,MoveMotor, MoveGear);
	while(true)
	{
		if(SensorValue[touchOrigin] == 0)
			motor[MoveMotor] = -20;
		else
			break;
	}
	motor[MoveMotor] = 0;

	// Shift head to bricks inventory
	//driveGear(6,10,MoveMotor, MoveGear);
	PlaySound(soundShortBlip);
}

// Move head to idle position
void moveToTop(){
	while(true)
	{
		if(SensorValue[touchTop] == 0)
			motor[LiftMotor] = -100;
		else
			break;
	}
	motor[LiftMotor] = 0;
	// To keep the distance(height) at minimum
	//driveGear(5,30,LiftMotor, LiftGear);
	PlaySound(soundShortBlip);
}

//string x[25];
//void showLetterMatrix(string letter){
//	x = letter;
//	for(int i=0; i<25; i+=5){
//			nxtDisplayTextLine(i/5+2,"%s%s%s%s%s", x[i], x[i+1], x[i+2], x[i+3], x[i+4]);
//	}
//	return;
//}

void startPrint(int asciiCode){
	const char* letter = vectorLetter(asciiCode);
	// TODO Only accept range 48-57 65-90 for number and letters

	//char* letter = "1000010101";
	nxtDisplayTextLine(1,"Printing: %d", asciiCode);
	//showLetterMatrix(letter);
	//writeLetter(letter, 25);
}


task main()
{
	StartTask(listenToBluetooth);

	moveToTop();
	moveToOrigin();
	wait1Msec(500);

	//startPrint(65);

	while(true){wait10Msec(100);}
}
