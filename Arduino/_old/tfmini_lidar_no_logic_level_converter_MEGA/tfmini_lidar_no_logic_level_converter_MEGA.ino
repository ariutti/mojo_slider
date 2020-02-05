#include "TFMini.h"

TFMini tfmini;

#define PAUSE 25
bool stringComplete = false;
String inputString = "";
bool bSendSerial = false;


// SETUP ////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(9600);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial) {};

  delay(1000);
     
  Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  Serial1.begin(TFMINI_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&Serial1);    
}


// LOOP /////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // Take one TF Mini distance measurement
  int16_t dist = tfmini.getDistance();
  int16_t strength = tfmini.getRecentSignalStrength();

  /*
  if( dist == -1)
  {
    Serial.println("\twe get an error!");
    //tfmini.begin(&mySerial);
  }
  */ 

  if (bSendSerial) {
    // Display the measurement
    Serial.print(dist);
    Serial.print("\t");
    Serial.print(" cm      sigstr: ");
    Serial.println(strength);
    
    //Serial.print(dist); Serial.print("_");Serial.println(strength);
  }

  if(stringComplete) {
    if( inputString[0] == 's' ) 
    { 
      //Serial.println("corrisponde ------------------------------------------------------->"); 
      //tfmini.setStandardOutputMode();
      bSendSerial = !bSendSerial;
      
    };
    //Serial.println( inputString );
    //Serial.println( inputString.length() );
    stringComplete = false;
    inputString="";
  }

  // Wait some short time before taking the next measurement
  delay( PAUSE );  
}


// SERIAL EVENT /////////////////////////////////////////////////////////////////////////////////
void serialEvent() 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') 
    {
      stringComplete = true;
    }
  }
}
