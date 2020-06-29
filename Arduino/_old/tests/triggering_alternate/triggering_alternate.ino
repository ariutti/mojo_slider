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

#define DEBUG false

#define PAUSE 50

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


// SETUP ////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect.
  while (!Serial) {};
  if( DEBUG ) Serial.println("Serial communication started");
  delay(1000);

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialA.begin( TFMINI_BAUDRATE );
  if( DEBUG ) Serial.println("serial A begin");
  // Step 3: Initialize the TF Mini sensor in external triggering mode≥
  tfminiA.begin( &mySerialA, true );
  if( DEBUG ) Serial.println("lidar A begin");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerialB.begin( TFMINI_BAUDRATE );
  if( DEBUG ) Serial.println("serial B begin");
  // Step 3: Initialize the TF Mini sensor in external triggering mode≥
  tfminiB.begin( &mySerialB, true );
  if( DEBUG ) Serial.println("lidar B begin");

}


// LOOP /////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
 
  //Serial.println("trying to send trigger to the A");
  mySerialA.listen();
  tfminiA.sendTrigger();
  delay(10);
  // Take one TF Mini distance measurement
  int16_t distA = tfminiA.getDistance();
  int16_t strengthA = tfminiA.getRecentSignalStrength();

  //fDistA = distA * A + fPrevDistA * B;
  //fPrevDistA = fDistA;

  //Serial.print("A: ");
  Serial.print( distA );
  Serial.print("\t");
  //Serial.print("strenght: ");
  //Serial.print( strengthA );
  //Serial.println();

  // pause between the two sensor reading
  delay( PAUSE );

  //Serial.println("trying to send trigger to the B");
  mySerialB.listen();
  tfminiB.sendTrigger();
  delay(10);
  // Take one TF Mini distance measurement
  int16_t distB = tfminiB.getDistance();
  int16_t strengthB = tfminiB.getRecentSignalStrength();

  //fDistB = distB * A + fPrevDistB * B;
  //fPrevDistB = fDistB;

  //Serial.print("B: ");
  Serial.print( distB );
  //Serial.print("\t");
  //Serial.print("strenght: ");
  //Serial.print( strengthB );

  
  Serial.println();

  // pause between the two sensor reading
  delay( PAUSE );
}
