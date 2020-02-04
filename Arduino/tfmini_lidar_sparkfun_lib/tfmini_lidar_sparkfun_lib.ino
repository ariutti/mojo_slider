// https://github.com/betaflight/betaflight/issues/7862


/*
Example code for Benewake TFMini time-of-flight distance sensor. 
by Peter Jansen (December 11/2017)
This example code is in the public domain.

This example communicates to the TFMini using a SoftwareSerial port at 115200, 
while communicating the distance results through the default Arduino hardware
Serial debug port. 

SoftwareSerial for some boards can be unreliable at high speeds (such as 115200). 
The driver includes some limited error detection and automatic retries, that
means it can generally work with SoftwareSerial on (for example) an UNO without
the end-user noticing many communications glitches, as long as a constant refresh
rate is not required. 

The (UNO) circuit:
 * Uno RX is digital pin 10 (connect to TX of TF Mini)
 * Uno TX is digital pin 11 (connect to RX of TF Mini)

THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

#include <SoftwareSerial.h>
#include "TFMini.h"

// Setup software serial port 
SoftwareSerial mySerial(10, 11);      // Uno RX (TFMINI TX), Uno TX (TFMINI RX)
TFMini tfmini;

#define PAUSE 25
bool stringComplete = false;
String inputString = "";


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
  mySerial.begin(TFMINI_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&mySerial);    
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

  // Display the measurement
  Serial.println(dist);
  //Serial.print("\t");
  //Serial.print(" cm      sigstr: ");
  //Serial.println(strength);
  
  //Serial.print(dist); Serial.print("_");Serial.println(strength);

  if(stringComplete) {
    if( inputString[0] == 'a' ) 
    { 
      //Serial.println("corrisponde ------------------------------------------------------->"); 
      tfmini.setStandardOutputMode();
      
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
