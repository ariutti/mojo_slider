import processing.serial.*;

Serial myPort;           // the serial port you're using
String myString = null;
String[] list;
int lfcr = 10;
boolean bSerialListen = true;
int distance, strength;


Meter meter;
float meterW = 800.0;
float meterH = 100.0;


// SETUP /////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  size(1024, 300);
  
   // list all the serial ports:
  println(Serial.list());

  // based on the list of serial ports printed from the
  //previous command, change the 0 to your port's number:
  // initialize the serial port:
  myPort = new Serial(this, Serial.list()[1], 9600);
  
  meter = new Meter( (width-meterW)*0.5, (height-meterH)*0.5, meterW, meterH, 30, 120);
  
}

// DRAW //////////////////////////////////////////////////////////////////////////////////////
void draw()
{
  background(0);
  read();
  meter.display();
}

// READ //////////////////////////////////////////////////////////////////////////////////////
void read()
{
  while (myPort.available() > 0) {
    myString = myPort.readStringUntil( lfcr );
    if (myString != null) {
      list = split(myString, '_');
      
      if( bSerialListen )
      {
        //print(myString);
        //printArray(list);
        //println(list.length);
        if(list.length == 2) {
          // si tratta di una stringa contenente i valori numerici da interpretare
          distance = Integer.parseInt( trim(list[0]));
          strength  = Integer.parseInt( trim(list[1]) );
          //println(distance);
          meter.setValue( distance );
          //println(strength);
        }
      }
    }
  }
}
