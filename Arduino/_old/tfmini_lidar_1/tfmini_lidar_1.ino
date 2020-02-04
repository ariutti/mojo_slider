// https://github.com/TFmini/TFmini-Arduino

/* 
  This program is the interpretation routine of standard output protocol of TFmini product on Arduino.
  For details, refer to Product Specifications.
  For Arduino boards with only one serial port like UNO board, the function of software virtual serial port is to be used. 
*/ 

#include <SoftwareSerial.h>  //header file of software serial port

SoftwareSerial mySerial(2,3); //define software serial port name as Serial1 and define respetively RX and TX

/* For Arduino boards with multiple serial ports like DUE board, interpret above two pieces of code and directly use Serial1 serial port*/

int dist; //actual distance measurements of LiDAR
int strength; //signal strength of LiDAR
int check;  //save check value
int i;
int uart[9];  //save data measured by LiDAR
const int HEADER=0x59;  //frame header of data package

// SETUP //////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600); //set bit rate of serial port connecting Arduino with computer
  mySerial.begin(115200);  //set bit rate of serial port connecting LiDAR with Arduino
}


// LOOP ///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{ 
  if (mySerial.available()) {  //check if serial port has data input
    if(mySerial.read() == HEADER) {  //assess data package frame header 0x59
      uart[0] = HEADER;
      if (mySerial.read() == HEADER) { //assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) { //save data in array
          uart[i] = mySerial.read();
        }
        
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        
        if ( uart[8] == (check & 0xff)) //verify the received data as per protocol
        { 
          dist = uart[2] + uart[3] * 256;     //calculate distance value
          strength = uart[4] + uart[5] * 256; //calculate signal strength value
          
          Serial.print("dist = ");
          Serial.print(dist); //output measure distance value of LiDAR
          Serial.print('\t');
          Serial.print("strength = ");
          Serial.print(strength); //output signal strength value
          Serial.print('\n');
        }
      }
    }
  }
  
  delay(10);
  //Serial.println("gianni");
  
}

/*
Download the program into Arduino board and power on monitor for serial port. Then real-time distance values measured by LiDAR and corresponding signal strength can be viewed.

In addition, data curve can be viewed in curve plotter for serial port, provided, however, that the above code regarding printing for serial port should be modified:

//  Serial.print("dist = ");
Serial.print(dist);//output measure distance value of LiDAR
Serial.print(' ');
//  Serial.print("strength = ");        
Serial.print(strength);//output signal strength value
Serial.print('\n');

Reinterpret and download the program into Arduino board and power on the curve plotter. Then two curves representing dist and strength can be viewed.
*/
