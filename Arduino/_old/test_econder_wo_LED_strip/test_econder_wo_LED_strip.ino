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
 *
 * Connect:
 * - Arduino pin 2 to encoder CHA (this rail has a physical 10K pullup resistor - green cable);
 * - Arduino pin 3 to encoder CHB (this rail has a physical 10K pullup resistor - yellow cable);
 * - Arduino GND pin to encoder GND (white cable);
 * - Arduino 5V pin to encoder 5V (red cable);
 *
 * I'm using an Arduino Micro
 * which has interrupts on pins 2, 3, 7 (also pins 0 and 1 use interrupt but they are dedicated to serial comm)
 * Note: On Arduino Micro pin 2 and 3 are on port D on position 1 and 0 respectively (see decoding snippet)
 *
*/


// debug info about the encoder, leds, scales and state machine
#define DEBUG_CARRIAGE true

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
#define MAX_STEPS 3000 // travel lenght expressed in encoder steps


// RESET STUFF
bool USE_RESET_L = true;
bool USE_RESET_R = true;

#define RESET_L   7
#define RESET_R   6

typedef struct {
  int pin;
  bool curr = false;
  bool prev = false;
} reset;

reset resetL;
reset resetR;

// TIME DEBUG STUFF
// we want to measure time passed between crossing the resets
long currTime = 0;
long prevTime = 0;

void measureTime(long current, long previous) {
  long elapsed = current - previous;
  Serial.print("curr: ");
  Serial.print( current );
  Serial.print(" - prev: ");
  Serial.print( previous );
  Serial.print(" - elapsed: ");
  Serial.print(elapsed);
  Serial.print(" ms (");
  Serial.print( elapsed / 1000.0 );
  Serial.print(" s)");
  Serial.println();
}


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

  currTime = millis();
  prevTime = millis();
}


// LOOP //////////////////////////////////////////////////////////////
void loop()
{
  // serial data
  getSerialData();

  // here we are using the reset function to measure the time we spend 
  // to move the carriage between the LEFT RESE>T and the RIGHT RESET
  
  if( USE_RESET_L ) {
    resetL.curr = digitalRead( resetL.pin );
    if( resetL.curr != resetL.prev) {
      resetL.prev = resetL.curr;
      
      if( !resetL.curr ) {
        // when we arrive at the sensor
        Serial.println("Reset L touched");
        prevTime = currTime;
        currTime = millis();
        measureTime(currTime, prevTime);
      } else {
        // when we leave the sensor
        Serial.println("Reset L released");
        prevTime = currTime;
        currTime = millis();
        measureTime(currTime, prevTime);
      }
    }
  }

  if( USE_RESET_R ) {
    resetR.curr = digitalRead( resetR.pin );
    if( resetR.curr != resetR.prev) {
      resetR.prev = resetR.curr;

      if( !resetR.curr ) {
        // when we arrive at the sensor
        Serial.println("Reset R touched");
        prevTime = currTime;
        currTime = millis();
        measureTime(currTime, prevTime);
      } else {
        // when we leave at the sensor
        Serial.println("Reset R released");
        prevTime = currTime;
        currTime = millis();
        measureTime(currTime, prevTime);
      }
    }
  }

 
	//where the maιc happens (so to say)
	if( integrator != prevIntegrator ) {
				
		// eventually update the old integrator
		prevIntegrator = integrator;

    Serial.print("raw: ");
    Serial.print( integrator );
    Serial.println();
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

  // TODO: probably this "normalization" will not be used in final application
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
