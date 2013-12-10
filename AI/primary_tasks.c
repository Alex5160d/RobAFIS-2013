/* ************************************************************************** *
 * Simple tasks: particular event/basic action
 * ************************************************************************** */

void adjustPrehension();

// During this time the robot will move slowly to adjust its position on the line after a rotation
task chronoTask()
{
	ClearTimer(T1);
 	while (time1[T1] < 200);
 	robot.line_stop = true;
}

// Using this task the robot will detect its start area
task sensorsStartTask()
{
	ClearTimer(T1);
 	while(true) 
 	{
 		// Using the line
		if((robot.left_col != BLACK) || (robot.right_col != BLACK))
			ClearTimer(T1);
		if (time1[T1] > 20) 
		{
		  wait1Msec(200);
		  break;
		}
		// Or the sonar
	  	if (SensorValue[sonarSensor] <= SON_DIST)
	    	break;
	}

	// magic number after the detection, because magic numbers are everything
	wait1Msec(400);
 	robot.line_stop = true;
}

/*******************************************		Maintenance functions		*******************************/

// Pause and resume the robot for maintenance
task maintenance()
{
	// freeze all tasks by taking the CPU time
 	hogCPU();	
 	// We save the current work and pause it
 	int motA = motor[motorA];
 	int motB = motor[motorB];
 	int motC = motor[motorC];
 	motor[motorA] = 0;
 	motor[motorB] = 0;
 	motor[motorC] = 0;

 	nxtDisplayCenteredTextLine(2, "Maintenance:");
 	nxtDisplayCenteredTextLine(4, "Bouton principal");
 	nxtDisplayCenteredTextLine(5, "	pour reprendre");

 	while(nNxtButtonPressed != (short) kEnterButton);

 	nxtDisplayClearTextLine(2);
 	nxtDisplayClearTextLine(4);
 	nxtDisplayClearTextLine(5);

 	wait1Msec(200);	// Let go the technician
 	ClearTimer(T1);	//	This timer isn't accurate anymore

 	// resume
 	motor[motorA] = motA;
 	motor[motorB] = motB;
 	motor[motorC] = motC;
 	releaseCPU();
}

// To detect if there's something on our way
task failureTask()
{
 	while(true) {
 		while(true) 
 		{
 		  if (SensorValue[sonarSensor] <= 25)
 		    break;
 		}
		StartTask(maintenance);
	}
}

/*******************************************		Functions for dots detection		*******************************/

/* Here we use the sensor values given by the trackline to save some sensor calls 
 * 'cause the rate is only of 1 color value per 10ms
 */
task colorSensorGreenDotTask()
{
 	ClearTimer(T1);
 	while (time1[T1] < TIME_COL_DOT)
 	{
 		if ((robot.left_col != GREEN) && (robot.right_col != GREEN))
 			ClearTimer(T1);
 		waitInMilliseconds(REF_DOT);
 	}
 	ClearTimer(T1);
 	while(time1[T1] < TIME_COL_DOT)
 		if ((robot.left_col == GREEN) || (robot.right_col == GREEN))
 			ClearTimer(T1);

 	robot.line_stop = true;
}

//	to detect the dots when going to the stock
task colorSensorGreenStockTask()
{
	ClearTimer(T1);
	while (!robot.line_stop)
 	{
 		if ((robot.left_col != GREEN) && (robot.right_col != GREEN))
 			ClearTimer(T1);
 		else if (time1[T1] > TIME_COL_ST)
 			robot.line_stop = true;
 		waitInMilliseconds(REF_DOT);
 	}
}

//	to detect the dots when on the main road, avoid errors
task colorSensorRedDotTask()
{
	ClearTimer(T1);
	while (time1[T1] < TIME_COL_DOT)
	{
		if ((robot.left_col != RED) && (robot.right_col != RED))
			ClearTimer(T1);
		waitInMilliseconds(REF_DOT);
	}
	ClearTimer(T1);
	while(time1[T1] < TIME_COL_DOT)
		if ((robot.left_col == RED) || (robot.right_col == RED))
			ClearTimer(T1);

	robot.line_stop = true;
}

