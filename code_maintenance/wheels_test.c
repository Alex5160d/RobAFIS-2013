task main()
{

  nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
  nPidUpdateInterval    = 5;

  nSyncedTurnRatio = 100; //motors move at 100% alignment to each other
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  nMotorEncoderTarget[motorB] = 1000; //set motorB's target position to 720
  motor[motorB] = 30; //power motorB at 30% power
  while(nMotorRunState[motorB] != runStateIdle)//while motorB is not in an idle position
  {
   //continue to monitor nMotorEncoderTarget's target position
  }
  motor[motorB] = 0;  // turn the motors off.

  wait(1);

  nSyncedTurnRatio = -100; //motors move at 100% alignment to each other
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  nMotorEncoderTarget[motorB] = 500; //set motorB's target position to 720
  motor[motorB] = 30; //power motorB at 30% power
  while(nMotorRunState[motorB] != runStateIdle)//while motorB is not in an idle position
  {
   //continue to monitor nMotorEncoderTarget's target position
  }
  motor[motorB] = 0;  // turn the motors off.

  wait(1);

  nSyncedTurnRatio = 100; //motors move at 100% alignment to each other
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  nMotorEncoderTarget[motorB] = 1000; //set motorB's target position to 720
  motor[motorB] = 30; //power motorB at 30% power
  while(nMotorRunState[motorB] != runStateIdle)//while motorB is not in an idle position
  {
   //continue to monitor nMotorEncoderTarget's target position
  }
  motor[motorB] = 0;  // turn the motors off.

  wait1Msec(3000);

}
