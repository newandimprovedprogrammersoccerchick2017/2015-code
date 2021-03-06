void checkBatteries() {
	float mainLevel = nAvgBatteryLevel/1000.0; //main battery level
	float backupLevel = BackupBatteryLevel/1000.0; //backup battery level
	float secondLevel = (SensorValue[powerExpanderStatus]/70)*4; //power expander battery level
	int outputOption = 0;
	if(mainLevel < 8.0) {
			outputOption += 1; //1, 3, and 5 are used for these numbers because all possible sums will be unique
	}
	if(backupLevel < 6.0) {
		outputOption += 3;
	}
	if(secondLevel < 8.0) {
		outputOption += 5;
	}
	switch (outputOption) {
		case 1:
			displayLCDCenteredString(0, "Critical:");
			displayLCDCenteredString(1, "Main battery low");
			break;
		case 3:
			displayLCDCenteredString(0, "Warning:   Check");
			displayLCDCenteredString(1, "backup battery");
			break;
		case 5:
			displayLCDCenteredString(0, "Critical:");
			displayLCDCenteredString(1, "PE battery low");
			break;
		case 4: //main and backup batteries low
			displayLCDCenteredString(0, "Critical: Main +");
			displayLCDCenteredString(1, "backup batts low");
			break;
		case 8: //power expander and backup batteries low
			displayLCDCenteredString(0, "Critical: PE +");
			displayLCDCenteredString(1, "backup batts low");
			break;
		case 6: //main and power expander batteries low
			displayLCDCenteredString(0, "Critical: Main");
			displayLCDCenteredString(1, "and PE batts low");
			break;
		case 9: //All (main, power expander, and backup) batteries low
			displayLCDCenteredString(0, "Blocker: All");
			displayLCDCenteredString(1, "batteries LOW");
			break;
	}
}
