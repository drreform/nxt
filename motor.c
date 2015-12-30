/*
Movement functions
*/

// Move per nipple
void driveNipple(float noOfNipples , int power, short motorX)
{
	float gearToNipple = 12/2.5; // each nipple is 2.5 teeth
	int tickGoal= (360*noOfNipples)/gearToNipple;

	nMotorEncoder[motorX] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[motorX] = tickGoal; //set the target stoping position
	motor[motorX] = power;

	while (nMotorRunState[motorX] != runStateIdle){}
	motor[motorX] = 0; //turn motor off
	PlaySound(soundBlip);
}

// Move per gear tooth
void driveGear(float teeth , int power, short motorX, float gearBox)
{

	int tickGoal= (360*teeth)/gearBox;

	nMotorEncoder[motorX] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[motorX] = tickGoal; //set the target stoping position
	motor[motorX] = power;

	while (nMotorRunState[motorX] != runStateIdle){}
	motor[motorX] = 0; //turn motor off
  PlaySound(soundBlip);
}

// Move per gear degree
void driveDegree(int degree , int power, short motorX)
{
	nMotorEncoder[motorX] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[motorX] = degree; //set the target stoping position
	motor[motorX] = power;

	while (nMotorRunState[motorX] != runStateIdle){}
	motor[motorX] = 0; //turn motor off
  PlaySound(soundBlip);
}
