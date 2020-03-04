bool bSerialWorking = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Hey, I'm here!");
  if( bSerialWorking )
    Serial.println("...and Serial Event is working fine");
  delay(1000);
}

/************************************************************************************
 * SERIAL EVENT
 ***********************************************************************************/
void serialEvent()
{
  byte c = Serial.read();
  // Processing can ask Arduino to send some raw
  // data in order to get a plotted version of them
  if (c == 'o') {
    bSerialWorking = true;

  }
  else if (c == 'c') {
    bSerialWorking = false;
  }
  else if (c == 'r')
  {
    // do nothing
  }
}
