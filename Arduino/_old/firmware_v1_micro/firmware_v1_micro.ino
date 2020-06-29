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

bool bSendSerial = false;

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
  Serial.println("Serial communication started");
  delay(1000);
     
  //Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialA.begin( TFMINI_BAUDRATE );
  Serial.println("serial A begin");
  // Step 3: Initialize the TF Mini sensor
  tfminiA.begin( &mySerialA ); 
  Serial.println("lidar A begin");  

  
  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialB.begin( TFMINI_BAUDRATE );
  Serial.println("serial B begin");
  // Step 3: Initialize the TF Mini sensor
  tfminiB.begin( &mySerialB );
  Serial.println("lidar B begin");
  
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

  if(bSendSerial) {
    Serial.println("serial communication active");
  }
  if(bSendSerial) { 
    Serial.print("A: ");
    Serial.print( int(fDistA) ); 
    Serial.println("\t"); 
  }

  // Now listen on the second port
  mySerialB.listen();
  // Take one TF Mini distance measurement
  int16_t distB = tfminiB.getDistance();
  int16_t strengthB = tfminiB.getRecentSignalStrength();

  fDistB = distB * A + fPrevDistB * B;
  fPrevDistB = fDistB;

  if(bSendSerial) { 
    Serial.print("B: ");
    Serial.print( int(fDistB) ); 
    Serial.println(";");
  }
  

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
 * SERIAL EVENT
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
