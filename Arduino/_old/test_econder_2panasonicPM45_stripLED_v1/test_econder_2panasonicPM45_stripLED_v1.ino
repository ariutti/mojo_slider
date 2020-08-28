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
 * - 1x Adafruit NeoPixels 60LED/m strip;
 * - 1x LED 5V power supply;
 * - 1x 470 ohm resistor;
 * - 1x 1000µF 6.1V electrolitic cap;
 *
 * Connect:
 * - Arduino pin 2 to encoder CHA (this rail has a physical 10K pullup resistor - green cable);
 * - Arduino pin 3 to encoder CHB (this rail has a physical 10K pullup resistor - yellow cable);
 * - Arduino pin 6 to the right Panasonic PM45 white cable (no physical pullup resistor)
 * - Arduino pin 7 to the left Panasonic PM45 white cable (no physical pullup resistor);
 * - Arduino pin 9 to the NeoPixels LED strip DIN pin via a 470 Ohm resistor;
 * - Arduino GND pin to encoder GND (white cable), 
 * - Arduino GND pin to Panasonics PM45 blue cable, 
 * - Arduino GND pin to NeoPixels GND;
 * - Arduino 5V pin to encoder 5V (red cable), 
 * - Arduino 5V pin to Panasonics PM45 brown cable;
 * - power supply GND to NeoPixel strip GND pin (also the Arduino GND);
 * - power supply 5V to NeoPixel strip 5V pin;
 * - put the cap between power supply GND and 5V rails;
 *
 * I'm using an Arduino Micro
 * which has interrupts on pins 2, 3, 7 (also pins 0 and 1 use interrupt but they are dedicated to serial comm)
 * Note: On Arduino Micro pin 2 and 3 are on port D on position 1 and 0 respectively (see decoding snippet)
 *
*/

// 2020-08-27 - discard the debounce objects because we are not using mechanical 
// switch anymore so we don't need to do debounce

#define bToVVVV false

// debug info about the Panasonic optical switches
#define DEBUG_RESET true
// debug info about the encoder, leds, scales and state machine
#define DEBUG_CARRIAGE true


bool USE_RESET_L = true;
bool USE_RESET_R = true;

// ENCODER STUFF

// On Arduino Micro digital pins 2 and 3 (interrupt # 1 and 0 respectively)
// are on the same port (which is port D). These pins are on position 1 and 0 of the port
#define CHA 2
#define CHB 3

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
#define MAX_STEPS 1791 // travel lenght expressed in encoder steps


// RESETS STUFF
#define RESET_L   7
#define RESET_R   6


typedef struct {
  int pin;
  bool curr = false;
  bool prev = false;
} reset;

reset resetL;
reset resetR; 


/*
#include "ButtonDebounce.h"
// (ButtonDebounce Library is written and maintained
// by Maykon L. Capellari <maykonluiscapellari@gmail.com>
// https://github.com/maykon/ButtonDebounce
#define DEBOUNCE_TIME 50

ButtonDebounce reset_left(RESET_L, DEBOUNCE_TIME);
ButtonDebounce reset_right(RESET_R, DEBOUNCE_TIME);


void reset_left_cb(int state){
  if( !state )
  {
    integrator = 0; // TODO: change here
    if( DEBUG ) {
      Serial.print("LEFT RESET touched - ");
      Serial.print("integrator = ");
      Serial.print(integrator);
      Serial.println(";");
    }
  }
}

void reset_right_cb(int state){
  if( !state )
  {
    integrator = 1024; // TODO: change here
    if( DEBUG ) {
      Serial.print("RIGHT RESET touched - ");
      Serial.print("integrator = ");
      Serial.print(integrator);
      Serial.println(";");
    }
  }
}
*/

// LED STUFF

#include "Adafruit_NeoPixel.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDPIN    9 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 144 // TODO: change here according to your number of LEDs in the strip

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
#define MAXBRIGHTNESS 0.2


// CARRIAGE STUFF
#include "Carriage.h"
Carriage carriage;


// SETUP /////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  while(!Serial) {delay(1);};
  //Serial.println("here I'm");

  // ENCODER STUFF
  pinMode(CHA,   INPUT_PULLUP );
  pinMode(CHB,   INPUT_PULLUP );
  attachInterrupt(digitalPinToInterrupt(CHA),   ISR_chA,   CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHB),   ISR_chB,   CHANGE);

  // RESETs STUFF
  resetL.pin = RESET_L;
  resetR.pin = RESET_R;
  
  pinMode(resetL.pin, INPUT_PULLUP);
  pinMode(resetR.pin, INPUT_PULLUP);

  //reset_left.setCallback( reset_left_cb );
  //reset_right.setCallback( reset_right_cb );
  
	carriage.init(&strip, 3);
}


// LOOP //////////////////////////////////////////////////////////////
void loop()
{

  // serial data
  getSerialData();

  
  // RESETs STUFF
  //reset_left.update();
  //reset_right.update();

  /*
  if( USE_RESET_L ) {
    resetL.curr = digitalRead( resetL.pin );
    if( resetL.curr != resetL.prev) {
      resetL.prev = resetL.curr;
      integrator = MAX_STEPS;
      if( !resetL.curr ) {
        Serial.println("Reset L touched");
      } else {
        Serial.println("Reset L released");
      }
    }
  }

  if( USE_RESET_R ) {
    resetR.curr = digitalRead( resetR.pin );
    if( resetR.curr != resetR.prev) {
      resetR.prev = resetR.curr;
      integrator = 0;
      if( !resetR.curr ) {
        if( DEBUG_RESET )
          Serial.println("Reset R touched");
      } else {
        if( DEBUG_RESET )
          Serial.println("Reset R released");
      }
    }
  }

  if( !resetL.curr ) {
    integrator = MAX_STEPS;
  }

  if( !resetR.curr ) {
    integrator = 0;
  }
  */

	carriage.update();


	//where the maιc happens (so to say)
	if( integrator != prevIntegrator ) {
		// if we are here it means encoder is on a new position
		// It's time to notify the LED manager (carriage)
		carriage.setPos( (integrator*1.0) / MAX_STEPS );
		// an also to notify VVVV
		if( bToVVVV ) {
			// for the moment I'm sending a value between 0-100
			// change her if you want to send different normalized value.
			Serial.println( (integrator*100.0) / MAX_STEPS );
		}
		// eventually update the old integrator
		prevIntegrator = integrator;

    Serial.print("raw: ");
    Serial.print( integrator );
    carriage.debug();
	}

  delay( 1 );
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
        integrator = 0;
      }
      else if( user_input == 'w' )
      {
        // reset right
        integrator = 2000;
      }
      else
      {
        //Serial.println("Invalid option entered.");
      }
  }
}
