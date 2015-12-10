
#include "../motor.c"

void moveToStock(){
	while(true)
	{
		if(SensorValue[touchSensor1] == 0){
			motor[motorB] = 10;
		}
		else {
			break;
		}
	}
	motor[motorB] = 0;
}


void remote_move(ubyte data)
{
	int gearTeeth = data;
	driveDistance(gearTeeth, -30, motorB);
}

//task main()
//{
//	moveToStock();
//	wait1Msec(1000);

//	motor[motorB] = -20;
//	wait1Msec(1000);
//	motor[motorB] = 0;

//	motor[motorA] = -10;
//	wait1Msec(1000);
//	motor[motorA] = 10;
//	wait1Msec(1000);
//	motor[motorA] = 0;

//}
