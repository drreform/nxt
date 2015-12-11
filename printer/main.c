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


	//move down & up
	driveGear(11,-10,motorLift);
	wait1Msec(500);

	driveGear(11,10,motorLift);
	wait1Msec(500);

	driveNipple(i+7 ,20,motorMove);
	wait1Msec(500);

	//driveNipple(j ,-30,motorMove); // for the 2D movement
	//wait1Msec(500);

	driveGear(12,-10,motorLift);
	wait1Msec(500);              //Stop in between each command to prevent momentum causing wheel skid.

	driveGear(12,10,motorLift);
	wait1Msec(500);

	driveNipple(i+7 , -20,motorMove);
	wait1Msec(500);
	//driveNipple(j ,30,motorMove);
	//wait1Msec(500);

}

//-------------writeLetter-------------//

void writeLetter(int * letter)
{
	int s= sizeof(letter);

	for(int i = 0; i< s; i++)
	{
		if (i!=0 && i%1==0){
			//moveConveyor(0x01);
			wait1Msec(500);
		}

		if (letter[i]==1)
		{
			setBrick(i,0);
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
			motor[motorMove] = -10;
		}
		else {
			break;
		}
	}
	motor[motorMove] = 0;
	PlaySound(soundBlip);
}

void moveToTop(){
	while(true)
	{
		if(SensorValue[touchTop] == 0){
			motor[motorLift] = 10;

		}
		else {
			break;
		}
	}
	motor[motorLift] = 0;
	PlaySound(soundBlip);
}



//-------------main-------------//

task main()
{
	moveToTop();
	moveToOrigin();
	driveDegree(4,10,motorMove);



	wait1Msec(1000);
	int letter[4] =
	{
		1,
		1,
		1,
		1,
	};
//moveConveyor(0x025);
//getBarcode();
	writeLetter(letter);
//	driveNipple(5,30,motorLift);

}
