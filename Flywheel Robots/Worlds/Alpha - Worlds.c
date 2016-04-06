#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl10, intakeLimit,    sensorTouch)
#pragma config(Sensor, dgtl11, yellowLED,      sensorLEDtoVCC)
#pragma config(Sensor, dgtl12, redLED,         sensorLEDtoVCC)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           intakeRoller,  tmotorVex393TurboSpeed_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           lFlywheel,     tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port3,           rDriveFront,   tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           rDriveMiddle,  tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port5,           rDriveBack,    tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           lDriveBack,    tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rFlyTop,       tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           rFlyBottom,    tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port9,           lDriveFrontMid, tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, I2C_5)
#pragma config(Motor,  port10,          intakeChain,   tmotorVex393TurboSpeed_HBridge, openLoop, reversed, encoderPort, I2C_3)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "..\..\Vex_Competition_Includes_No_LCD.c"   //Main competition background code...do not modify!
#include "..\..\LCD Autonomous Play Selection.c"
#include "..\State\Global\Simple PID Controller.h"

fw_controller lFly, rFly;
string str;
float flywheelMode = 0; //0 - stopped, 0.5 - stopping, 1 - close, 2 - center, 3 - purple, 4 - long
int ballsInIntake = 0;
#define FORWARD 1;
#define BACKWARD -1;

void setLDriveMotors (float power) {
	motor[lDriveFrontMid] = power;
	motor[lDriveBack] = power;
}

void setRDriveMotors (float power) {
	motor[rDriveFront] = power;
	motor[rDriveMiddle] = power;
	motor[rDriveBack] = power;
}

void setIntakeMotors (float power) {
	motor[intakeChain] = power;
	motor[intakeRoller] = power;
}

int yellowLEDFlashTime = 0; //the time the flashing yellow LED should stay on or off, in milliseconds.  This should be equivalent to half a period.
task flashLED() {
	while(1) {
		if (yellowLEDFlashTime == 0) {
			SensorValue[yellowLED] = false;
		} else {
			SensorValue[yellowLED] = true;
			wait1Msec(yellowLEDFlashTime);
			SensorValue[yellowLED] = false;
			wait1Msec(yellowLEDFlashTime);
		}
	}
}

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

	startTask(selectionController); //run LCD selection
}

void setLeftFwSpeed (float power) {
	motor[lFlywheel] = power;
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
		getEncoderAndTimeStamp(lFlywheel,fw->e_current, fw->encoder_timestamp);
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
		setLeftFwSpeed(fw->motor_drive);

		//output debug info on the LCD
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
		setRightFwSpeed( fw->motor_drive );

		// Run at somewhere between 20 and 50mS
		wait1Msec( FW_LOOP_SPEED );
	}
}

//long shooting
void initializePIDLong() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.5821, 3, 0.006481, 0, 70, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.5821, 3, 0.006481, 0, 70, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//short shooting
void initializePIDShort() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.1281, 3, 0.005081, 0, 50, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.1281, 3, 0.005081, 0, 50, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//purple shooting (for skills)
void initializePIDPurple() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.4281, 3.03, 0.005481, 0, 55, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.4281, 3.03, 0.005481, 0, 55, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

