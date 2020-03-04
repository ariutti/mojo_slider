// see this example for reference: https://www.arduino.cc/en/Tutorial/TwoPortReceive

#include <SoftwareSerial.h>
#include "TFMini.h"

// Using an Arduino micro with a TFmini Lidar via SoftwareSerial port and a LogicLevelConverter.
// We are using two SoftwareSerial and talking with two sensors simultaneously.


// SETUP SERIAL PORTS ***********************************/

// NOTE: Not all pins on the Leonardo and Micro support change interrupts,
// so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 // software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial mySerialA(10, 11);
 // software serial #2: RX = digital pin 8, TX = digital pin 9
SoftwareSerial mySerialB(8, 9);

TFMini tfminiA, tfminiB;

#define PAUSE 25

// FILTERSTUFF ******************************************/

float A = 0.1;
float B = 1.0 - A;
// raw values
int16_t distA, strenghtA;
// filtered values
float fDistA, fPrevDistA;

// raw values
int16_t distB, strenghtB;
// filtered values
float fDistB, fPrevDistB;

// probably max and min value which I will capture will be
// min: 37  cm
// max: 137 cm

bool triggerFlag;
bool trigger; // true: lidar1 - false lidar2

// SETUP ////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect.
  while (!Serial) {};
  Serial.println("Serial communication started");
  delay(1000);

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialA.begin( TFMINI_BAUDRATE );
  Serial.println("serial A begin");
  // Step 3: Initialize the TF Mini sensor in external triggering mode≥
  tfminiA.begin( &mySerialA, true );
  Serial.println("lidar A begin");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialB.begin( TFMINI_BAUDRATE );
  Serial.println("serial B begin");
  // Step 3: Initialize the TF Mini sensor in external triggering mode≥
  tfminiB.begin( &mySerialB, true );
  Serial.println("lidar B begin");

	triggerFlag = false;
}


// LOOP /////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
	// Reads from serial and define what lidar to trigger
	if (Serial.available() > 0){
		int tmp = Serial.read();
		triggerFlag = true;
		switch (tmp) {
			case 1:
				trigger = true
			break;
			case 2:
				trigger = false;
			break;
			default:
				triggerFlag = false;
			break;
		}
	}

	if (triggerFlag) {
		// send proper trigger
		if (trigger) {
			mySerialA.listen();
		  // Take one TF Mini distance measurement
		  int16_t distA = tfminiA.getDistance();
		  int16_t strengthA = tfminiA.getRecentSignalStrength();

		  fDistA = distA * A + fPrevDistA * B;
		  fPrevDistA = fDistA;

		  Serial.print("A: ");
		  Serial.print( int(fDistA) );
		  Serial.println("\t");
		} else {
			// Now listen on the second port
		  mySerialB.listen();
		  // Take one TF Mini distance measurement
		  int16_t distB = tfminiB.getDistance();
		  int16_t strengthB = tfminiB.getRecentSignalStrength();

		  fDistB = distB * A + fPrevDistB * B;
		  fPrevDistB = fDistB;

		  Serial.print("B: ");
		  Serial.print( int(fDistB) );
		  Serial.println(";");
		}
		triggerFlag = false;
	}

  // Wait some short time before taking the next measurement
  delay( PAUSE );
}
