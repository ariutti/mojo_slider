/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include "VL53L0X.h"

VL53L0X sensor;
#define DELAY 5

float value, prevValue=0;
float A=0.2, B;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous();

  B = 1.0 - A;
}

void loop()
{
  float value = sensor.readRangeContinuousMillimeters() / 10.0;

  value = A * value + B * prevValue;
  prevValue = value;
  
  if( value > 100 )
    Serial.print(100.0);
  else
    Serial.print(value);
  
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  Serial.println();
  delay( DELAY );
}
