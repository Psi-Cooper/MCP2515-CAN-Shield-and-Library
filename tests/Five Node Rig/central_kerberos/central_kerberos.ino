/*Node 4 - Central ECU.
Code Name: kerberos
Receives all information to pass on to node 5
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 kerberos(10);
uint32_t kerberosID = 0x01;

void setup(){
  Serial.begin(115200);
  kerberos.begin();
  kerberos.setBitRate(250);
  kerberos.resetFiltersAndMasks();
  kerberos.setFilterStandardID(FILTER0, 0x63);  //let in nyx on RXB0
  kerberos.setFilterStandardID(FILTER1, 0x41);  //let in styx on RXB0
  kerberos.setFilterStandardID(FILTER2, 0xAA);  //let in charon on RXB1
  kerberos.RXB0ReceiveValid();
  kerberos.RXB1ReceiveValid();  
  kerberos.setMode(NORMAL);
}

byte data[] = {0,0,0,0,0,0,0,0};
long currentTime = 0;
long prevTime = 0;
int  interval = 5; //time between message sends

// RX variables
  byte dataRX[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;  
  
void loop(){
  currentTime = millis(); 
  byte messageStatus = 0; //message receive status
  
  messageStatus = kerberos.RXStatus();
  
  if((messageStatus&0x40)==0x40){  //motor speed controllers or driver interface message    
    
    kerberos.readRXBuffer0(&DLCRX, &dataRX[0], &IDRX, &EXRX, &RRX);
    if(IDRX==0x63){  //potentiometer values split
      data[0]=dataRX[0];
      data[1]=dataRX[1];
      data[2]=dataRX[2];
      data[3]=dataRX[3];
    }
    if(IDRX==0x41){  //Counter Button states - not enough registers for individual button states so combined and assigned value
      if(dataRX[0]==1&dataRX[1]==0){data[7]=1;}
      if(dataRX[0]==0&dataRX[1]==1){data[7]=2;}
      if(dataRX[0]==0&dataRX[1]==0){data[7]=0;}
      if(dataRX[0]==1&dataRX[1]==1){data[7]=3;}
    }    
  }
  if((messageStatus&0x80)==0x80){ // output node message
    kerberos.readRXBuffer1(&DLCRX, &dataRX[0], &IDRX, &EXRX, &RRX);
    data[4] = dataRX[0];
    data[5] = dataRX[1];
    data[6] = dataRX[2];    
  }
  if(currentTime-prevTime>interval){
    //transmit consolidated data
    kerberos.loadTXBuffer0(kerberosID, STANDARD, sizeof(data), data);
    kerberos.sendTXBuffer0();
    prevTime = currentTime;
  }

}
