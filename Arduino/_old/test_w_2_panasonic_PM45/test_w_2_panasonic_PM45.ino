/*
 * CIRCUIT
 *
 * Component:
 * - 1x Arduino micro;
 * - 2x Panasonic PM45 optical switch;
 *
 * Connect:
 * - Arduino pin 6 to the right Panasonic PM45 white cable (no physical pullup resistor)
 * - Arduino pin 7 to the left Panasonic PM45 white cable (no physical pullup resistor);
 * - Arduino GND pin to Panasonics PM45 blue cable;
 * - Arduino 5V pin to Panasonics PM45 brown cable;
 *
*/


// RESETS STUFF
#define RESET_L   7
#define RESET_R   6

bool resetL_cur_value =  false;
bool resetL_prev_value = false;
bool resetR_cur_value =  false;
bool resetR_prev_value = false;

// SETUP /////////////////////////////////////////////////////////////
void setup() 
{
  // SERIAL STUFF
  Serial.begin(9600);
  while( !Serial ) { delay(1); }

  // RESETs STUFF
  pinMode(RESET_L, INPUT_PULLUP);
  pinMode(RESET_R, INPUT_PULLUP);
}

// LOOP //////////////////////////////////////////////////////////////
void loop() 
{
  resetL_cur_value = digitalRead( RESET_L );
  resetR_cur_value = digitalRead( RESET_R );

  if( resetL_cur_value != resetL_prev_value ) {
    resetL_prev_value = resetL_cur_value;
    
    // panasonic sensors white cable (the one we are using) is active LOW
    if( !resetL_cur_value )
      Serial.println("RESET L: touched");
    else
      Serial.println("RESET L: released");
  }

  if( resetR_cur_value != resetR_prev_value ) {
    resetR_prev_value = resetR_cur_value;

    // panasonic sensors white cable (the one we are using) is active LOW
    if( !resetR_cur_value )
      Serial.println("RESET R: touched");
    else
      Serial.println("RESET R: released");
  }

  delay(10);
}
