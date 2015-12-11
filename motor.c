/*
Movement functions
*/

// Move per nipple
void driveNipple(int noOfNipples , int power, short motorX)
{

	int tickGoal= noOfNipples * 15*2.5 ; // gear has 24 head = 24mm which is equal to 360*; each nipple is equal to 2,5mm

	nMotorEncoder[motorX] = 0;  //clear the LEGO motor encoders
	nMotorEncoderTarget[motorX] = tickGoal; //set the target stoping position
	motor[motorX] = power;

	while (nMotorRunState[motorX] != runStateIdle){}
	motor[motorX] = 0; //turn motor off
	PlaySound(soundBlip);
}

// Move per gear tooth
void driveGear(int teeth , int power, short motorX)
{

	int tickGoal= (360*teeth)/24 ; // gear has 24 head = 24mm which is equal to 360*; each nipple is equal to 2,5mm

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
