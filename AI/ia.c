/* ************************************************************************** *
* Robot IA: define our robot comportment using complex and primary tasks
* ************************************************************************** */

// Called each time we need to check for a ball
void findBall()
{
	// is there a ball ?
	robot.b_color = colorSensorBall();

	while(robot.b_color == NO_COLOR)	// while there is no ball
	{
		if(robot.se)	// It means that we have no more balls to take in SE
		{
			robot.empty = true;	// So we'll never come to SE again
			goSPEfromSEandViceVersa(); // go take a look at SE, it's the last hiding place for balls
			robot.se = false;
		}
		else if(robot.empty) // The game is over, SE is already empty and we are in SPE which seems to be empty too
		{
			goStart(LEFT); // So we go home
		}
		else // Ok, the opponent is too slow, there is no ball in SPE but SE isn't already empty. Go take a look in SE
		{
			ClearTimer(T4);	//	We have already checked that SPE was empty so we reset the timer (for the 2min penalty)
			goSPEfromSEandViceVersa();	// Go to SE
			robot.se = true;
		}
		//	This line means that we moved to the other entry stock and we are checking again for a ball
		robot.b_color = colorSensorBall();
	}
}

//	To go back to the main line
void goOutStock(int rotation, TaskType t, int angle = ANGLE_90)
{
	trackLineBack(t);
	//not to be too brutal in our movement
	stop();
	wait1Msec(150);

	rotate(rotation, angle);
	//we are now on the main line, ready to run
}

//	Go from SE or SPE to the start area
void goStart(int rotation)
{
	// the direction depends on wether I come from SE or SPE
	goOutStock(rotation, GREEN_DOT_TASK);

	// Run until we've found the rest area with the sonar
	trackLine(SPEED, LOW_SPEED, START_TASK);
	// We have to be at rest in the start area
	StartTask(takeBackArm);
	motor[motorB] = SPEED;
	motor[motorC] = SPEED;
	stop();

	playMario();
	StopAllTasks(); //	exit the program
}

// The function called when the current stock is empty
void goSPEfromSEandViceVersa()
{
	//	we turn around
	goOutStock(LEFT, GREEN_DOT_TASK, ANGLE_180);
	//	open the arm to try again
	ballDeprehension();
	trackLine(STOCK_SPEED, LOW_STOCK_SPEED, GREEN_STOCK_TASK);
	stop();
	//try again
	ballPrehension();
}

// The first movement of the robot, we'll check SE
void goSEfromStart()
{
	StartTask(failureTask);	// In case of obstacle
	// go to the first dot
	trackLine(SPEED, LOW_SPEED, GREEN_DOT_TASK);
	StopTask(failureTask);	// The stock is to close for the sonar
	rotate(RIGHT);
	// go to SE
	trackLine(STOCK_SPEED, LOW_STOCK_SPEED, GREEN_STOCK_TASK);
	stop();

	// is there a ball ?
	ballPrehension();
	findBall();

	// The program didn't quit, there is still a ball
	if(robot.se)	// if the stock is SE
	{
		robot.bse--;	// one less ball to take (on three)
		goOutStock(LEFT, GREEN_DOT_TASK);
	}
	else	// The stock is SPE
	{
		goOutStock(RIGHT, GREEN_DOT_TASK);
	}
}

// The function to bring the balls to SA and SPD
void goSAs()
{
	// go to the first red dot
	StartTask(failureTask);
	trackLine(SPEED, LOW_SPEED, RED_DOT_TASK);
	StopTask(failureTask);	// The stock is to close for the sonar

	if(robot.b_color == robot.p)	// This ball belongs to us !!!!
		trackLine(SPEED, LOW_SPEED, RED_DOT_TASK);	// go SA
	else	// not mine :(
		rotate(LEFT);	// go SPD

	trackLine(STOCK_SPEED, LOW_STOCK_SPEED, RED_STOCK_TASK);
	stop();
	ballDeprehension();

	/*
	* The job is done, now we return on the main line
	* rotation direction is important here
	*/
	if(robot.b_color == robot.p)	// SA
	{
		goOutStock(RIGHT,RED_DOT_TASK, 500);
		StartTask(failureTask);
		trackLine(SPEED, LOW_SPEED, RED_DOT_TASK);
	}
	else	// SPD
	{
		goOutStock(LEFT, RED_DOT_TASK);
		StartTask(failureTask);
	}
}

// The function to go back from SAs to SE and SPE
void goSEs()
{
	// go to the green dot
	trackLine(SPEED, LOW_SPEED, GREEN_DOT_TASK);
	StopTask(failureTask);

	if(time100[T4] > 600 || robot.empty)	// We know SE is empty or it's been more than a minute since the last time we came to SPE
	{
		// go SPE
		robot.se = false;
		rotate(RIGHT);
	}
	else	//go to SPE
	{
		robot.se = true;
		rotate(LEFT);	// go SE
	}
	trackLine(STOCK_SPEED, LOW_STOCK_SPEED, GREEN_STOCK_TASK);
	stop();

	ballPrehension();
	findBall();
	// The program didn't quit, there is still a ball
	if(robot.se)	// if the stock is SE
	{
		robot.bse--;	// one less ball to take (on three)
		if(robot.bse == 0)	// We've just taken the last ball in SE
			robot.empty = true;
		goOutStock(LEFT, GREEN_DOT_TASK);
	}
	else	// The stock is SPE
	{
		goOutStock(RIGHT, GREEN_DOT_TASK);
	}
}
