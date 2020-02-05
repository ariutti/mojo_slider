import processing.serial.*;

Serial myPort;           // the serial port you're using
String myString = null;
String[] list;
int lfcr = 10;
int distance, strength; 

Meter meter;
PFont font;

boolean bSerialListen = false;

// SETUP /////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  //size(1024, 300);
  fullScreen();
  
   // list all the serial ports:
  println(Serial.list());

  // based on the list of serial ports printed from the
  //previous command, change the 0 to your port's number:
  // initialize the serial port:
  myPort = new Serial(this, Serial.list()[0], 9600);
  
  meter = new Meter( width, height*0.5, 60, height, 30, 150);
  
  font = loadFont("Monospaced-32.vlw");
  textFont(font, 18);
  
}

// DRAW //////////////////////////////////////////////////////////////////////////////////////
void draw()
{
  background(0);
  if( bSerialListen) read();
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
          meter.setDist( distance );
          meter.setStrenght( strength );
          //println(strength);
        }
      }
    }
  }
}

// KEY PRESSED //////////////////////////////////////////////////////////////////////
void keyPressed()
{
  if (key == 'o' || key == 'O') {
    myPort.write('o');
    bSerialListen = true;
  }
  // press 'c' to close serial communication w/ Arduino
  else if( key == 'c' || key == 'C') {
    myPort.write('c');
    bSerialListen = false;
  }
  // press 'r' to reset all graphs and also send an 'r' 
  // to Arduino in order for it to reset each MPR boards
  else if (key == 'r' || key == 'R') {
    //myPort.write('r');
    meter.reset();
  }
}