//	to detect the dots when on the main road, avoid errors
task colorSensorRedStockTask()
{
	ClearTimer(T1);
	while (!robot.line_stop)
	{
		if ((robot.left_col != RED) && (robot.right_col != RED))
			ClearTimer(T1);
		else if (time1[T1] > TIME_COL_ST)
			robot.line_stop = true;
		waitInMilliseconds(REF_DOT);
	}
}

//	to detect a dot while rotating, avoid errors
task colorSensorAllDotTask()
{
	ClearTimer(T1);
	while(time1[T1] < TIME_COL_DOT)
	{
		if(((robot.left_col == RED) || (robot.right_col == RED)) ||
		   ((robot.left_col == GREEN) || (robot.right_col == GREEN)))
			ClearTimer(T1);
	}

	ClearTimer(T1);
	while (time1[T1] < TIME_COL_DOT)
	{
		if (((robot.left_col != RED) && (robot.right_col != RED)) &&
		    ((robot.left_col != GREEN) && (robot.right_col != GREEN)))
			ClearTimer(T1);
	}
	robot.line_stop = true;
}

//	to detect a dot when going out of a stock, avoid errors
task colorSensorOutAllDotTask()
{
	ClearTimer(T1);
	while(time1[T1] < TIME_COL_DOT)
	{
		if (((robot.left_col == RED) || (robot.right_col == RED)) ||
		    ((robot.left_col == GREEN) || (robot.right_col == GREEN)))
			ClearTimer(T1);
	}
	robot.line_stop = true;
}

//Zelda music
task ZeldaTask()
{
  PlayImmediateTone(440, 15); wait1Msec(200);
  PlayImmediateTone(466.16, 15);  wait1Msec(200);
  PlayImmediateTone(493.88, 15);  wait1Msec(200);
  PlayImmediateTone(523.25, 60); wait1Msec(650);
}

/*******************************************		Arm functions		*******************************/

// Deploy arm while running
task deployArm()
{
	nMotorEncoder[motorA] = 0;
	motor[motorA] = 40;
 	while (nMotorEncoder[motorA] < 90);
 	motor[motorA] = 0;
}

//	Fold away the arm while running
task takeBackArm()
{
	nMotorEncoder[motorA] = 0;
	motor[motorA] = -40;
	while (nMotorEncoder[motorA] > -90);
	motor[motorA] = 0;
}

// Task to take a ball without ejecting the others
void ballPrehension()
{
  int current_encoder = 0;
  int last_encoder = 0;
  nMotorEncoder[motorA] = 0;
  // We move slowly at first
  motor[motorA] = -15;
  ClearTimer(T1);

  do {
    current_encoder = nMotorEncoder[motorA];

    if (current_encoder < -50)	// here we speed up the prehension to decrease our chances of failure
      motor[motorA] = -40;

  	//	Means that our arm is blocked
    if (abs(current_encoder - last_encoder) > 5) 
    {
      ClearTimer(T1);
      last_encoder = current_encoder;
    }

    if (time1[T1] > 500) // Seems like we're failing to take the, we'll reajust our position
    {
      motor[motorA] = 30;
      while (nMotorEncoder[motorA] < 0);
      motor[motorA] = 0;
      adjustPrehension();
      return;
    }
  }while (current_encoder > -65);

  // We've taken the ball, now we slow down the movement
  motor[motorA] = -20;
  while (nMotorEncoder[motorA] > -90);
  // Ball taken
  motor[motorA] = 0;
}

// Task to put the ball in the stock
void ballDeprehension()
{
	nMotorEncoder[motorA] = 0;
	motor[motorA] = 80;
	while (nMotorEncoder[motorA] < 70);
	motor[motorA] = 0;
}

// Task to determine the ball color, avoid errors
int colorSensorBall()
{
	int i;
	int br = SensorValue[light_sensor];
	for(i=0; i<19; i++)
		br += SensorValue[light_sensor];
	br /= 20;

	if (abs(br - robot.blue) < robot.blue_threshold) 
	{
	  	StartTask(ZeldaTask);//	'cause Zelda is cool
		return BLUE;
	}

	if (abs(br - robot.red) < robot.red_threshold) 
	{
		StartTask(ZeldaTask);
		return RED;
	}

	return NO_COLOR;
}
