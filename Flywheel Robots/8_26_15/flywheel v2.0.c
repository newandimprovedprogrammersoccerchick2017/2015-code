#pragma config(Motor,  port1,           LFfly,         tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           LBfly,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           RBfly,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           RFfly,         tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
  while(1 == 1)
  {
    if(vexRT[Btn6U] == 1)
    {
      	motor[LBfly] = 127;
				motor[RBfly] = 127;
				motor[LFfly] = 127;
				motor[RFfly] = 127;
    }
    else if(vexRT[Btn6D] == 1)
    {
      	motor[LBfly] = -127;
				motor[RBfly] = -127;
				motor[LFfly] = -127;
				motor[RFfly] = -127;
    }
    else
    {
      	motor[LBfly] = 0;
				motor[RBfly] = 0;
				motor[LFfly] = 0;
				motor[RFfly] = 0;
    }
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
