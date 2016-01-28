#pragma config(Sensor, S1, touchOrigin, sensorTouch)
#pragma config(Sensor, S2, touchOnTop, sensorTouch)

#include "constants.h"
#include "motor.c"


#define PushMotor motorA
#define LiftMotor motorB
const float LiftGear = 24;  // 8/3.0;
const float PushGear = 24;


void moveToOrigin(){
	// make sure touch sensor is untriggered
	while(true)
	{
		if(SensorValue[touchOrigin] == 0)
			motor[PushMotor] = -20;
		else
			break;
	}
	motor[PushMotor] = 0;


	driveGear(0.5,10,PushMotor, PushGear);
	PlaySound(soundShortBlip);
}

void moveLiftToOrigin(){
	// make sure touch sensor is untriggered
	while(true)
	{
		if(SensorValue[touchOnTop] == 0)
			motor[LiftMotor] = -20;
		else
			break;
	}
	motor[LiftMotor] = 0;


	driveGear(0.5,10,LiftMotor, LiftGear);
	PlaySound(soundShortBlip);
}



void moveToPushPlate(){
	driveGear(44,20,PushMotor, PushGear);

}

void loadPlate(){
	driveGear(25,20,LiftMotor,LiftGear)	;
 	driveGear(23,20,LiftMotor,LiftGear)	;
 	//driveGear(60,100,LiftMotor,LiftGear)	;

 	//repeat punching the plate
 	int i = 0;
 	while(i<5){
 	driveGear(10,-40,LiftMotor,LiftGear)	;
 	driveGear(13,40,LiftMotor,LiftGear)	;
 	i++;
			}
	driveGear(5,-30,LiftMotor,LiftGear)	;
 	}



//Main Program
task main()
{

//push a plate to the car
//driveGear(5,30,PushMotor, PushGear);
//moveToOrigin();
//moveToPushPlate();
//moveToOrigin();

//load the plate on conveyor
moveLiftToOrigin();
loadPlate();
moveLiftToOrigin();

}
