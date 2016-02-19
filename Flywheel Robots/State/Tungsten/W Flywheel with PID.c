#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_5,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_6,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           intakeRoller,  tmotorVex393TurboSpeed_HBridge, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port2,           rFlyTop,       tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           rFlyBottom,    tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port4,           rDriveFront,   tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_5)
#pragma config(Motor,  port5,           rDriveBack,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port6,           lDriveBack,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           lDriveFront,   tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_6)
#pragma config(Motor,  port8,           lFlyTop,       tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           lFlyBottom,    tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port10,          intakeChain,   tmotorVex393TurboSpeed_HBridge, openLoop, reversed, encoderPort, I2C_4)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
#include "..\Global\Simple PID Controller.h"

fw_controller lFly, rFly;
string str;

void pre_auton()
{
	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;

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
	tbhInit(lFly, 392, 0.2481, 0.6481, 0.005481, 0, 75, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.2481, 0.6481, 0.005481, 0, 75, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

//short shooting
void initializePIDShort() {
	//note the order of the parameters:
	//(controller, motor ticks per rev, KpNorm, KpBallLaunch, Ki, Kd, constant, RPM drop on ball launch)
	tbhInit(lFly, 392, 0.7481, 0.8481, 0.005481, 0, 50, 20); //initialize PID for left side of the flywheel //left side might be able to have a higher P
	tbhInit(rFly, 392, 0.7481, 0.8481, 0.005481, 0, 50, 20); //initialize PID for right side of the flywheel //x.x481
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}
task autonomous()
{

}

int lSpeed = 55; //Added For Short Shot Test -- Crawford
int rSpeed = 55; //Added For Short Shot Test -- Crawford
//int lSpeed = 70; // Evan's Long Shot
//int rSpeed = 70; // Evan's Long Shot
task usercontrol()
{
	writeDebugStreamLine("nPgmTime,lFly.current, lFly.motor_drive, lFly.p, lFly.i, lFly.d, lFly.constant, 50*lFly.postBallLaunch, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d, rFly.constant, 60*rFly.postBallLaunch");
	setLeftFwSpeed(lSpeed);
	setRightFwSpeed(rSpeed);
	wait1Msec(500);

	//short shooting
	initializePIDShort();
	FwVelocitySet(lFly, 83, .5); //Added For Short Shot Test -- Crawford
	FwVelocitySet(rFly, 83, .5); //Added For Short Shot Test -- Crawford

	//long shooting
	//initializePIDLong();
	//FwVelocitySet(lFly,136,.7); // Evan's Long Shot 141
	//FwVelocitySet(rFly,136,.7); // Evan's Long Shot 141

	//short shooting

	while (true)
	{
		motor[intakeChain] = 125;
		motor[intakeRoller] = 125;
		//writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d,%d",rFly.encoder_timestamp, rFly.e_current, rFly.error, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d);
	  writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",nPgmTime,lFly.current, lFly.motor_drive, lFly.p, lFly.i, lFly.d, lFly.constant, 50*lFly.postBallLaunch, rFly.current, rFly.motor_drive, rFly.p, rFly.i, rFly.d, rFly.constant, 60*rFly.postBallLaunch);
		wait1Msec(25);
	}
}
