/*03/04/2015
Testing CAN shield and library with car - ford fiesta zetec 09

*/

#include <SPI.h>
#include <MCP2515.h>

MCP2515 canbus(9);
void setup() {                
 
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);
  	
  byte retVal;
  canbus.begin();
  
  //removed as added new filter functions to lib
  //canbus.modify(RXB0CTRL, 0x60, 0xFF); //let buffer receive any and all messages detected
  
  
  
  retVal = canbus.read(CANSTAT );
  Serial.print("\nCar CANbus test. Current operating mode: ");
  Serial.print(retVal,HEX);
  delay(100);
    
  canbus.setBitRate(250);
  canbus.RXB0ReceiveValid();
  canbus.resetFiltersAndMasks();
  delay(1000);
  //Masking first 11bits (standard ID message)
  canbus.setMaskOrFilter(MASK0, 0x00, 0x00,0x00,0x00);

  
  //debugging ID filter generation code
  //Serial.println("\n");
  //Serial.println(filt1,BIN);
  //Serial.println(filt2,BIN);
  
  
    
  //setMode function modified to give integer return. A returned 1 means device was sucessfully initialised
  //in chosen mode, a zero means failure to initialise. This is being used here to ensure no testing is
  //run on car unless device is in LISTEN only mode. This means no data can be transmitted on to the cars
  //bus. Failure to initialise puts the device in an infinite loop of doing nothing for safety
  if(canbus.setMode(LISTEN)==1){  
  retVal = canbus.read(CANSTAT);
  Serial.print("\nOperating Mode: ");
  Serial.print(retVal,HEX);  
  delay(1000);
  Serial.println("\nDevice now ready for monitoring"); 
  }
  else{
    Serial.print("\nDevice failed to initialise in listen only mode");
    while(1){
      delay(100);
    }
  }
}

void printRXBuffer(uint32_t ID, byte DLC, byte data[], byte ext, byte remote)
{	
	Serial.print("\nMessage ID: ");
	Serial.print(ID);
	Serial.print("\t\tExtended: ");
	Serial.print(ext);
	Serial.print("\tRemote: ");
	Serial.print(remote);
	Serial.print("\tData: ");
	int i;
  	for(i=0; i<DLC; i++)
  	{
    	Serial.print("\t[");
    	Serial.print(data[i],HEX);
        Serial.print("]");
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
  retVal = canbus.RXStatus();
  //Serial.print("\nRXSTATUS bus 1: ");
  //Serial.println(retVal,HEX);
 
  if((retVal&0x40)==0x40){
  canbus.readRXBuffer0(&DLCRX, &dataBytesRX[0], &IDRX, &EXRX, &RRX);
  //Serial.print("\n Bus 1 Message Received:");
  printRXBuffer(IDRX, DLCRX, dataBytesRX, EXRX, RRX);

  //dataBytesRX={0,0,0,0,0,0,0,0};
  IDRX = 0;
  DLCRX= 0;
  EXRX=0;
  RRX = 0;
  } 
  }
}

