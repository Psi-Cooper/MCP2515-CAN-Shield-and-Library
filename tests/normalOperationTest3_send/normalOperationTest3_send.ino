/*17/03/2015
Test of MCP2515s in normal operation road to test transmission and reception of data. Test will be performed using
two arduino uno controlling one bus node each. One node will transmit while the other checks reception

*/

#include <SPI.h>
#include <MCP2515.h>

MCP2515 canBus(10);

void setup() {                
  Serial.begin(115200);
  byte retVal; 
  canBus.begin();
 
  retVal = canBus.read(CANSTAT );
  Serial.print("\nMode of bus 1: ");
  Serial.print(retVal,HEX);
  delay(100);
    
  canBus.setBitRate(500);  //500kbps transmission
  delay(1000);
    
  canBus.setMode(NORMAL);   //set normal operation and confirm operation mode
  retVal = canBus.read(CANSTAT);
  Serial.print("\nMode of bus 1: ");
  Serial.print(retVal,HEX);  
  delay(1000);  
}

void loop() { 
  int counter = 1;  //counter value for data field
  while(1){  
    byte dataBytes1[] = {0xFF,0xEE,counter};  //data array for message sent, first two value arbitrary
    uint32_t ID1=123;  //id of message to transmit
  
    Serial.println("\nBus 1 message transmission - standard frame:");
    canBus.loadTXBuffer0(ID1, STANDARD, sizeof(dataBytes1), dataBytes1);
    canBus.sendTXBuffer0();
    delay(1000);
    Serial.print("\nMessage Sent\n");  
    counter++;  //increment counter value
}
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
