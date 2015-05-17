/*Node 2 - Output.
Code Name: Charon
Receives motor speeds over CAN and LED count up or down toggles
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 charon(10);
uint32_t charonID = 0xAA;

int lMotor = 6; //left motor pin
int rMotor = 5; //right motor pin
//LED pins
int D1 = 4;  
int D2 = 2;
int D3 = 8;
int D4 = 7;

void setup(){
  Serial.begin(115200);
  charon.begin();
  charon.setBitRate(250);
  charon.resetFiltersAndMasks();
  charon.setFilterStandardID(FILTER0, 0x63);  //let in nyx on RXB0
  charon.setFilterStandardID(FILTER2, 0x41);  //let in styx on RXB1
  charon.RXB0ReceiveValid();
  charon.RXB1ReceiveValid();  
  charon.setMode(NORMAL);
  
  //setup LED pins
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
}

//initialising variables 
int leftSpeed=0;
int rightSpeed=0;
int mapLeft = 0;  //remapped speed for left motor
int mapRight = 0; //remapped speed for right motor

int countUp = 0;
int countDown = 0;
int upPrev = 0;
int downPrev = 0;
byte counter = 0; //LED value

long currentTime = 0;
long prevTime = 0;
int  interval = 5; //time between message sends

void loop(){
  currentTime = millis();
  byte messageStatus = 0; //message receive status
  
  // RX variables
  byte dataRX1[]={0,0,0,0,0,0,0,0};
  byte dataRX2[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;
  
  byte data[] = {mapLeft, mapRight, counter}; //central node can get sensors directly from nyx node
  
  //sending out data
  if(currentTime-prevTime>interval){
    charon.loadTXBuffer0(charonID, STANDARD, sizeof(data), data);
    charon.sendTXBuffer0();
    prevTime = currentTime;
  } 
  
  
  messageStatus = charon.RXStatus();
  
  if((messageStatus&0x40)==0x40){  //motor speed message (Node 1)
    charon.readRXBuffer0(&DLCRX, &dataRX1[0], &IDRX, &EXRX, &RRX);
    leftSpeed = ((dataRX1[0]<<8)&0x300)+(dataRX1[1]&0xFF);  //recombine 2 registers
    rightSpeed = ((dataRX1[2]<<8)&0x300)+(dataRX1[3]&0xFF);  //recombine 2 registers
  }
  
  if((messageStatus&0x80)==0x80){ //message from driver interface  (Node 3)
    charon.readRXBuffer1(&DLCRX, &dataRX2[0], &IDRX, &EXRX, &RRX);
    countUp = dataRX2[0];    //button up state
    countDown = dataRX2[1];  //button down state
  }
  
  //Code for counting up or down counter
  if((upPrev==0)&(countUp==1)){   
    counter++;}
  if((downPrev==0)&(countDown ==1)){
    counter--;}
    
  upPrev = countUp;
  downPrev = countDown;
  Serial.print(counter);
  Serial.print("\n");  
  LEDPattern(counter);  //Output to LEDs
  
  //remap potentiometer 10 bit values to 8 bit
  mapLeft = map(leftSpeed, 0,1023, 0,255);
  mapRight = map(rightSpeed, 0, 1023, 0, 255);
  analogWrite(lMotor, mapLeft);
  analogWrite(rMotor, mapRight);
}

//function for outputting binary value to LEDs
void LEDPattern(byte count){
  int D1Val, D2Val, D3Val, D4Val;
  D1Val = (count&0x01)==0x01;
  D2Val = (count&0x02)==0x02;
  D3Val = (count&0x04)==0x04;
  D4Val = (count&0x08)==0x08;
  digitalWrite(D1, D1Val);
  digitalWrite(D2, D2Val);
  digitalWrite(D3, D3Val);
  digitalWrite(D4, D4Val);
}  
