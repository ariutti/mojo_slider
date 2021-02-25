#include "Carriage.h"

void Carriage::init( Adafruit_DotStar *_strip, int _halfMask )
{
	strip = _strip;
	HALFMASK = _halfMask;

	// fill the lookup table
	for(int i=0; i<SIDELOBE; i++) {
		int x = i+1;
		lookup[i] = 0.5 * (cos ( (PI*x)/SIDELOBE ) + 1 );
		lookup[i] = lookup[i]*lookup[i];
	}
	nLeds = strip->numPixels();

	// LED STUFF
  strip->begin(); // Initialize NeoPixel strip object (REQUIRED)
  // set every pixel to sleep
  for(uint8_t i=0; i< nLeds; i++) {
    strip->setPixelColor(i, 0x00, 0x00, 0x00);
  }
  strip->show();  // Initialize all pixels to 'off'

	arsine = new Animator_Sine();
  arsine->init(1, 0.0);
}



void Carriage::update()
{
	if( status == STANDBY )
	{
		arsine->update();
    float y = arsine->getY();
		y = y * y;

		lightLEDs( y );

	}
	else if( status == FOLLOW )
	{
		if( millis() - prevTime > TIMETOWAIT) {
			// it means nobady has moved the carriage for a while
			// so we should go into a standby status
			status = STANDBY;
      arsine->reset();
			return;
		}

		lightLEDs( 1.0 );
	}


}


// A function to light up the LEDs which is called both
// in STANDBY and FOLLOW state of the carriage STATE MACHINE.
// It wants only an overall multiplier for the light value.
// This value can be a variable one or a fixed one
// accordine to the wanted light animation.
void Carriage::lightLEDs( float _variableIntensity )
{
	float vi = _variableIntensity;
	byte color;
	for(int i=0; i<nLeds; i++) {
		int distance = abs(i-ledPos);

		if(distance <= HALFMASK ) {
			color = 255.0 * vi * MASKLIGHTLVL;
		}
		else if( distance <= HALFMASK + SIDELOBE ) {
			color = lookup[distance-1-HALFMASK] * 255.0 * vi * MAXLIGHTLVL;
		}
		else {
			color = 0;
		}
		strip->setPixelColor(i, color, color, color);
	}
	strip->show();
}



void Carriage::setPos( float _normalized_pos, bool bFlip )
{
	normalized_pos = _normalized_pos;
  //ledPos = int(normalized_pos*nLeds);

  //21-02-25
  // modifiche in cantiere: sembra che il carrello di luce non sia perfettamente
  // allineato con il carrello fisico, se non al centro della striscia.
  // occorre dunque fare uso di una sorta di offset laterale (magari sinistro e destro indipendenti)
  // grazie al quale sia possibile correggere questo comportamento.
  if( bFlip ) 
  {
    ledPos = LED_OFFSET_R + int(normalized_pos*(nLeds - LED_OFFSET_L - LED_OFFSET_R) );
  }
  else
  {
    ledPos = LED_OFFSET_L + int(normalized_pos*(nLeds - LED_OFFSET_L - LED_OFFSET_R) );
  }

  prevTime = millis();
	if( status == STANDBY ) {
		status = FOLLOW;
	}
	
}

void Carriage::debug() 
{
  Serial.print(",\tnormalized Pos:  ");
  Serial.print( normalized_pos );
  
  Serial.print(",\tLED pos: ");
  Serial.print( ledPos );

  // status info
  Serial.print(",\tSTATUS: ");
  if(status == 0)
    Serial.print("STANDBY");
  else if(status == 1)
    Serial.print("FOLLOW");
  
  // end of debug print
  Serial.println();
}
