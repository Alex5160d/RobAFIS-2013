/* ************************************************************************** *
* Complex tasks: built of primary tasks, are our robot main functionalities
* ************************************************************************** */

void trackLine(int speed, int low_speed, TaskType t);

// Read the values of blue and red ball's brightness and their threshold
void _readValues()
{
	TFileHandle file;
	TFileIOResult result;
	string fname = "calibration.txt";
	int fsize = 100;

	OpenRead(file, result, fname, fsize);
	char b;

	ReadShort(file, result, robot.red);
	ReadByte(file, result, b);
	ReadShort(file, result, robot.blue);
	ReadByte(file, result, b);
	ReadShort(file, result, robot.red_threshold);
	ReadByte(file, result, b);
	ReadShort(file, result, robot.blue_threshold);
	ReadByte(file, result, b);
	ReadShort(file, result, robot.line_lost_time);
}

// Let the technician choose the p parameter
void _chooseP()
{
	short button;
	nxtDisplayCenteredTextLine(1, "Parametre p");
	nxtDisplayCenteredTextLine(3, "<= Bleu	Rouge =>");
	nxtDisplayCenteredTextLine(4, "0              1");

	do
	{
		button = nNxtButtonPressed;

		if (button == (short) kRightButton)
			robot.p = RED;
		else if (button == (short) kLeftButton)
			robot.p = BLUE;
	} while((button != (short) kRightButton) && (button != (short) kLeftButton));
	eraseDisplay();

	nxtDisplayTextLine(2, "Parametre p : %d", robot.p == BLUE ? 0 : 1);
	nxtDisplayTextLine(3, "Appuyer sur le");
	nxtDisplayTextLine(4, "bouton Orange");

	wait1Msec(250);
	while (nNxtButtonPressed != (short) kEnterButton);
	wait1Msec(450);
	eraseDisplay();
}

// Build the main environment
void buildEnvironment()
{
	_readValues();
	robot.b_color = NO_COLOR;
	_chooseP();

	robot.line_stop = false;
	robot.se = true;
	robot.empty = false;
	robot.bse = 3;

	robot.left_col = NO_COLOR;
	robot.right_col = NO_COLOR;
}

void rotate(Direction direction, int angle = ANGLE_90)
{
	tSensors scd_sensor;
	tMotor fwd_motor, bwd_motor;

	nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
	nSyncedTurnRatio = -100; //motors move at 100% alignment to each other
	nPidUpdateInterval = 10;

	nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero

	// This is the main rotation
	switch (direction)
	{
		case LEFT:
			scd_sensor = right_sensor;
			fwd_motor = motorC;
			bwd_motor = motorB;
			nMotorEncoderTarget[motorB] = -angle;
			motor[motorB] = -ROT_SPEED;
			break;
		default:
			scd_sensor = left_sensor;
			fwd_motor = motorB;
			bwd_motor = motorC;
			nMotorEncoderTarget[motorB] = angle;
			motor[motorB] = ROT_SPEED;
			break;
	}
	while(nMotorRunState[motorB] != runStateIdle);	//while motorB is not in an idle position

	motor[motorB] = 0;
	nSyncedMotors = synchNone;

	// In case we went on the dot during the rotation
	robot.line_stop = false;
	robot.left_col = SensorValue[left_sensor];
	robot.right_col = SensorValue[right_sensor];
	StartTask(colorSensorOutAllDotTask);
	while(!robot.line_stop)
	{
		robot.left_col = SensorValue[left_sensor];
		robot.right_col = SensorValue[right_sensor];

		motor[motorB] = SPEED;
		motor[motorC] = SPEED;
	}

	// Smoothly finish the rotation
	while(SensorValue[scd_sensor] != BLACK)
	{
		motor[fwd_motor] = GO_BLACK;
		motor[bwd_motor] = -GO_BLACK;
	}

	// Adjust the robot position before it speeds up
	trackLine(STOCK_SPEED, LOW_STOCK_SPEED, CHRONO_TASK);
}

// Follow the black line using two sensors
void trackLine(int speed, int low_speed, TaskType t)
{
	robot.line_stop = false;
	robot.left_col = SensorValue[left_sensor];
	robot.right_col = SensorValue[right_sensor];

	// Start the background task that will end the line tracking
	switch (t)
	{
		case GREEN_DOT_TASK:
			StartTask(colorSensorGreenDotTask);
			break;
		case RED_DOT_TASK:
			StartTask(colorSensorRedDotTask);
			break;
		case GREEN_STOCK_TASK:
			StartTask(colorSensorGreenStockTask);
			break;
		case RED_STOCK_TASK:
			StartTask(colorSensorRedStockTask);
			break;
		case START_TASK:
			StartTask(sensorsStartTask);
			break;
		case CHRONO_TASK:
			StartTask(chronoTask);
			break;
	}
	while (!robot.line_stop)
	{
		robot.left_col = SensorValue[left_sensor];
		robot.right_col = SensorValue[right_sensor];

		if (robot.left_col == BLACK)
		{
			motor[motorB] = low_speed;
			motor[motorC] = speed;
		}
		else if ((robot.right_col == BLACK) && (robot.left_col == WHITE))
		{
			motor[motorB] = speed;
			motor[motorC] = low_speed;
		}
		else
		{
			motor[motorB] = speed;
			motor[motorC] = speed;
		}
	}
}

// Backward trackline to go out of stocks
void trackLineBack(TaskType t)
{
	robot.left_col = SensorValue[left_sensor];
	robot.right_col = SensorValue[right_sensor];
	robot.line_stop = false;

	// The trackline ends when we find a dot
	StartTask(colorSensorAllDotTask);

	while (!robot.line_stop)
	{
		robot.left_col = SensorValue[left_sensor];
		robot.right_col = SensorValue[right_sensor];

		if (robot.left_col == BLACK)
		{
			motor[motorB] = -STOCK_SPEED;
			motor[motorC] = -LOW_STOCK_SPEED;
		}
		else if ((robot.right_col == BLACK) && (robot.left_col == WHITE))
		{
			motor[motorB] = -LOW_STOCK_SPEED;
			motor[motorC] = -STOCK_SPEED;
		}
		else
		{
			motor[motorB] = -STOCK_SPEED;
			motor[motorC] = -STOCK_SPEED;
		}
	}

	stop();
	// Not to be too brutal
	wait1Msec(150);
	// We quickly move forward to adjust the robot's position before it rotates
	trackLine(SPEED, LOW_SPEED, t);
}

// if our position is wrong to take the ball we will adjust it
void adjustPrehension()
{
	// Adjust the robot position in front of the stock
	robot.left_col = SensorValue[left_sensor];
	robot.right_col = SensorValue[right_sensor];

	while(robot.left_col != GREEN || robot.right_col != GREEN)
	{
		if((robot.left_col != GREEN) && (robot.right_col != GREEN))
		{
			motor[motorB] = ADJUST_SPEED;
			motor[motorC] = ADJUST_SPEED;
		}
		else if(robot.left_col != GREEN)
		{
			motor[motorB] = ADJUST_SPEED;
			motor[motorC] = -ADJUST_SPEED;
		}
		else
		{
			motor[motorB] = -ADJUST_SPEED;
			motor[motorC] = ADJUST_SPEED;
		}

		robot.left_col = SensorValue[left_sensor];
		robot.right_col = SensorValue[right_sensor];
	}

	stop();
	ballPrehension();
}
