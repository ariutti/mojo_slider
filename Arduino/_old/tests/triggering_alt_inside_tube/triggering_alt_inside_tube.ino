// see this example for reference: https://www.arduino.cc/en/Tutorial/TwoPortReceive

#include <SoftwareSerial.h>
#include "TFMini.h"

// Using an Arduino micro with a TFmini Lidar via SoftwareSerial port and a LogicLevelConverter.
// We are using two SoftwareSerial and talking with two sensors simultaneously.


// SETUP SERIAL PORTS ***********************************/

// NOTE: Not all pins on the Leonardo and Micro support change interrupts,
// so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

// software serial #1: RX = digital pin  8, TX = digital pin  9
SoftwareSerial mySerialSX(8, 9);
// software serial #2: RX = digital pin 10, TX = digital pin 11
SoftwareSerial mySerialDX(10, 11);

TFMini tfminiSX, tfminiDX;

#define DEBUG false
#define PAUSE 50

#define READ_SX false
#define READ_DX true
#define READ_BOTH false

// FILTERSTUFF ******************************************/

float A = 0.1;
float B = 1.0 - A;

// raw values
int16_t distSX, strengthSX;
// filtered values
float fDistSX, fPrevDistSX;

// raw values
int16_t distDX, strengthDX;
// filtered values
float fDistDX, fPrevDistDX;


// SETUP ////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect.
  while (!Serial) {};
  if( DEBUG ) Serial.println("Serial communication started");
  delay(1000);

  if( READ_SX ){
    // Step 2: Initialize the data rate for the first SoftwareSerial port
    mySerialSX.begin( TFMINI_BAUDRATE );
    if( DEBUG ) Serial.println("serial SX begin");
    // Step 3: Initialize the TF Mini sensor in external triggering mode
    tfminiSX.begin( &mySerialSX, true );
    if( DEBUG ) Serial.println("lidar SX begin");
  }
  
  if( READ_DX ) {
    // Step 2: Initialize the data rate for the second SoftwareSerial port
    mySerialDX.begin( TFMINI_BAUDRATE );
    if( DEBUG ) Serial.println("serial DX begin");
    // Step 3: Initialize the TF Mini sensor in external triggering mode
    tfminiDX.begin( &mySerialDX, true );
    if( DEBUG ) Serial.println("lidar DX begin");
  }

}


// LOOP /////////////////////////////////////////////////////////////////////////////////////////
void loop()
{

  if( READ_SX) {
    //Serial.println("trying to send trigger to the SX");
    mySerialSX.listen();
    tfminiSX.sendTrigger();
    delay(10);
    // Take one TF Mini distance measurement
    distSX = tfminiSX.getDistance();
    strengthSX = tfminiSX.getRecentSignalStrength();
    if( distSX == -1) { /* c'è stato un errore gestito dalla libreria */};

    // debug, fai un check sulla distanza negativa 
    // e sulla forza negativa (in caso scarta il valore)
  
    //fdistSX = distSX * A + fPrevDistSX * B;
    //fPrevDistSX = fdistSX;
    
    Serial.print("SX: ");
    Serial.print( distSX );
    Serial.print("\t");
    Serial.print("strenght: ");
    Serial.print( strengthSX );
    Serial.println();
    
    // pause between the two sensor reading
    delay( PAUSE );
  }

  if( READ_DX ) {
    //Serial.println("trying to send trigger to the DX");
    mySerialDX.listen();
    tfminiDX.sendTrigger();
    delay(10);
    // Take one TF Mini distance measurement
    distDX = tfminiDX.getDistance();
    strengthDX = tfminiDX.getRecentSignalStrength();
    if( distDX == -1) { /* c'è stato un errore gestito dalla libreria */};
  
    //fDistDX = distDX * A + fPrevDistDX * B;
    //fPrevDistDX = fDistDX;

    Serial.print("DX: ");
    Serial.print( distDX );
    Serial.print("\t");
    Serial.print("strenght: ");
    Serial.print( strengthDX );
    Serial.println();
  
    // pause between the two sensor reading
    delay( PAUSE );
  }

  // DEBUG: fai un check sulla distanza negativa 
  // e sulla forza negativa (in caso scarta il valore)

  if (READ_BOTH ) {
    //Serial.println("trying to send trigger to the SX");
    mySerialSX.listen();
    tfminiSX.sendTrigger();
    delay(10);
    // Take one TF Mini distance measurement
    distSX = tfminiSX.getDistance();
    strengthSX = tfminiSX.getRecentSignalStrength();
    if( distSX == -1) { /* c'è stato un errore gestito dalla libreria */};
    
    //fdistSX = distSX * A + fPrevDistSX * B;
    //fPrevDistSX = fdistSX;

    delay( PAUSE );

    //Serial.println("trying to send trigger to the DX");
    mySerialDX.listen();
    tfminiDX.sendTrigger();
    delay(10);
    // Take one TF Mini distance measurement
    distDX = tfminiDX.getDistance();
    strengthDX = tfminiDX.getRecentSignalStrength();
    if( distDX == -1) { /* c'è stato un errore gestito dalla libreria */};
  
    //fDistDX = distDX * A + fPrevDistDX * B;
    //fPrevDistDX = fDistDX;

    Serial.print("SX: ");
    Serial.print( distSX );
    Serial.print("\t");
    Serial.print("strenght: ");
    Serial.print( strengthSX );
    Serial.print("\t");

    Serial.print("DX: ");
    Serial.print( distDX );
    Serial.print("\t");
    Serial.print("strenght: ");
    Serial.print( strengthDX );
    Serial.println();

    delay( PAUSE );  
  }
}
