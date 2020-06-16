#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "Animator_Sine.h"

class Carriage
{
  private:

		float normalized_pos;

		// LED STUFF
		int nLeds;
		// the position of the carriage LEDS relative (between 0 and nLeds)
		int ledPos;

		// must define sidelobe dimension first
		// this is a monolateral dimension.
		// SIDELOBE is
		const int SIDELOBE = 4;
		// nust define also the number of LEDs which are masked below
		// the carriage (it is counterproductive to light up them).
		// Actually, as for SIDELOBE, we are interested in getting
		// half of the masked size.
		// HALFMASK is the number of LED which are aside from the LED pos
		// you don't want to shine at the same brightness of SIDEBOLEs
		int HALFMASK;
		float MASKLIGHTLVL = 0.25;
		float MAXLIGHTLVL  = 1.0;
		// then we declare an array that will work as a lookup table
		// for the squared-raised-cosine figure.
		float* lookup = new float[SIDELOBE];
		Adafruit_NeoPixel *strip;
		Animator_Sine *arsine;

		// STATE MACHINE STUFF
		enum statuses{
		  STANDBY=0,
		  FOLLOW,
		} status=STANDBY;
		long prevTime;
		long TIMETOWAIT = 5000; // time to wait before moving from FOLLEW to STANDBY


  public:
    Carriage() {};
    ~Carriage() {};

    void init( Adafruit_NeoPixel *strip, int halfMask );
    void setPos( float _normalized_pos );
		void update();
		void debug();
		void lightLEDs( float variableintensity );
};

#endif
