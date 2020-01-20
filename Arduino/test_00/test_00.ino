#define DELAY 1

//filter stuff
float B, A=0.1;
float mean  = 0.0;
float sigma = 0.0;


//Store distance readings to get rolling average
#define HISTORY_SIZE 10
int history[HISTORY_SIZE];
byte historySpot;

// SETUP ////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  Serial.println("ready");
  B = 1.0 - A;

  for (int x = 0 ; x < HISTORY_SIZE ; x++)
    history[x] = 0;
}

// LOOP /////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{
  
  int value = analogRead(A0);

  //Serial.println(value);

  history[historySpot] = value;
  if (++historySpot == HISTORY_SIZE) historySpot = 0;

  long avgValue = 0;
  for (int x = 0 ; x < HISTORY_SIZE ; x++)
    avgValue += history[x];
  avgValue /= HISTORY_SIZE;
  
  //Serial.print("avgValue: "); Serial.println(avgValue);

  mean = avgValue;
  float accum = 0;
  for (int x = 0 ; x < HISTORY_SIZE ; x++) {
    accum += pow(history[x] - mean, 2);
  }
  sigma = sqrt( accum / HISTORY_SIZE );

  //Serial.print("sigma: "); Serial.println(sigma);
  if( sigma < 3 )
  {
    // there's a touch
    Serial.print("sigma: "); Serial.print(sigma); Serial.print("\tmean: "); Serial.println(mean);
  }
  else
  {
    Serial.print("sigma: "); Serial.println(sigma); 
  }
  
  

  

  
  delay(DELAY);        // delay in between reads for stability
}
