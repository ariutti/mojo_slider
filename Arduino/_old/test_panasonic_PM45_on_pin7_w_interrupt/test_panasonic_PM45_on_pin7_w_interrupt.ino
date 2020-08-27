#define PANASONIC 7

volatile uint8_t value = 0;


void setup() {
  Serial.begin(9600);
  while( !Serial ) { delay(1); }
  pinMode( PANASONIC, INPUT_PULLUP );
  attachInterrupt(digitalPinToInterrupt(PANASONIC), ISR_reset, CHANGE);
}

void loop() {
  //int value = digitalRead( PANASONIC );
  Serial.println( value );
  delay(10);
}

void ISR_reset() {
  value++;
}
