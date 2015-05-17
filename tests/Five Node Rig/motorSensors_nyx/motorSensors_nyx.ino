/*Node 1 - Motor speed sensors.
Code Name: Nyx
Reads in value from two potentiometers and transmits on to CAN bus
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 nyx(10);
//Potentiometer input analog input pins
int leftWheel  = A0;
int rightWheel = A1;

uint32_t nyxID = 0x63;  //ID for Node

void setup(){
  Serial.begin(115200);
  nyx.begin();
  nyx.setBitRate(250);
  nyx.resetFiltersAndMasks();
  nyx.setMaskOrFilter(MASK0, 0xFF, 0xFF, 0xFF, 0xFF);  //no messages received RXB0
  nyx.setMaskOrFilter(MASK1, 0xFF, 0xFF, 0xFF, 0xFF);  //no messages received RXB1
  nyx.setMode(NORMAL);
}

long currentTime = 0;
long prevTime = 0;
int  interval = 5; //time between message sends

void loop(){
  currentTime = millis();  
  int LS = analogRead(leftWheel);  //Left speed - 10 bit read, need to separate
  int RS = analogRead(rightWheel);
  
  byte leftSpeedA, leftSpeedB, rightSpeedA, rightSpeedB;
  //Splitting up bits
  leftSpeedA = (LS>>8)&0x03;
  leftSpeedB = LS&0xFF;
  rightSpeedA = (RS>>8)&0x03;
  rightSpeedB = RS&0xFF;
  
  //arranging data for transmission
  byte data[] = {leftSpeedA, leftSpeedB, rightSpeedA, rightSpeedB};
  
  if(currentTime-prevTime>interval){
    //transmit
    nyx.loadTXBuffer0(nyxID, STANDARD, sizeof(data), data);
    nyx.sendTXBuffer0();
    prevTime = currentTime;
  }  
}
  
  
  
  
  
