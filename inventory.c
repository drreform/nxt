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


int r,g,b;
int R,G,B;
int x;
task main()
{
		short nAtoDValues[4];
	short nRawValues[4];
	while(true){
		  getColorSensorData(colorSensor, colorAtoD,   &nAtoDValues[0]);
  		getColorSensorData(colorSensor, colorRaw,    &nRawValues[0]);
  		r = nAtoDValues[0];
  		g = nAtoDValues[1];
  		b = nAtoDValues[2];
  		R = nRawValues[0];
  		G = nRawValues[0];
  		B = nRawValues[0];
  		x = SensorValue[colorSensor];
	}
	getBarcode();
}
