#pragma config(Motor,  port2,           lDriveFront,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           lDriveBack,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           rDriveFront,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           rDriveBack,    tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*

*/
task main()
{

  while(1)
  {
  	motor[lDriveFront] = vexRT[Ch3] - vexRT[Ch4];
  	motor[lDriveBack] = vexRT[Ch3] + vexRT[Ch4];

  //Remote Control Commands
    motor[frontRight] = vexRT[Ch3] - vexRT[Ch1] - vexRT[Ch4];
    motor[backRight] = vexRT[Ch3] - vexRT[Ch1] + vexRT[Ch4];
    motor[frontLeft] = vexRT[Ch3] + vexRT[Ch1] + vexRT[Ch4];
    motor[backLeft] =  vexRT[Ch3] + vexRT[Ch1] - vexRT[Ch4];
  }

}
