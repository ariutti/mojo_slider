/*
 * CIRCUIT
 *
 * Component:
 * - 1x Arduino micro;
 * - 1x Bourns optical relative encoder ();
 * - 2x micro switch
 * - 1x Adafruit NeoPixels 60LED/m strip;
 * - 1x LED 5V povwer supply;
 * - 1x 470 ohm resistor;
 * - 1x 1000µF 6.1V electrolitic cap;
 *
 * Connect:
 * - Arduino pin 2 to encoder CHA;
 * - Arduino pin 3 to encoder CHB;
 * - Arduino pin 4 to the left micro switch NO pin;
 * - Arduino pin 5 to the right micro switch NO pin;
 * - Arduino pin 9 to the NeoPixels LED strip DIN pin via a 470 Ohm resistor;
 * - Arduino GND pin to encoder GND, microswitch COM pin, NeoPixels GND;
 * - Arduino 5V pin to encoder 5V;
 * - power supply GND to NeoPixel strip GND pin (also the Arduino GND);
 * - power supply 5V to NeoPixel strip 5V pin;
 * - put the cap among power supply GND and 5V rails;
 *
 *
 * I'm using an Arduino Micro
 * which has interrupts on pins 2, 3, 7
 * Note: On Arduino Micro pin 2 and 3 are on port D on position 1 and 0 respectively (see decoding snippet)
 *
*/

#define DEBUG false
#define bToVVVV true

// ENCODER STUFF

// On Arduino Micro digital pins 2 and 3 (interrupt # 1 and 0 respectively)
// are on the same port (which is port D). These pins are on position 1 and 0 of the port
#define CHA 2
#define CHB 3

volatile byte bCurrent = 0;
volatile byte bPrevious = 0;
volatile long integrator = 0;
long prevIntegrator = 0;
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
#define MAX_STEPS 400*1 // travel lenght expressed in encoder steps



// RESETS STUFF
#define RESET_L   7
#define RESET_R   6


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
      Serial.print("integrator = ");
      Serial.print(integrator);
      Serial.println(";");
    }
  }
}


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
  pinMode(RESET_L, INPUT_PULLUP);
  pinMode(RESET_R, INPUT_PULLUP);

  reset_left.setCallback( reset_left_cb );
  reset_right.setCallback( reset_right_cb );


	carriage.init(&strip, 3);
}


// LOOP //////////////////////////////////////////////////////////////
void loop()
{
  if(DEBUG) {
    Serial.print( integrator );
    Serial.println();
  }

  // RESETs STUFF
  reset_left.update();
  reset_right.update();

	carriage.update();

  if( DEBUG )
    carriage.debug();


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
	}

  delay( 10 );
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
