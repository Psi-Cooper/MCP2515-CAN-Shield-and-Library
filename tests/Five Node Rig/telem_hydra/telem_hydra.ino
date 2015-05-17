/*Node 5 - Not-so-Wireless telemetry.
Code Name: Hydra
Receives data and prints to serial monitor
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 hydra(10);
uint32_t hydraID = 0xAA;

void setup(){
  Serial.begin(115200);
  hydra.begin();
  hydra.setBitRate(250);
  hydra.resetFiltersAndMasks();
  hydra.setFilterStandardID(FILTER0, 0x01);  //let in kerberos on RXB0
  hydra.RXB0ReceiveValid();
  hydra.RXB1ReceiveValid();  
  hydra.setMode(NORMAL);
}

long currentTime = 0;
long prevTime = 0;
int  interval = 5; //time between message sends

//initialising variables for all incoming data
int leftSpeed = 0;
int rightSpeed = 0;
int leftMotor = 0;
int rightMotor = 0;
byte counter = 0;
int buttonUp = 0;
int buttonDown = 0;
// RX variables
  byte dataRX[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;  

void loop(){
  currentTime = millis(); 
  byte messageStatus = 0; //message receive status
  

  messageStatus = hydra.RXStatus();
  
  if((messageStatus&0x40)==0x40){  //data incoming        
    hydra.readRXBuffer0(&DLCRX, &dataRX[0], &IDRX, &EXRX, &RRX);
  }
  //remap all data to appropriate variables
  leftSpeed = ((dataRX[0]<<8)&0x300)+(dataRX[1]&0xFF);
  rightSpeed = ((dataRX[2]<<8)&0x300)+(dataRX[3]&0xFF);
  leftMotor = dataRX[4];
  rightMotor = dataRX[5];
  counter = dataRX[6];
  
  switch (dataRX[7]) {
    case 0:
      buttonUp = 0;
      buttonDown = 0;
      break;
    case 1:
      buttonUp = 1;
      buttonDown = 0;
      break;
    case 2:
      buttonUp = 0;
      buttonDown = 1;
      break;
    case 3:
      buttonUp = 1;
      buttonDown = 1;
      break;
  }
  

  if(currentTime-prevTime>interval){
    //print data to serial monitor
    Serial.print("\nControllers: ");
    Serial.print(leftSpeed);
    Serial.print("\t");
    Serial.print(rightSpeed);
    Serial.print("\tMotors: ");
    Serial.print(leftMotor);
    Serial.print("\t");
    Serial.print(rightMotor);
    Serial.print("\tCounter: ");
    Serial.print(counter);
    Serial.print("\tButton States: ");
    Serial.print(buttonUp);
    Serial.print("\t");
    Serial.print(buttonDown);
    prevTime = currentTime;
  }

  
}
