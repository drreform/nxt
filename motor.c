/*
	Movement functions
*/

// Move per gear tooth
void driveDistance(int noOfNipples , int power, short motorB)
{

	int tickGoal= noOfNipples * 15*2.5 ; // gear has 24 head = 24mm which is equal to 360*; each nipple is equal to 2,5mm

	nMotorEncoder[motorB] = 0;  //clear the LEGO motor encoders
  nMotorEncoderTarget[motorB] = tickGoal; //set the target stoping position
  motor[motorB] = power;

  while (nMotorRunState[motorB] != runStateIdle){}
  motor[motorB] = 0; //turn both motors off

}
