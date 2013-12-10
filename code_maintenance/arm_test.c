// test arm
task main()
{
  nMotorEncoder[motorA] = 0;
 	motor[motorA] = 20;
 	while (nMotorEncoder[motorA] < 90){}
 	motor[motorA] = 0;
 	wait(2);
 	motor[motorA] = -20;
 	while (nMotorEncoder[motorA] > 0){}
 	motor[motorA] = 0;

	wait1Msec(5000);
}
