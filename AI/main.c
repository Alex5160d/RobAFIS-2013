#pragma config(Sensor, S1,     left_sensor,    sensorCOLORFULL)
#pragma config(Sensor, S2,     right_sensor,   sensorCOLORFULL)
#pragma config(Sensor, S3,     light_sensor,   sensorLightActive)
#pragma config(Sensor, S4,     sonarSensor,    sensorSONAR)

void goStart(int rotation);
void goSPEfromSEandViceVersa();

#include "music.c"

#include "global_variables.h"
#include "primary_tasks.c"
#include "composed_tasks.c"
#include "ia.c"


task main()
{
	buildEnvironment();

	waitInMilliseconds(200);	// Let the technician go
	ClearTimer(T4);

	// Open the arm while moving
	StartTask(deployArm);

	/* Let the motors be regulated, so that they move at the same speed */
	nMotorPIDSpeedCtrl[motorB] = mtrSpeedReg;
	nMotorPIDSpeedCtrl[motorC] = mtrSpeedReg;

	// go SE
	goSEfromStart();
	// We're now between the to stocks with a ball
	while(true)
	{
		// go to the exit stocks from entry stocks
		goSAs();
		// go to the entry stocks from exit stocks
		goSEs();
	}
}
