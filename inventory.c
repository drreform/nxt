 #pragma config(Sensor, S1, colorSensor, sensorCOLORFULL)

-------------getBarcode-------------//
 int barCode[4];
 int * getBarcode(){
	 barCode[0]=1;
	 int k=0;
	 int i=1;

	 while (i < 4 ) {

		 while ( SensorValue[colorSensor]!= REDCOLOR &&
						 SensorValue[colorSensor]!= BLUECOLOR &&
						 SensorValue[colorSensor]!= WHITECOLOR &&
						 SensorValue[colorSensor]!= YELLOWCOLOR &&
						 SensorValue[colorSensor]!= GREENCOLOR ) {
			// do nothing -- just wait for above colors
		 }

		 if (SensorValue [ colorSensor ]!=	 barCode[i-1] &&	SensorValue [ colorSensor ]!= BLACKCOLOR){
			 barCode[i]= SensorValue [ colorSensor ];
			 k = 10 * k + barCode[i];
			 i=i+1;
		 }
	 }

	 sendMessageWithParm(WEBSERVER, STT_BARCODE , k);
	 return	barCode;
 }


 task main()
 {


	 getBarcode();
 }
