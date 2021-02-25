/*
 * CIRCUIT
 * 
 *                  O
 * [________________A________________________]
 *  
 *  left                                right
 *  reset                               reset
 *  
 *                                    encoder
 *                            strip frist LED
 *                            
 *
 * Component:
 * - 1x Arduino micro;
 * - 1x Bourns optical relative encoder ();
 * - 2x Panasonic PM45 optical switch;
 * - 1x Adafruit DotStar 144LED/m strip;
 * - 1x LED 5V power supply (smartphone USB power supply will do the job)
 * - 2x 470 ohm resistor;
 * - 1x 1000µF 6.1V electrolitic cap;
 *
 * Connect:
 * - Arduino pin 2 to encoder CHA (this rail has a physical 10K pullup resistor - green cable);
 * - Arduino pin 3 to encoder CHB (this rail has a physical 10K pullup resistor - yellow cable);
 * - Arduino pin 6 to the left Panasonic PM45 white cable (no physical pullup resistor);
 *  - Arduino pin 7 to the right Panasonic PM45 white cable (no physical pullup resistor)
 * - Arduino pin 8 to the DotStar LED strip DIN pin via a 470 Ohm resistor;
 * - Arduino pin 9 to the DotStar LED strip CLK pin via a 470 Ohm resistor;
 * - Arduino GND pin to encoder GND (white cable), 
 * - Arduino GND pin to Panasonics PM45 blue cable, 
 * - Arduino GND pin to DotStar GND;
 * - Arduino 5V pin to encoder 5V (red cable), 
 * - Arduino 5V pin to Panasonics PM45 brown cable;
 * - LED 5V power supply GND to DotStar strip GND pin (also the Arduino GND);
 * - LED 5V power supply 5V to DotStar strip 5V pin;
 * - put the cap between power supply GND and LED 5V power supply rails;
 *
 * I'm using an Arduino Micro
 * which has interrupts on pins 2, 3, 7 (also pins 0 and 1 use interrupt but they are dedicated to serial comm)
 * Note: On Arduino Micro pin 2 and 3 are on port D on position 1 and 0 respectively (see decoding snippet)
 *
*/

// 2020-08-27 - discard the debounce objects because we are not using mechanical 
// switch anymore so we don't need to do debounce

#define bToVVVV true

// debug info about the Panasonic optical switches
#define DEBUG_RESET false
// debug info about the encoder, leds, scales and state machine
#define DEBUG_CARRIAGE false


// ENCODER STUFF

// On Arduino Micro digital pins 2 and 3 (interrupt # 1 and 0 respectively)
// are on the same port (which is port D). These pins are on position 1 and 0 of the port
#define CHA 3
#define CHB 2

volatile byte bCurrent = 0;
volatile byte bPrevious = 0;
volatile long integrator = 0;
volatile long prevIntegrator = 0;


volatile enum STATE {
  ENC_IDLE,
  ENC_CW,
  ENC_CCW
} status;

// we know the Bourns  encoder has 100 counts per shaft turn
// but we are incrementing the counter each time we have a new
// CHA and CHB configuration so 4 counts each quadrature cycle.
// This way we have 100 * 4 = 400 counts per shaft turn.
#define TOTALSTEPSPERROTATION 400

// from the moment we use so many different scales
// whose the carriage movement must ne mapped to, we
// must define some MACRO here
#define MAX_MM 100 // travel lenght if the carriage in mm
#define MAX_STEPS 1950 // travel lenght expressed in encoder steps
const int LEFT_POS = TOTALSTEPSPERROTATION;
const int RIGHT_POS = LEFT_POS + MAX_STEPS;
float normPos = 0.0;
int pos100, prevPos100 = 0;

// 2020-02-25
// a new variable to flip the LED direction movement if needed
bool B_FLIP = true;

// RESETS STUFF
bool USE_RESET_L = true;
bool USE_RESET_R = true;
#define RESET_L   6
#define RESET_R   7


typedef struct {
  int pin;
  bool curr = false;
  bool prev = false;
} reset;
reset resetL;
reset resetR; 

// LED STUFF /////////////////////////////////////////////////////////////////////

#include "Adafruit_DotStar.h"
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 144 // TODO: change here according to your number of LEDs in the strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    8 // (green cable)
#define CLOCKPIN   9 // (yellow cable)
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

#define MAXBRIGHTNESS 0.2

// CARRIAGE STUFF
#include "Carriage.h"
Carriage carriage;


// SETUP /////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  
  //while(!Serial) {delay(1);};
  //Serial.println("here I'm");

  // ENCODER STUFF
  pinMode(CHA,   INPUT_PULLUP );
  pinMode(CHB,   INPUT_PULLUP );
  attachInterrupt(digitalPinToInterrupt(CHA),   ISR_chA,   CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHB),   ISR_chB,   CHANGE);

  // RESETs STUFF
  if( USE_RESET_L ) 
  {
    resetL.pin = RESET_L;
    pinMode(resetL.pin, INPUT_PULLUP);
  }
  if( USE_RESET_R ) 
  {
    resetR.pin = RESET_R;
    pinMode(resetR.pin, INPUT_PULLUP);
  }

  // carriage stuff
	carriage.init(&strip, 3);
}


