/*17/03/2015
Test of MCP2515s in normal operation mode to test transmission and reception of data. Test will be performed using
CAN shields. Sending 1 standard frame with one data byte incrementing once per message transmission
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 canBus(10);

void setup() {                
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH); 	
  byte retVal;
  //byte dataBytes1[] = {0xFF,0xEE,0xDD,0x11, 0x10, 0x11};
  //byte dataBytes2[] = {0xCC,0x12,0x34,0x56};
  //uint32_t ID1=123;
  //uint32_t ID2=111234;
  canBus.begin();
  canBus.modify(RXB0CTRL, 0x60, 0xFF);
  retVal = canBus.read(CANSTAT );
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(100);

  canBus.setBitRate(500);  //500kbps transmission
  delay(1000);   
  canBus.setMode(NORMAL); 
  retVal = canBus.read(CANSTAT);
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(1000);
}

void printRXBuffer(uint32_t ID, byte DLC, byte data[], byte ext, byte remote)
{	
	Serial.print("\nMessage ID: ");
	Serial.print(ID);
	Serial.print("\tExtended: ");
	Serial.print(ext);
	Serial.print("\tRemote: ");
	Serial.print(remote);
	Serial.print("\nData: ");
	int i;
  	for(i=0; i<DLC; i++)
  	{
    	Serial.print("\t");
    	Serial.print(data[i],HEX);
  	}	
}

void loop() { 
  byte retVal;
  // RX variables
  byte dataBytesRX[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;
  
  //message reception status
  while(1){
    retVal = canBus.RXStatus();
    if((retVal&0x40)==0x40){  //standard data message received if retVal = 0x40
      canBus.readRXBuffer0(&DLCRX, &dataBytesRX[0], &IDRX, &EXRX, &RRX);
      Serial.print("\n Bus 1 Message Received:");
      printRXBuffer(IDRX, DLCRX, dataBytesRX, EXRX, RRX);

      IDRX = 0;
      DLCRX= 0;
      EXRX=0;
      RRX = 0;
    } 
  }
}

