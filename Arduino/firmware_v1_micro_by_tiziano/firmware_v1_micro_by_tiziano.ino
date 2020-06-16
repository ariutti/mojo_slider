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
#define DEBUG
#define DEBUGVAL true
#define VVVVSEND




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


// MECH SETTINGS ****************************************/

float startDistA = 0;
float startDistB = 0;
float centerVal = 0;
float mapVal = 0;

// MECH SETTINGS ****************************************/



bool bSendSerial = true;

// probably max and min value which I will capture will be
// min: 37  cm
// max: 137 cm

// SETUP ////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect.
  while (!Serial) {};
#ifdef DEBUG
  Serial.println("Serial communication started");
#endif
  delay(1000);



  //Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialA.begin( TFMINI_BAUDRATE );
#ifdef DEBUG
  Serial.println("serial A begin");
#endif
  // Step 3: Initialize the TF Mini sensor
  tfminiA.begin( &mySerialA );
#ifdef DEBUG
  Serial.println("lidar A begin");
#endif



  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialB.begin( TFMINI_BAUDRATE );
#ifdef DEBUG
  Serial.println("serial B begin");
#endif
  // Step 3: Initialize the TF Mini sensor
  tfminiB.begin( &mySerialB );
#ifdef DEBUG
  Serial.println("lidar B begin");
#endif
}


// LOOP /////////////////////////////////////////////////////////////////////////////////////////
void loop()



{
  readSerial();

  // By default, the last intialized port is listening.
  // when you want to listen on a port, explicitly select it:
  mySerialA.listen();
  // Take one TF Mini distance measurement
  int16_t distA = tfminiA.getDistance();
  int16_t strengthA = tfminiA.getRecentSignalStrength();

  fDistA = distA * A + fPrevDistA * B;
  fPrevDistA = fDistA;

#ifdef DEBUGVAL
  if (bSendSerial) {
    Serial.print("A: ");
    Serial.print( int(fDistA) );
    Serial.print("-");
    Serial.print( int(strengthA) );
    Serial.print(";  ");
  }
#endif

  // Now listen on the second port


  mySerialB.listen();
  // Take one TF Mini distance measurement
  int16_t distB = tfminiB.getDistance();
  int16_t strengthB = tfminiB.getRecentSignalStrength();

  fDistB = distB * A + fPrevDistB * B;
  fPrevDistB = fDistB;

#ifdef DEBUGVAL
  if (bSendSerial) {
    Serial.print("B: ");
    Serial.print( int(fDistB) );
    Serial.print("-");
    Serial.print( int(strengthB) );
    Serial.print("; ");
  }
#endif

  //MEDIA A-B ******************************************/
  centerVal = fDistA - fDistB;
#ifdef DEBUGVAL
  Serial.print("Center val: "); Serial.print( centerVal ); Serial.print(" ---> ");
#endif
  mapVal = map(centerVal, -93, 65, 0, 1000);
  mapVal = constrain(mapVal, 0, 1000);
#ifdef VVVVSEND
  Serial.println( int(mapVal) );
#endif

  //****************************************************/



  // Wait some short time before taking the next measurement
  delay( PAUSE );
}

void readSerial() {
  if (Serial.available() > 0) {
    byte c = Serial.read();
    // Processing can ask Arduino to send some raw
    // data in order to get a plotted version of them
    if (c == 'o') {
      bSendSerial = true;
    }
    else if (c == 'c') {
      bSendSerial = false;
    }
  }
}



// The function below will not work on Arduino Micro
/************************************************************************************
   SERIAL EVENT
 ***********************************************************************************
  void serialEvent()
  {
  byte c = Serial.read();
  // Processing can ask Arduino to send some raw
  // data in order to get a plotted version of them
  if (c == 'o') {
    bSendSerial = true;

  }
  else if (c == 'c') {
    bSendSerial = false;
  }
  else if (c == 'r')
  {
    // do nothing
  }
  }
*/
