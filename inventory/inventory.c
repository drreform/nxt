#pragma config(Sensor, S1, colorSensor, sensorCOLORFULL)

//-------------getBarcode-------------//
int barCode[4];
int * getBarcode(){
	barCode[0]=1;
	int i=1;

	while (i < 4 ) {

		while ( SensorValue[colorSensor]!= REDCOLOR &&
						SensorValue[colorSensor]!= BLUECOLOR &&
						SensorValue[colorSensor]!= WHITECOLOR &&
						SensorValue[colorSensor]!= YELLOWCOLOR &&
						SensorValue[colorSensor]!= GREENCOLOR ) {
			// do nothing -- just wait for red or blue color
		}

		if (SensorValue [ colorSensor ]!=	 barCode[i-1] &&	SensorValue [ colorSensor ]!= BLACKCOLOR){
			barCode[i]= SensorValue [ colorSensor ];
			i=i+1;
		}

	}
	return	barCode;
}



task main()
{
	getBarcode();
}
