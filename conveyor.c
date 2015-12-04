#include "motor.c"
#pragma config(Sensor, S1,     touchSensor1,         sensorTouch)
#pragma config(Sensor, S4,     touchSensor4,         sensorTouch)

void moveToStock(){
	while(true)
	{
		if(SensorValue(touchSensor1) == 0){
			motor[motorB] = 10;
		}
		else {
			break
		}
	}
	motor[motorB] = 0;
}

task main()
{
	moveToStock();
	wait1Msec(1000);

	motor[motorB] = -20;
	wait1Msec(1000);
	motor[motorB] = 0;

	motor[motorA] = -10;
	wait1Msec(1000);
	motor[motorA] = 10;
	wait1Msec(1000);
	motor[motorA] = 0;

}
