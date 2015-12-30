#pragma config(Sensor, S1, colorSensor, sensorCOLORFULL)
#pragma config(Sensor, S3, touchTop, sensorTouch)
#pragma config(Sensor, S4, touchOrigin, sensorTouch)
//#pragma platform(NXT)
/* DON'T TOUCH THE ABOVE CONFIGURATION */

#include "../constants.c"
#include "../motor.c"
#include "printer_bt.c"

#define motorLift motorA
#define motorMove motorC

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
	driveGear(down,15,motorLift, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,motorLift, LiftGear);
	wait1Msec(500);

	// move to plate
	driveNipple(i+6 ,20,motorMove);
	//driveGear(1,-20,motorMove, MoveGear);
	wait1Msec(500);

	// printing
	driveGear(down,15,motorLift, LiftGear);
	wait1Msec(500);
	driveGear(down,-30,motorLift, LiftGear);
	wait1Msec(500);

	// move back to reload
	//driveNipple(i+7 ,-30,motorMove);
	//wait1Msec(500);

	//driveNipple(i+7 , -20,motorMove);
	//wait1Msec(500);
	//driveNipple(j ,30,motorMove);
	//wait1Msec(500);

}

//-------------writeLetter-------------//

void writeLetter(int * letterBad)
{


	int letter[5] =
	{
		1,
		0,
		1,
		0,
		1
	};

	int s= sizeof(letter);

	for(int i = 0; i<s; i++)
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
			motor[motorMove] = -20;
		}
		else {
			break;
		}
	}
	motor[motorMove] = 0;
	wait1Msec(50);
	driveGear(1,10,motorMove, MoveGear);
	PlaySound(soundBlip);
}

void moveToTop(){
	while(true)
	{
		if(SensorValue[touchTop] == 0){
			motor[motorLift] = -20;

		}
		else {
			break;
		}
	}
	motor[motorLift] = 0;
}


task adjust(){


}


//-------------main-------------//

task main()
{
	wait1Msec(1000);

	moveToTop();
	wait1Msec(50);
	driveGear(7,30,motorLift, LiftGear);
	PlaySound(soundBlip);
	driveGear(5,30,motorMove, MoveGear);
	PlaySound(soundBlip);

	moveToOrigin();

	wait1Msec(500);


	int letter[6] =
	{
		0,
		0,
		0,
		1,
		1,
		1,
	};
//moveConveyor(0x025);
//getBarcode();
	writeLetter(letter);
//	driveNipple(5,30,motorLift);

}