task stopFlywheel() {
	//in order to continuing monitoring RPM after the flywheel is stopped (to prevent balls from being shot when the flywheel is not spinning fast enough to shoot them out of the robot),
	//  the flywheel is stopped using a special instace of the PIC controller, which will monitor flywheel RPM until it reaches 5 and then shutdown the flywheel motors completely)
  //  This is structured such that starting the flywheel will immediately override anything this task does.
	while(1) {
		if(flywheelMode == 0.5) { //trigger this by changing the value of flywheelMode to 0.5 rather than using a function call
			//stop the flywheel tasks so we can restart them with our new controllers
			stopTask(leftFwControlTask);
			stopTask(rightFwControlTask);

			//create the new controllers.  Both P constants are high so that the motor value ends up being 0 (KpBallShot is the same as KpNorm so that the P constant has a constant value regardless of whether the controller thinks a ball has been shot [or if a ball has actually been shot])
			tbhInit(lFly, 392, 1, 1, 0, 0, 0, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
			tbhInit(rFly, 392, 1, 1, 0, 0, 0, 20); //initialize PID for right side of the flywheel //x.x481

			//restart the flywheel tasks with these new controllers
			startTask(leftFwControlTask);
			startTask(rightFwControlTask);
			FwVelocitySet(lFly, 0, 0);
	    FwVelocitySet(rFly, 0, 0);

			//wait for the flywheels to have a velocity <= 5 RPM (for this only one side needs to meet this condition since the sides are mechanically linked)
			while ((lFly.current > 5 || rFly.current > 5) && flywheelMode == 0.5) {
				//wait to continue
				wait1Msec(25);
			}

			//the above while loop can be exited for one of two reasons:
			//  1. flywheel velocity on one side drops below 5 RPM
			//  2. flywheel mode changes (i.e., the user selects a new flywheel mode [close, purple, or long]
			//In case 1, we can stop the flywheel completely.  In case 2, we need to stop the flywheel stop process and start the flywheel back up (starting the flywheel is
			//  handled in the usercontrol task).
			if (flywheelMode == 0.5) { //only shutdown the flywheel if the user hasn't restarted the flywheel
					//return to open-loop control so we can control the flywheel motor powers
					stopTask(leftFwControlTask);
					stopTask(rightFwControlTask);

					//turn off the flywheel motors
					setLeftFwSpeed(0);
					setRightFwSpeed(0);

					flywheelMode = 0; //make sure we know that the flywheel is fully stopped
			}
		}
		wait1Msec(25); //don't overload the cpu
	}
}

//stop flywheel (note: this function should only be used for autonomous code.  The stopFlywheel *task* handles flywheel stops during driver control
void stopFlywheelAuton() {
	//disable PIC control of the flywheels and switch to open-loop control
	stopTask(leftFwControlTask);
	stopTask(rightFwControlTask);
	//turn off the flywheel motors
	setLeftFwSpeed(0);
	setRightFwSpeed(0);

	flywheelMode = 0; //make sure we know that the flywheel is stopped
}

task drivetrainController() {
	int lYRequested,
			rYRequested,
			lYLastSent = 0,
			rYLastSent = 0,
			lY,
			rY,
			slewRateLimit = 15,
			threshold = 15;
	while(true) {
		lYRequested = vexRT[Ch3];
		rYRequested = vexRT[Ch2];
		if (abs(lYRequested - lYLastSent) > slewRateLimit) { //if the new power requested is greater than the slew rate limit
			if (lYRequested > lYLastSent) {
				lY += slewRateLimit; //only increase the power by the max allowed by the slew rate
			} else {
				lY -= slewRateLimit; //only decrease the power by the max allowed by the slew rate
			}
		} else {
			lY = (lYRequested == 0) ? 0 : lY;
		}
		lYLastSent = lY;
		if (abs(rYRequested - rYLastSent) > slewRateLimit) {
			if (rYRequested > rYLastSent) {
				rY += slewRateLimit;
			} else {
				rY -= slewRateLimit;
			}
		} else {
			rY = (rYRequested == 0) ? 0 : rY;
		}
		rYLastSent = rY;
		motor[lDriveFrontMid] = (abs(lY) > threshold) ? lY : 0;
		motor[lDriveBack] = (abs(lY) > threshold) ? lY : 0;
		motor[rDriveFront] = (abs(rY) > threshold) ? rY : 0;
		motor[rDriveMiddle] = (abs(rY) > threshold) ? rY : 0;
		motor[rDriveBack] = (abs(rY) > threshold) ? rY : 0;
		wait1Msec(15);
	}
}

void intakeChainDistance (int encoderCounts, int direction, float power) {
	int encoderGoal = nMotorEncoder[intakeChain] + encoderCounts*direction; //intake encoder counts up for forward
	if (direction == 1) {
		while (nMotorEncoder[intakeChain] > encoderGoal) {
			motor[intakeChain] = power*direction;
		}
	} else {
		while (nMotorEncoder[intakeChain] < encoderGoal) {
			motor[intakeChain] = power*direction;
		}
	}

	motor[intakeChain] = 0; //stop the intake (second stage) so that it doesn't keep going after the target is reached if no other commands are given in the program
}

//needs to be revised
void longShotAuton(bool waitAtStart) {
	//if(waitAtStart) {
	//	wait1Msec(3000);
	//}
	//initializePIDLong();
	//FwVelocitySet(lFly,132.5,.7);
	//FwVelocitySet(rFly,132.5,.7);
	//wait1Msec(2000);
	//intakeDistance(150,1,125);
	//wait1Msec(2000);
	//intakeDistance(150,1,125);
	//wait1Msec(2000);
	//intakeDistance(300,1,125);
	//wait1Msec(2000);
	//intakeDistance(300,1,125);
	//wait1Msec(1500);
	//stopFlywheelAuton(); //use stopFlywheelAuton() function here since a task is used for flywheel stops during driver control
}

//needs to be revised
void closeShotAuton(bool waitAtStart) {
	if(waitAtStart) {
		wait1Msec(3000);
	}
	initializePIDShort();
	FwVelocitySet(lFly, 97.75, .5);
	FwVelocitySet(rFly, 97.75, .5);
	//driveDistance(3350, 1, 85);
	wait1Msec(500);
	//rotate(0,1);
	wait1Msec(250);
	setIntakeMotors(115); //turn on the intake to outtake the balls
	wait1Msec(1750); //wait long enough to shoot all the balls
	setIntakeMotors(0); //stop the intake
	stopFlywheelAuton(); //use stopFlywheelAuton() function here since a task is used for flywheel stops during driver control
}

//needs to be revised
void programmingSkills() {
	startTask(flashLED);
	initializePIDPurple();
	FwVelocitySet(lFly,115,.7);
	FwVelocitySet(rFly,115,.7);
	setIntakeMotors(125);
	wait1Msec(25000);
	stopFlywheelAuton(); //use stopFlywheelAuton() function here since a task is used for flywheel stops during driver control
	setIntakeMotors(0);
	//rotateDegrees(860,1);
	wait1Msec(750);
	//driveDistance(3375, -1, 85);
	setIntakeMotors(125);
	initializePIDPurple();
	FwVelocitySet(lFly,115,.7);
	FwVelocitySet(rFly,115,.7);
	wait1Msec(750);
	//rotateDegrees(895,-1);
	wait1Msec(500);
	setIntakeMotors(125);
	wait1Msec(25000);
}

task autonomous()
{
	if (pgmToRun == "R Side Long" || pgmToRun == "R Back Long"
			|| pgmToRun == "B Side Long"
			|| pgmToRun == "B Back Long") {
			longShotAuton(delayStart);
	} else if (pgmToRun == "B Side Close" || pgmToRun == "B Back Close"
			|| pgmToRun == "R Side Close"
			|| pgmToRun == "R Back Close") {
			closeShotAuton(delayStart);
	} else if (pgmToRun == "Prog. skills") {
			programmingSkills();
	}
}

bool userIntakeControl = true;
task closeShootingMacro() {
	while (1) {
		if (vexRT[Btn5D] == 1 && flywheelMode == 1) { //only run this if the flywheel is in the correct operating state (close shooting only), to prevent mishaps resulting from accidental button presses
			userIntakeControl = false; //prevent user from controlling intake while macro is running
			setIntakeMotors(115); //turn on the intake to outtake the balls
			wait1Msec(1750); //wait long enough to shoot all the balls
			setIntakeMotors(0); //stop the intake
			userIntakeControl = true; //return intake control to user
			flywheelMode = 0.5; //turn off the flywheel.  The stopFlywheel task will recognize this value and stop the flywheel
		}
		wait1Msec(25); //don't hog the CPU
	}
}

task liftController() {
	while(1) {
		if (vexRT[Btn8L] == 1 && vexRT[Btn8D] == 1) {
			//motor[liftPlatform] = 127;
		} else {
			//motor[liftPlatform] = 0;
		}

		if (vexRT[Btn8U] == 1 && vexRT[Btn8L] == 1) {
			//motor[fourBarRelease] = 127;
	  } else {
			//motor[fourBarRelease] = 0;
		}

		wait1Msec(25);
	}
}

task countBallsInIntake() {
	while(1) {

		while(!SensorValue[intakeLimit]) { //wait until the limit switch is pressed
			wait1Msec(25);
		}

		while(SensorValue[intakeLimit]) { //wait until the limit switch is released so we only increment/decrement ballsInIntake once per ball in the intake
			wait1Msec(25);
		}

		//NOTE: this doesn't account for balls leaving the intake via the flywheel
		//reach this point once the intake limit switch has been pressed and then released (so balls are counted after they are done passing the dangle)
		if (vexRT[Btn6U]) { //if the roller is moving forward
			ballsInIntake++; //increment the number of balls in the intake
		} else if (vexRT[Btn6D]) { //if the roller is moving backwards
			ballsInIntake--; //decrement the number of balls in the intake
		}

		//flash the yellow LED to indicate the number of balls the robot thinks are in the intake
		//mainly for debugging
		//red LED turns on if the robot thinks 5 balls are in the intake
		//red LED turns off once count of balls in intake goes below 5
		if (flywheelMode == 0) {
			switch (ballsInIntake) {
				case 0:
					yellowLEDFlashTime = 1000;
					SensorValue[redLED] = false;
					break;
				case 1:
					yellowLEDFlashTime = 500;
					SensorValue[redLED] = false;
					break;
				case 2:
					yellowLEDFlashTime = 250;
					SensorValue[redLED] = false;
					break;
				case 3:
					yellowLEDFlashTime = 125;
					SensorValue[redLED] = false;
					break;
				case 4:
					yellowLEDFlashTime = 62.5;
					SensorValue[redLED] = false;
					break;
				case 5:
					SensorValue[redLED] = true;
					break;
			}
		}

	}
}

task autoIntake() {
	while(1) {
		if(SensorValue[intakeLimit] && !vexRT[Btn5D] && ballsInIntake < 3) { //if the intake limit is pressed
			//The condition is ballsInIntake < 3 (rather than ballsInIntake <= 3) because the flow of the countBallsInIntake task is:
			//1) Wait for intake limit switch to be pressed, 2) wait for intake limit switch to be released, 3) count the new ball as being in the intake
			//The first ball enters the intake, limit switch released.  ballsInIntake = 1
			//Second ball presses, releases limit switch, ballsInIntake = 2
			//Third balls presses, releass limit switch, ballsInIntake = 3 AFTER the limit switch is released for this ball.  Thus, we don't want the intake to
			//  move automatically once ballsInIntake = 3
			userIntakeControl = false;
			intakeChainDistance(450, 1, 125);
			userIntakeControl = true;
			while(SensorValue[intakeLimit]) { //wait until the intake limit switch is no longer pressed so that the moveIntakeChain command doesn't run multiple times
				wait1Msec(25);
			}
		} else {
			userIntakeControl = true;
		}
		wait1Msec(25);
	}
}

void moveIntakeBack() {
	userIntakeControl = false;
	setIntakeMotors(-127);
	wait10Msec(10);
	setIntakeMotors(0);
	wait10Msec(20);
	userIntakeControl = true;
}

//needs intake line follower sensor
bool outtakeOnly = false;
task intakeWatchDog() {
	//while(1) {
	//	if(flywheelMode > 0 && lFly.current < 30 && rFly.current < 30) {
	//		if(SensorValue[intakeBall] < 1500) { //sonar sensor values that indicate the presence of a ball
	//			outtakeOnly = true;
	//			moveIntakeBack(); //move the intake back so that the ball is not touching the flywheel
	//			wait1Msec(300);
	//		}
	//	} else {
	//			outtakeOnly = false;
	//	}
	//	wait1Msec(25);
	//}
}


task flywheelWatchdog() {
	while(1) {
		if (flywheelMode >= 1) { //if the flywheel is supposed to be running
			if (lFly.current == 0 || rFly.current == 0) { //if one side of the flywheel is not moving
					wait1Msec(275); //wait half a second to see if the flywheel just needs time to start
					if (lFly.current == 0 || rFly.current == 0) { //if the flywheel is still not moving
						flywheelMode = 0.5; //stop the flywheel (stopFlywheel task)
						SensorValue[redLED] = true; //turn on the red LED
					}
			}
		}
		if ((lFly.current > 0 || rFly.current > 0) && flywheelMode >= 1) { //if the flywheel is moving
			SensorValue[redLED] = false; //turn off the red LED since the flywheel is OK now
		}
		wait1Msec(25);
	}
}

task usercontrol()
{
	//initalize tasks to control various subsystems that need to run concurrently during driver control
	//TODO: move the lift controller task into here
	//TODO: see if any intake tasks can be consolidated
	//some tasks below have not been tested yet and/or lack necessary hardware or sensors.  That's why they are commented out:
	// -closeShootingMacro: not tuned
	// -intakeWatchdog: no line follower on intake
	// -liftController: actuation mechanisms not finished

	//startTask(closeShootingMacro);
	//startTask(drivetrainController);
	//startTask(intakeWatchDog);
	startTask(flashLED);
	//startTask(liftController);
	//startTask(stopFlywheel);
	//startTask(flywheelWatchdog);

		initializePIDShort();
		FwVelocitySet(lFly, 103, .5);
		FwVelocitySet(rFly, 103, .5);
	  yellowLEDFlashTime = 320;
	  userIntakeControl = false;
	  motor[intakeChain] = 127;
	  motor[intakeRoller] = 127;

	//need to update these with latest versions
	//startTask(autoIntake);
	//startTask(countBallsInIntake);

	while (true)
	{
		//intake
		if(userIntakeControl) {

			if(vexRT[Btn5U] == 1) { //TODO: need a new button for this command
				//moveIntakeBack();
			} else if (!outtakeOnly) { //if the program is not overriding control of the intake
				motor[intakeChain] = vexRT[Btn5U]*125 - vexRT[Btn5D]*125;
				motor[intakeRoller] = vexRT[Btn6U]*125 - vexRT[Btn6D]*125;
			} else if (outtakeOnly) {
				motor[intakeChain] = -vexRT[Btn5D]*125;
				motor[intakeRoller] = -vexRT[Btn6D]*125;
			}
		}

		//flywheel speed control
		//7U - long, 7R - purple, 7D - short
		//8R - stop, 8D - if short shooting (flywheel must be on and in short shooting mode), outtake 4 balls and then stop the flywheel automatically
		if (vexRT[Btn7U] == 1 && flywheelMode != 4) { //second condition prevents reinitialization of long shooting if the flywheel is currently in long shooting mode
			//mode 0.5 is for when the flywheel has been shutdown but is still spinning.  Since the control tasks are used for this process, the flywheel tasks need to be restarted.
			if (flywheelMode >= 0.5) { //if the flywheel is currently running (modes 0.5,1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
			}

			//for now, assume that starting the flywheel means that balls are going to be shot and the ballsInIntake variable can be reset to 0
			//  this will likely be changed in the future once we can count balls exiting the intake
			ballsInIntake = 0;

			//next 4 lines have to run every time to run flywheel
			flywheelMode = 4; //make sure we set the flywheel mode
			initializePIDLong(); //prepare controller for long shooting
			//set long shooting velocities
		  FwVelocitySet(lFly,138,.7);
	    FwVelocitySet(rFly,138,.7);

	    yellowLEDFlashTime = 320; //flash the yellow LED for pacing
			userIntakeControl = false; //disable user intake control so the intake can run automatically
	    setIntakeMotors(127);
		} else if (vexRT[Btn7R] == 1 && flywheelMode != 3) { //purple shooting
			//mode 0.5 is for when the flywheel has been shutdown but is still spinning.  Since the control tasks are used for this process, the flywheel tasks need to be restarted.
			if (flywheelMode >= 0.5) { //if the flywheel is currently running (modes 0.5,1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
				userIntakeControl = true;
			}

			//for now, assume that starting the flywheel means that balls are going to be shot and the ballsInIntake variable can be reset to 0
			//  this will likely be changed in the future once we can count balls exiting the intake
			ballsInIntake = 0;

			//next 4 lines have to run every time to run flywheel
			flywheelMode = 3;
			initializePIDPurple();
			FwVelocitySet(lFly,118.5,.7);
			FwVelocitySet(rFly,118.5,.7);

		} else if (vexRT[Btn7D] == 1 && flywheelMode != 1) { //close shooting
			//mode 0.5 is for when the flywheel has been shutdown but is still spinning.  Since the control tasks are used for this process, the flywheel tasks need to be restarted.
			if (flywheelMode >= 0.5) { //if the flywheel is currently running (modes 0.5,1-4), we need to stop the controller tasks before re-initializing the PID controller
				stopTask(leftFwControlTask);
				stopTask(rightFwControlTask);
				userIntakeControl = true;
			}

			//for now, assume that starting the flywheel means that balls are going to be shot and the ballsInIntake variable can be reset to 0
			//  this will likely be changed in the future once we can count balls exiting the intake
			ballsInIntake = 0;

			//next 4 lines have to run every time to run flywheel
			flywheelMode = 1;
			initializePIDShort();
			FwVelocitySet(lFly, 103, .5);
			FwVelocitySet(rFly, 103, .5);

		} else if (vexRT[Btn8R] == 1 && flywheelMode >= 1) { //this is an else statement so that if two buttons are pressed, we won't switch back and forth between starting and stopping the flywheel
																												 //  flywheelMode needs to be >=1 and not >=0.5 because we don't want to stop the flywheel again if it is currently in the process of the stopping,
																												 //  although since the value of flywheelMode would not change in that case, it would appear as if nothing happened
			userIntakeControl = true; //make sure the driver can control the intake again
			//below line triggers flywheel shutdown procedure
			flywheelMode = 0.5;
		}


		//writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",nPgmTime,lFly.current, lFly.motor_drive, lFly.p, lFly.i, lFly.d, lFly.constant, 50*lFly.postBallLaunch, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d, rFly.constant, 60*rFly.postBallLaunch);

		wait1Msec(25); //don't overload the CPU
	}
}
