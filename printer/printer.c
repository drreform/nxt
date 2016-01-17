#pragma config(Sensor, S1, colorSensor, sensorCOLORFULL)
#pragma config(Sensor, S3, touchTop, sensorTouch)
#pragma config(Sensor, S4, touchOrigin, sensorTouch)
//#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */

#include "../constants.c"
#include "../motor.c"
#include "printer_bt.c"

// Constants
#define LiftMotor motorA
#define MoveMotor motorC
const float LiftGear = 8/3.0;
const float MoveGear = 24;

void moveToOrigin();

bool haveBrick(){
	driveNipple(4.2,30,MoveMotor);
	bool check = false;
	if ( SensorValue[colorSensor] == REDCOLOR){
		check = true;
		PlaySound(soundBeepBeep);
	} else {
		while(true){
			PlaySound(soundException);
			wait1Msec(1000);
		}
	}
	driveNipple(4.2,-30,MoveMotor);

	return check;
}

//-------------setBrick-------------//
void setBrick(int i, int j)
{
	// how deep should it go
	const float down = 3.3;

	// Check ERROR1, if no brick wait 2,5 sec and check again
	bool error1 = haveBrick();
	while (error == false ){
	// send to server error1 message
	wait1Msec(2500);
	error1 = haveBrick();
	}
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
	driveGear(down,15,LiftMotor, LiftGear);
	//wait1Msec(10);

	// Try few times to make sure it is plugged in firmly
	float vibr = 1;
	int speed = 100;
	driveGear(vibr,-speed,LiftMotor, LiftGear);
	wait1Msec(50);
	driveGear(vibr,speed,LiftMotor, LiftGear);
	driveGear(vibr,-speed,LiftMotor, LiftGear);
	wait1Msec(50);
	driveGear(vibr,speed,LiftMotor, LiftGear);

	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);
	haveBrick();
}

//-------------writeLetter-------------//

void writeLetter(int* letter, int size)
{
	for(int i=0; i<size; i++)
	{
		// move the conveyor
		if (i!=0 && i%5==0){
			moveConveyor(0x01);
			PlaySound(soundBeepBeep);
			wait1Msec(500);
		}

		if (letter[i]==1){
			setBrick(i%5,0);
			// Calibrate after each brick placement
			moveToOrigin();
		}
	}

}

//-------------checkBrick Error 1 & 2-------------//

bool checkBrick()
{
	if 	(SensorValue [ colorSensor ] == WHITECOLOR){
	return true;
	}
	else
		return false;

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
			motor[LiftMotor] = -20;
		else
			break;
	}
	motor[LiftMotor] = 0;
	// To keep the distance(height) at minimum
	driveGear(2,30,LiftMotor, LiftGear);
	PlaySound(soundShortBlip);
}


//-------------main-------------//

task main()
{
	wait1Msec(1000);

	moveConveyor(0x01);
	return;

	moveToTop();
	moveToOrigin();
	wait1Msec(500);


	int letter[10] =
	{
		1,0,0,0,1,
		1,0,0,0,0
	};

	writeLetter(letter, sizeof(letter));
}
