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

//-------------getBarcode-------------//
int barCode[4];
int * getBarcode(){
	barCode[0]=1;
	int i=1;

	while (i < 4 ) {

		while ( ( SensorValue [ colorSensor ]!= REDCOLOR ) &&
			( SensorValue [ colorSensor ]!= BLUECOLOR ) &&
		( SensorValue [ colorSensor ]!= WHITECOLOR )
		&&  SensorValue [ colorSensor ]!= YELLOWCOLOR &&  SensorValue [ colorSensor ]!= GREENCOLOR
		) {
			// do nothing -- just wait for red or blue color
		}

		if (SensorValue [ colorSensor ]!=  barCode[i-1] &&  SensorValue [ colorSensor ]!= BLACKCOLOR){
			barCode[i]= SensorValue [ colorSensor ];
			i=i+1;
		}

	}
	return 	barCode;
}

//-------------setBrick-------------//

void setBrick(int i, int j)
{


// brick check error 1
	const int down = 4.5;

	//move down & up
	// loading
	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);

	// move to plate
	driveNipple(i+6 ,20,MoveMotor);
	//driveGear(1,-20,MoveMotor, MoveGear);
	wait1Msec(500);

	// printing
	driveGear(down,15,LiftMotor, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,LiftMotor, LiftGear);
	wait1Msec(500);

	// move back to reload
	//driveNipple(i+7 ,-30,MoveMotor);
	//wait1Msec(500);

	//driveNipple(i+7 , -20,MoveMotor);
	//wait1Msec(500);
	//driveNipple(j ,30,MoveMotor);
	//wait1Msec(500);

}

//-------------writeLetter-------------//

void writeLetter(int letter[], int size)
{
	for(int i=0; i<size; i++)
	{
		PlaySound(soundBeepBeep);
		if (i!=0 && i%5==0){
			//moveConveyor(0x01);
			wait1Msec(500);
		}

		if (letter[i]==1)
		{
			setBrick(i%5,0);
		}
		if (letter[i]!=0){
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

//--------move printer head to origin--------//
void moveToOrigin(){
	while(true)
	{
		if(SensorValue[touchOrigin] == 0){
			motor[MoveMotor] = -20;
		}
		else {
			break;
		}
	}
	motor[MoveMotor] = 0;
	wait1Msec(50);
	driveGear(1,10,MoveMotor, MoveGear);
	PlaySound(soundBlip);
}

void moveToTop(){
	while(true)
	{
		if(SensorValue[touchTop] == 0){
			motor[LiftMotor] = -20;

		}
		else {
			break;
		}
	}
	motor[LiftMotor] = 0;
}


//-------------main-------------//

task main()
{
	wait1Msec(1000);

	moveToTop();
	wait1Msec(50);
	driveGear(7,30,LiftMotor, LiftGear);
	PlaySound(soundBlip);
	driveGear(5,30,MoveMotor, MoveGear);
	PlaySound(soundBlip);

	moveToOrigin();

	wait1Msec(500);


	int letter[5] =
	{
		0,0,0,1,1
	};
//moveConveyor(0x025);
//getBarcode();
	writeLetter(letter, sizeof(letter));
//	driveNipple(5,30,LiftMotor);

}
