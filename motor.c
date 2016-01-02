/*
Movement functions
*/

// Move per gear degree
void driveDegree(int degree , int power, short motorX)
{
	nMotorEncoder[motorX] = 0;  //clear the motor encoder
	nMotorEncoderTarget[motorX] = degree; //set the target stoping position
	motor[motorX] = power;

	while (nMotorRunState[motorX] != runStateIdle){} // wait till it reaches the target
	motor[motorX] = 0; //turn motor off
}

// Move per nipple
void driveNipple(float noOfNipples , int power, short motorX)
{
	float gearToNipple = 12/2.5; // each nipple is 2.5 teeth
	int target = (360*noOfNipples)/gearToNipple;

	driveDegree(target, power, motorX);
}

// Move per gear tooth
void driveGear(float teeth , int power, short motorX, float gearRatio)
{
	int target = (360*teeth)/gearRatio;

	driveDegree(target, power, motorX);
}
