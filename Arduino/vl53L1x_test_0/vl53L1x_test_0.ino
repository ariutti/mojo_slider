#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example prints the distance to an object.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h"

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

#define DELAY 1
#define DEBUG true
#define PRINTDIST true
uint8_t rangeStatus = -1;
long startTime, endTime;


// SETUP ///////////////////////////////////////////////////////////////////////////////////////////////
void setup(void)
{
  Wire.begin();

  Serial.begin(9600);
  Serial.println("\nVL53L1X Test");

  if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor online!");
  }

  initDistanceSensor();
  //distanceSensor.startRanging(); 
}

// LOOP ////////////////////////////////////////////////////////////////////////////////////////////////
void loop(void)
{

  
  startTime = millis();
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.stopRanging();
  endTime = millis();  

  /*
  if (distanceSensor.checkForDataReady())
    Serial.println("data ready");
  else
    Serial.println("not ready");
   */

  /*
  float cms = distance / 10.0;

  if( PRINTDIST ) {
    //Serial.print("Distance(mm): ");
    //Serial.print(distance);
    //Serial.print("Distance(cm): ");
    Serial.print( cms ); Serial.print("\t");
  }

   if(DEBUG) {
    Serial.print("Hz: ");
    Serial.print(1000.0 / (float)(endTime - startTime), 2);
    Serial.println();
  }
  */

  //printSignalRate();
  printRates();  

  /*
  float distanceInches = distance * 0.0393701;
  float distanceFeet = distanceInches / 12.0;

  Serial.print("\tDistance(ft): ");
  Serial.print(distanceFeet, 2);
  */
  delay( DELAY );
}

// INIT DISTANCE SENSOR ////////////////////////////////////////////////////////////////////////////////
void initDistanceSensor(void) 
{
  uint16_t sigTh = distanceSensor.getSignalThreshold(); //Returns the signal threshold in kcps
  uint16_t sigma = distanceSensor.getSigmaThreshold(); //Returns the current sigma threshold.
  Serial.print("Signal Threshold (kcps): "); Serial.print(sigTh); Serial.println();
  Serial.print("Sigma: "); Serial.print(sigma);Serial.println();

  /**Table of Optical Centers**
  *
  * 128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248
  * 129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249
  * 130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250
  * 131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251
  * 132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252
  * 133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253
  * 134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254
  * 135,143,151,159,167,175,183,191,  199,207,215,223,231,239,247,255
  
  * 127,119,111,103, 95, 87, 79, 71,  63, 55, 47, 39, 31, 23, 15, 7
  * 126,118,110,102, 94, 86, 78, 70,  62, 54, 46, 38, 30, 22, 14, 6
  * 125,117,109,101, 93, 85, 77, 69,  61, 53, 45, 37, 29, 21, 13, 5
  * 124,116,108,100, 92, 84, 76, 68,  60, 52, 44, 36, 28, 20, 12, 4
  * 123,115,107, 99, 91, 83, 75, 67,  59, 51, 43, 35, 27, 19, 11, 3
  * 122,114,106, 98, 90, 82, 74, 66,  58, 50, 42, 34, 26, 18, 10, 2
  * 121,113,105, 97, 89, 81, 73, 65,  57, 49, 41, 33, 25, 17, 9, 1
  * 120,112,104, 96, 88, 80, 72, 64,  56, 48, 40, 32, 24, 16, 8, 0 Pin 1
  * 
  */


  /**
   * @brief This function programs the ROI (Region of Interest)\n
   * The ROI position is centered, only the ROI size can be reprogrammed.\n
   * The smallest acceptable ROI size = 4\n
   * @param X:ROI Width; Y=ROI Height
   */
  //  distanceSensor.setROI(uint8_t x, uint8_t y, uint8_t opticalCenter); 
  distanceSensor.setROI(12, 12, 0); //Set the height and width of the ROI(region of interest) in SPADs, lowest possible option is 4. Set optical center based on above table
  uint16_t roiX = distanceSensor.getROIX(); //Returns the width of the ROI in SPADs
  uint16_t roiY = distanceSensor.getROIY(); //Returns the height of the ROI in SPADs
  uint16_t spadNb = distanceSensor.getSpadNb(); //Returns the current number of enabled SPADs
  if(DEBUG) {
    Serial.print("ROI ["); Serial.print(roiX); Serial.print(", "); Serial.print(roiY); Serial.print("]");Serial.println();
    Serial.print("spadNb: "); Serial.print(spadNb);Serial.println();
  }

  //distanceSensor.setDistanceModeLong(); //Set to 4M range
  distanceSensor.setDistanceModeShort(); //Set to 1.3M range
  uint8_t distanceMode = distanceSensor.getDistanceMode(); //Get the distance mode, returns 1 for short and 2 for long
  if(DEBUG) {
    Serial.print("Distance Mode: ");Serial.print(distanceMode);Serial.println();
  }
  /**
   * @brief This function programs the threshold detection mode\n
   * Example:\n
   * VL53L1X_SetDistanceThreshold(dev,100,300,0,1): Below 100 \n
   * VL53L1X_SetDistanceThreshold(dev,100,300,1,1): Above 300 \n
   * VL53L1X_SetDistanceThreshold(dev,100,300,2,1): Out of window \n
   * VL53L1X_SetDistanceThreshold(dev,100,300,3,1): In window \n
   * @param   dev : device address
   * @param   ThreshLow(in mm) : the threshold under which one the device raises an interrupt if Window = 0
   * @param   ThreshHigh(in mm) :  the threshold above which one the device raises an interrupt if Window = 1
   * @param   Window detection mode : 0=below, 1=above, 2=out, 3=in
   * @param   IntOnNoTarget = 1 (No longer used - just use 1)
   */
  //distanceSensor.setDistanceThreshold(uint16_t lowThresh, uint16_t hiThresh, uint8_t window);//Set bounds for the interrupt. lowThresh and hiThresh are the bounds of your interrupt while window decides when the interrupt should fire. The options for window are shown below.
  //0: Interrupt triggered on measured distance below lowThresh.
  //1: Interrupt triggered on measured distance above hiThresh.
  //2: Interrupt triggered on measured distance outside of bounds.
  //3: Interrupt triggered on measured distance inside of bounds.
  uint16_t distanceThWinOpt = distanceSensor.getDistanceThresholdWindow(); //Returns distance threshold window option
  uint16_t distanceThLow = distanceSensor.getDistanceThresholdLow(); //Returns lower bound in mm.
  uint16_t distanceThHig = distanceSensor.getDistanceThresholdHigh(); //Returns upper bound in mm

  if(DEBUG) {
    Serial.print("Distance Th Win: "); Serial.print(distanceThWinOpt); Serial.print("; ");
    Serial.print("Th LOW: "); Serial.print(distanceThLow); Serial.print("; ");
    Serial.print("Th HIGH: "); Serial.print(distanceThHig); Serial.println();
  }

  //void setOffset(int16_t offset); //Manually set an offset in mm
  int16_t offset = distanceSensor.getOffset(); //Get the current offset in mm
  //void setXTalk(uint16_t xTalk); //Manually set the value of crosstalk in counts per second (cps), which is interference from any sort of window in front of your sensor.
  uint16_t xTalk = distanceSensor.getXTalk(); //Returns the current crosstalk value in cps.

  if(DEBUG) {
    Serial.print("offset: "); Serial.print(offset);Serial.println();
    Serial.print("cross talk: ");Serial.print(xTalk);Serial.println();
  }

  // TIMING BUDGET & INTERMEASUREMENT PERIOD

  distanceSensor.setTimingBudgetInMs(100); //Set the timing budget for a measurement (ms)
  uint16_t timingBudget = distanceSensor.getTimingBudgetInMs(); //Get the timing budget for a measurement
  if(DEBUG) {
    Serial.print("Timing budget: "); Serial.print(timingBudget); Serial.println();
  }
  //Intermeasurement period must be >/= timing budget. This condition is not checked by the API,
  //the customer has the duty to check the condition. Default = 100 ms
  distanceSensor.setIntermeasurementPeriod(500);
  uint16_t intermeasurementPeriod = distanceSensor.getIntermeasurementPeriod();
  if(DEBUG) {
    Serial.print("IntermeasurementPeriod: ");Serial.print(intermeasurementPeriod); Serial.println();
  }
}

