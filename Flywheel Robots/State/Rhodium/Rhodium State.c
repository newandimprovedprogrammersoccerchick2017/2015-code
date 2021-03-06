#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl12, led,            sensorLEDtoVCC)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           intakeLeft,    tmotorVex393HighSpeed_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           rFlyTop,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           rFlyBottom,    tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port4,           rDriveFront,   tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_5)
#pragma config(Motor,  port5,           rDriveBack,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           lDriveBack,    tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           lDriveFront,   tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port8,           lFlyTop,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           lFlyBottom,    tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port10,          intakeRight,   tmotorVex393HighSpeed_HBridge, openLoop, encoderPort, I2C_3)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "..\..\..\Vex_Competition_Includes_No_LCD.c"   //Main competition background code...do not modify!
#include "..\..\..\LCD Autonomous Play Selection.c"
#include "..\Global\Simple PID Controller.h"

task flashLED() {
	while(1) {
		SensorValue[led] = true;
		wait1Msec(450);
		SensorValue[led] = false;
		wait1Msec(450);
	}
}
fw_controller lFly, rFly;
string str;
int flywheelMode = 0; //0 - stopped, 1 - close, 2 - center, 3 - purple, 4 - long
#define FORWARD 1;
#define BACKWARD -1;

void setLDriveMotors (float power) {
	motor[lDriveFront] = power;
	motor[lDriveBack] = power;
}

void setRDriveMotors (float power) {
	motor[rDriveFront] = power;
	motor[rDriveBack] = power;
}


void setIntakeMotors (float power) {
	motor[intakeLeft] = power;
	motor[intakeRight] = power;
}

void driveDistance (int encoderCounts, int direction, float power) {
	int	encoderGoalLeft = nMotorEncoder[lDriveFront] + encoderCounts*direction;
	int encoderGoalRight = nMotorEncoder[rDriveFront] + encoderCounts*direction;
	int lAdjust = 0;
	int rAdjust;
	while (nMotorEncoder[rDriveFront] < encoderGoalRight) {
		if (abs(nMotorEncoder[rDriveFront] - nMotorEncoder[rDriveBack]) <= 10) { //sides are close together
			//lAdjust = 0;
		} else if (nMotorEncoder[lDriveFront] > nMotorEncoder[rDriveFront]) { //left side is ahead
			//lAdjust = -10; //slow left side down
		} else { //right side is ahead
			//lAdjust = 10; //speed left side up to compensate
		}
		setLDriveMotors((power+lAdjust)*direction);
		setRDriveMotors(power*direction);
		wait1Msec(10);
	}

	setLDriveMotors(0);
	setRDriveMotors(0);
}


//rotate the robot to a certain position (rotationally)
//@param deg The number of degrees to turn; positive values are counterclockwise, negative values are clockwise.
//@param direction The direction to turn in to get to the position; 1 is counterclockwise, -1 is clockwise
void rotate (int deg, int direction) {
	//Specify the number of degrees for the robot to turn (1 degree = 10, or 900 = 90 degrees)
  int degrees10 = deg*10; //multiply the degrees parameter by 10 to get the amount to turn relative to gyro sensor values

  //Specify the amount of acceptable error in the turn
  int error = 5;

  //While the absolute value of the gyro is less than the desired rotation - 100...
  while(abs(SensorValue[gyro]) < degrees10 - 100)
  {
    setRDriveMotors(50*direction);
    setLDriveMotors(-50*direction);
  }
  //Brief brake to eliminate some drift
  setRDriveMotors(-5*direction);
  setLDriveMotors(5*direction);
  wait1Msec(100);

  //Second while loop to move the robot more slowly to its goal, also setting up a range
  //for the amount of acceptable error in the system
  while(abs(SensorValue[gyro]) > degrees10 + error || abs(SensorValue[gyro]) < degrees10 - error)
  {
    if(abs(SensorValue[gyro]) > degrees10)
    {
      setRDriveMotors(-50);
      setLDriveMotors(50);
    }
    else
    {
      setRDriveMotors(50);
      setLDriveMotors(-50);
    }
  }

  //Stop
	setRDriveMotors(0);
	setLDriveMotors(0);
}


