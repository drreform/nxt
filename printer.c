#include "motor.c"

void setBrick(int i, int j)
{

	driveDistance(7,-30,motorB);
	wait1Msec(500);							 //Stop in between each command to prevent momentum causing wheel skid.
	driveDistance(7,30,motorB);
	wait1Msec(500);

	driveDistance(i+5 ,-30,motorC);
	wait1Msec(500);

	//driveDistance(j ,-30,motorC); // for the 2D movement
	//wait1Msec(500);

	driveDistance(8,-90,motorB);
	wait1Msec(500);							 //Stop in between each command to prevent momentum causing wheel skid.
	driveDistance(8,30,motorB);
	wait1Msec(500);

	driveDistance(i+5 , 30,motorC);
	wait1Msec(500);
	//driveDistance(j ,30,motorC);
	//wait1Msec(500);

}

//void writeLetter(int letter)
//{
//	int s= letter.length;

// for(int i = 0; i< s; i++)
//	 {
//		 if (IntArray[i]==1)
//		 {
//			 setBrick(i,0);
//			 }
//	}

//}

task main()
{

	int IntArray[4] =
	{
		1,
		0,
		1,
		1,
	};

	for(int i = 0; i< 4; i++)
	{
		if (IntArray[i]==1)
		{
			setBrick(i,0);
		}
	}
}