// LOOP //////////////////////////////////////////////////////////////
void loop()
{

  // serial data
  getSerialData();
  
  // RESETs STUFF ****************************************************
  if( USE_RESET_L ) {
    resetL.curr = digitalRead( resetL.pin );
    if( resetL.curr != resetL.prev) {
      resetL.prev = resetL.curr;
      integrator = LEFT_POS;
      if( !resetL.curr ) {
        if( DEBUG_RESET ) Serial.println("Reset L touched");
      } else {
        if( DEBUG_RESET ) Serial.println("Reset L released");
        integrator = LEFT_POS;
      }
    }
  }

  if( USE_RESET_R ) {
    resetR.curr = digitalRead( resetR.pin );
    if( resetR.curr != resetR.prev) {
      resetR.prev = resetR.curr;
      integrator = RIGHT_POS;
      if( !resetR.curr ) {
        if( DEBUG_RESET ) Serial.println("Reset R touched");
      } else {
        if( DEBUG_RESET ) Serial.println("Reset R released");
          integrator = RIGHT_POS;
      }
    }
  }
  
	carriage.update();

	// Here's where the magic happens (so to say)
	if( integrator != prevIntegrator ) 
	{
		// if we are here it means encoder is on a new position
		// It's time to notify the LED manager (carriage)
    normPos = clip((((integrator - TOTALSTEPSPERROTATION)*1.0) / MAX_STEPS ) );
    pos100 = int(normPos * 100);
    
    // update position to the carriage for the LEDs to follow the movement
    if( B_FLIP ) {
		  carriage.setPos( 1.0 - normPos, B_FLIP );
    } else {
      carriage.setPos( normPos, B_FLIP );
    }
    
		// and also to notify VVVV
		if( bToVVVV ) 
		{
      if( pos100 != prevPos100 ) 
      {
        // send value to VVVV only if they are changing
        prevPos100 = pos100;
  			// for the moment I'm sending a value between 0-100
  			// change her if you want to send different normalized value.
  			Serial.println( int(normPos * 100) );
      }
		}
   
		// eventually update the old integrator
		prevIntegrator = integrator;

    if( DEBUG_CARRIAGE ) {
      Serial.print("raw: ");
      Serial.print( integrator );
      carriage.debug();
    }
	}

  delay( 1 );
}

float clip( float toBeClipped ) {
  if( toBeClipped < 0.0 )
    return 0.0;
  else if( toBeClipped >1.0 )
    return 1.0;
  else
    return toBeClipped;
}

// OTHER STUFF ///////////////////////////////////////////////////////
void ISR_chA()
{
  bCurrent = (PIND & B00000011)>>0;
  decode();
}

void ISR_chB()
{
  bCurrent = (PIND & B00000011)>>0;
  decode();
}

/*
 * Questa funzione incrementa la variabile 'volatile'
 * ogni nuovo passaggio tra le quadrature.
 */
void decode()
{
  // encoder stuff
  if( bCurrent != bPrevious )
  {
    if(
        ( bCurrent == 2 && bPrevious == 0 ) ||
        ( bCurrent == 3 && bPrevious == 2 ) ||
        ( bCurrent == 1 && bPrevious == 3 ) ||
        ( bCurrent == 0 && bPrevious == 1 )
      )
    {
      status = ENC_CW;
      integrator ++;
      
    }
    else if(
        ( bCurrent == 1 && bPrevious == 0 ) ||
        ( bCurrent == 3 && bPrevious == 1 ) ||
        ( bCurrent == 2 && bPrevious == 3 ) ||
        ( bCurrent == 0 && bPrevious == 2 )
      )
    {
      status = ENC_CCW;
      integrator --;
    }
    bPrevious = bCurrent;
  }

  // TODO: probably this "normalization will not be used in final application
  // Here we are not working with a rotating cirlce but with a linear movement
  // which is made up with more than a complete rotation of the encoder shaft.

  // Turn a bipolar signal between -400 and 400
  // to a signal which is unipolar going from 0 to 400;
  if( integrator < 0)
    integrator = TOTALSTEPSPERROTATION + integrator;
}


void getSerialData()
{
  if(Serial.available())
  {
    // Read user input and trigger appropriate function
    char user_input = Serial.read();

      if(user_input == 'q' ) 
      {
        // reset left
        integrator = LEFT_POS;
      }
      else if( user_input == 'w' )
      {
        // reset right
        integrator = RIGHT_POS;
      }
      else
      {
        //Serial.println("Invalid option entered.");
      }
  }
}