void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;
	displayLCDCenteredString(0,"Startup");
	SensorType[gyro] = sensorNone;
  wait1Msec(500);
  //Reconfigure Analog Port 8 as a Gyro sensor and allow time for ROBOTC to calibrate it
  SensorType[gyro] = sensorGyro;
  wait1Msec(2000);
  startTask(selectionController);

}

void setLeftFwSpeed (float power) {
	motor[lFlyTop] = power;
	motor[lFlyBottom] = power;
}

void setRightFwSpeed (float power) {
	motor[rFlyTop] = power;
	motor[rFlyBottom] = power;
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the left flywheel               */
/*-----------------------------------------------------------------------------*/
task leftFwControlTask()
{
	fw_controller *fw = lFly;

	// We are using Speed geared motors
	// Set the encoder ticks per revolution
	fw->ticks_per_rev = fw->MOTOR_TPR;

	while(1)
	{
		// debug counter
		fw->counter++;

		// Calculate velocity
		getEncoderAndTimeStamp(lFlyBottom,fw->e_current, fw->encoder_timestamp);
		FwCalculateSpeed(fw);

		// Set current speed for the tbh calculation code

		fw->v_current = getMotorVelocity(lFlyBottom);
		fw->current = fw->v_current;

		// Do the velocity TBH calculations
		FwControlUpdateVelocityTbh( fw ) ;

		// Scale drive into the range the motors need
		fw->motor_drive  = fw->drive * (FW_MAX_POWER/127);

		// Final Limit of motor values - don't really need this
		if( fw->motor_drive >  127 ) fw->motor_drive =  127;
		if( fw->motor_drive < -127 ) fw->motor_drive = -127;

		// and finally set the motor control value
		//if(fw->current < fw->target - 20) {
		//	setLeftFwSpeed( 70 );
		//} else
			setLeftFwSpeed(fw->motor_drive);
		str = sprintf( str, "%4d %4d  %5.2f", fw->target,  fw->current, nImmediateBatteryLevel/1000.0 );
		displayLCDString(0, 0, str );
		str = sprintf( str, "%4.2f %4.2f ", fw->drive, fw->drive_at_zero );
		displayLCDString(1, 0, str );
		// Run at somewhere between 20 and 50mS
		wait1Msec( FW_LOOP_SPEED );
	}
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the right flywheel              */
/*-----------------------------------------------------------------------------*/
task rightFwControlTask()
{
	fw_controller *fw = rFly;

	// We are using Speed geared motors
	// Set the encoder ticks per revolution
	fw->ticks_per_rev = fw->MOTOR_TPR;

	while(1)
	{
		// debug counter
		fw->counter++;

		// Calculate velocity
		getEncoderAndTimeStamp(rFlyBottom,fw->e_current, fw->encoder_timestamp);
		FwCalculateSpeed(fw);

		// Set current speed for the tbh calculation code
		fw->current = fw->v_current;

		// Do the velocity TBH calculations
		FwControlUpdateVelocityTbh( fw ) ;

		// Scale drive into the range the motors need
		fw->motor_drive  = fw->drive * (FW_MAX_POWER/127);

		// Final Limit of motor values - don't really need this
		if( fw->motor_drive >  127 ) fw->motor_drive =  127;
		if( fw->motor_drive < -127 ) fw->motor_drive = -127;

		// and finally set the motor control value
		//if(fw->current < fw->target - 20) {
		//	setRightFwSpeed( 70 );
		//} else
			setRightFwSpeed( fw->motor_drive );

		// Run at somewhere between 20 and 50mS
		wait1Msec( FW_LOOP_SPEED );
	}
}

//long shooting
void initializePIDLong() {
	//tbhInit(lFly, 392, 0.6, 0.008064, 0, 70); //initialize PID for left side of the flywheel
	//tbhInit(rFly, 392, 0.55, 0.008064, 0, 70); //initialize PID for right side of the flywheel
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.3081, 1.3181, .005481, 0, 75, 20); // .2481 .6681 initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.3081, 1.3181, .005481, 0, 75, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//short shooting
void initializePIDShort() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.7481, 1.0481, 0.005481, 0, 42, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.7481, 1.0481, 0.005481, 0, 42, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//purple shooting (for skills)
void initializePIDPurple() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.6281, 1.03, 0.005481, 0, 55, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.6281, 1.03, 0.005481, 0, 55, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//stop flywheel
void stopFlywheel() {
	//disable PIC control of the flywheels and switch to open-loop control
	stopTask(leftFwControlTask);
	stopTask(rightFwControlTask);

	//turn off the flywheel motors
	setLeftFwSpeed(0);
	setRightFwSpeed(0);

	flywheelMode = 0; //make sure we know that the flywheel is stopped
}

void intakeDistance (int encoderCounts, int direction, float power) {
	int encoderGoal = nMotorEncoder[intakeRight] - encoderCounts*direction; //intake encoder counts down for forward
	if (direction == 1) {
		while (nMotorEncoder[intakeRight] > encoderGoal) {
			setIntakeMotors(power*direction);
		}
	} else {
		while (nMotorEncoder[intakeRight] < encoderGoal) {
			setIntakeMotors(power*direction);
		}
	}

	setIntakeMotors(0);
}

void centerShotAuton(bool waitToStart) {
	if (waitToStart) {
		wait1Msec(3000);
	}
	initializePIDPurple();
	FwVelocitySet(lFly, 128, .5);
	FwVelocitySet(rFly, 128, .5);
	driveDistance(1600, 1, 85);
	wait1Msec(750);
	intakeDistance(150,1,125);
	wait1Msec(1250);
	intakeDistance(205,1,125);
	wait1Msec(1250);
	intakeDistance(300,1,125);
	wait1Msec(1250);
	intakeDistance(300,1,125);
	wait1Msec(1250);
	stopFlywheel(); //turn off the flywheel
}

void closeShotAuton(bool waitToStart) {
	if (waitToStart) {
		wait1Msec(3000);
	}
	initializePIDShort();
	FwVelocitySet(lFly, 102, .5);
	FwVelocitySet(rFly, 102, .5);
	driveDistance(3200, 1, 85);
	wait1Msec(500);
	rotate(0,-1);
	wait1Msec(250);
	setIntakeMotors(105); //turn on the intake to outtake the balls
	wait1Msec(2500); //wait long enough to shoot all the balls
	setIntakeMotors(0); //stop the intake
	stopFlywheel(); //turn off the flywheel
}

task autonomous()
{
	if (pgmToRun == "R Side Long" || pgmToRun == "R Back Long"
			|| pgmToRun == "B Side Long"
			|| pgmToRun == "B Back Long") {
			centerShotAuton(delayStart);
	} else if (pgmToRun == "B Side Close" || pgmToRun == "B Back Close"
			|| pgmToRun == "R Side Close"
			|| pgmToRun == "R Back Close") {
			closeShotAuton(delayStart);
	} else if (pgmToRun == "Prog. skills") {
			//programmingSkills();
	}

	/*initializePIDLong();
	setLeftFwSpeed(70);
	setRightFwSpeed(70);
	wait1Msec(500);
	FwVelocitySet(lFly,136,.7);
	FwVelocitySet(rFly,136,.7);
	wait1Msec(2000);
	motor[intakeLeft] = 125;
	motor[intakeRight] = 125;
	wait1Msec(6000);
	motor[intakeLeft] = 0;
	motor[intakeRight] = 0;
	wait1Msec(1000);
	stopFlywheel();*/

}

bool userIntakeControl = true;
task closeShootingMacro() {
	while (1) {
		if (vexRT[Btn8D] == 1 && flywheelMode == 1) { //only run this if the flywheel is in the correct operating state (close shooting only), to prevent mishaps resulting from accidental button presses
			userIntakeControl = false; //prevent user from controlling intake while macro is running
			setIntakeMotors(105); //turn on the intake to outtake the balls
			wait1Msec(2000); //wait long enough to shoot all the balls
			setIntakeMotors(0); //stop the intake
			userIntakeControl = true; //return intake control to user
			stopFlywheel(); //turn off the flywheel
		}
		wait1Msec(25); //don't hog the CPU
	}
}

int lSpeed = 60;
int rSpeed = 60;
task usercontrol()
{
	startTask(closeShootingMacro);
	//startTask(autonomous);
	//startTask(autonomous); //comment out and change while loop condition
	//writeDebugStreamLine("nPgmTime,lFly.current, lFly.motor_drive, lFly.p, lFly.i, lFly.d, lFly.constant, 50*lFly.postBallLaunch, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d, rFly.constant, 60*rFly.postBallLaunch");
	//setLeftFwSpeed(lSpeed);
	//setRightFwSpeed(rSpeed);
	//wait1Msec(500);

	//short shooting
	//intake power 115
	//initializePIDShort();
	//FwVelocitySet(lFly, 93.75, .5);
	//FwVelocitySet(rFly, 93.75, .5);
	//wait1Msec(2000);
	//userIntakeControl = false;
	//setIntakeMotors(105);
	//wait1Msec(2000); //testing
	//long shooting
	//initializePIDLong();
	//FwVelocitySet(lFly,144,.7);
	//FwVelocitySet(rFly,144,.7);
	//purple shooting
	//intake power 125
	//initializePIDPurple();
	//FwVelocitySet(lFly,115,.7);
	//FwVelocitySet(rFly,115,.7);
  startTask(flashLED);
	int intakePower,
	threshold = 15,
	lY,
	rY;
	while (true)
	{
		//drivetrain
		lY = vexRT[Ch3];
		rY = vexRT[Ch2];
		motor[lDriveFront] = (abs(lY) > threshold) ? lY : 0;
		motor[lDriveBack] = (abs(lY) > threshold) ? lY : 0;
		motor[rDriveFront] = (abs(rY) > threshold) ? rY : 0;
		motor[rDriveBack] = (abs(rY) > threshold) ? rY : 0;

		//intake
		if (userIntakeControl) { //if the program is not overriding control of the intake
			intakePower = 125*vexRT[Btn6U] - 125*vexRT[Btn6D];
			setIntakeMotors(intakePower);
		}
		if(vexRT[Btn5D] == 1)
		{
			setIntakeMotors(-127);
			wait10Msec(10);
			setIntakeMotors(0);
			wait10Msec(20);
		}
		//flywheel speed control
		//7U - long, 7R - purple, 7D - short
		//8R - stop, 8D - if short shooting (flywheel must be on and in short shooting mode), outtake 4 balls and then stop the flywheel automatically
		if (vexRT[Btn7U] == 1 && flywheelMode != 4) { //second condition prevents reinitialization of long shooting if the flywheel is currently in long shooting mode
			if (flywheelMode >= 1) { //if the flywheel is currently running (modes 1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
			}

			//next 4 lines have to run every time to run flywheel
			flywheelMode = 4; //make sure we set the flywheel mode
			initializePIDLong(); //prepare controller for long shooting
			//set long shooting velocities
		  FwVelocitySet(lFly,134,.7);
	    FwVelocitySet(rFly,134,.7);
		} else if (vexRT[Btn7R] == 1 && flywheelMode != 3) { //purple shooting
			if (flywheelMode >= 1) { //if the flywheel is currently running (modes 1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
			}

			//next 4 lines have to run every time to run flywheel

			flywheelMode = 3;
			initializePIDPurple();
			FwVelocitySet(lFly,120,.7);
			FwVelocitySet(rFly,120,.7);

		} else if (vexRT[Btn7D] == 1 && flywheelMode != 1) { //close shooting
			if (flywheelMode >= 1) { //if the flywheel is currently running (modes 1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
			}

			//next 4 lines have to run every time to run flywheel
			flywheelMode = 1;
			initializePIDShort();
			FwVelocitySet(lFly, 102, .5);
			FwVelocitySet(rFly, 102, .5);

		} else if (vexRT[Btn8R] == 1 && flywheelMode >= 1) { //this is an else statement so that if two buttons are pressed, we won't switch back and forth between starting and stopping the flywheel
			stopFlywheel();																		 // flywheelMode needs to be greater than 1 so that we don't run the stopFlywheel function if the flywheel is already stopped
		}

		//writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d,%d",rFly.encoder_timestamp, rFly.e_current, rFly.error, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d);
	  writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",nPgmTime,lFly.current, lFly.motor_drive, lFly.p, lFly.i, lFly.d, lFly.constant, 50*lFly.postBallLaunch, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d, rFly.constant, 60*rFly.postBallLaunch);
		wait1Msec(25);
	}
}