// PRINT RANGE STATUS //////////////////////////////////////////////////////////////////////////////////
void printRates(void) {

  uint16_t signalPerSPAD = distanceSensor.getSignalPerSpad(); //Returns the average signal rate per SPAD (The sensitive pads that detect light, the VL53L1X has a 16x16 array of these) in kcps/SPAD, or kilo counts per second per SPAD.
  uint16_t ambientPerSPAD = distanceSensor.getAmbientPerSpad(); //Returns the ambient noise when not measuring a signal in kcps/SPAD.
  uint16_t signalRate = distanceSensor.getSignalRate(); //Returns the signal rate in kcps. All SPADs combined.
  uint16_t ambientRate = distanceSensor.getAmbientRate(); // Returns the total ambinet rate in kcps. All SPADs combined.

  Serial.print("SPS: "); Serial.print(signalPerSPAD); Serial.print(";\t");
  Serial.print("APS: "); Serial.print(ambientPerSPAD); Serial.print(";\t");
  Serial.print("SR (kcps): "); Serial.print(signalRate); Serial.print(";\t");
  Serial.print("AR (kcps?): "); Serial.print(ambientRate); Serial.print(";\t");

  /**
   * @brief This function returns the ranging status error \n
   * (0:no error, 1:sigma failed, 2:signal failed, ..., 7:wrap-around)
   */
  uint8_t rangeStatus = distanceSensor.getRangeStatus(); //Returns the range status, which can be any of the following. 0 = no error, 1 = signal fail, 2 = sigma fail, 7 = wrapped target fail

  Serial.print("\tRange Status: ");

  //Make it human readable
  switch (rangeStatus)
  {
    case 0:
      Serial.print("Good");
      break;
    case 1:
      Serial.print("Sigma fail");
      break;
    case 2:
      Serial.print("Signal fail");
      break;
    case 7:
      Serial.print("Wrapped target fail");
      break;
    default:
      Serial.print("Unknown: ");
      Serial.print(rangeStatus);
      break;
  }
  Serial.println();

}
