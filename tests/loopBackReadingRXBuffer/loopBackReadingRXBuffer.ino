/* First loopback test of RX buffer message reception actually reading in the message and data
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 canBus(10);

void setup() {                
  Serial.begin(9600);	
  canBus.begin();    //initialise CAN
  byte retVal;      //var to hold returned values
 
  retVal = canBus.read(CANSTAT );  //should be in config mode
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(100);
  
  canBus.setBitRate(500);  //unecessary as loopback test
  delay(1000);
   
  canBus.setMode(LOOPBACK);  //set to loopback mode
  retVal = canBus.read(CANSTAT);
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(1000);
  
  byte dataBytes1[] = {0xFF,0xEE,0xDD,0x11, 0x10, 0x11};  //random array of data
  uint32_t ID1=532;  //random ID value less that 11bits
  
  // RX variables for received data
  byte dataBytesRX[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;
  
  Serial.println("\nSending Standard Data Frame:");
  canBus.loadTXBuffer0(ID1, STANDARD, sizeof(dataBytes1), dataBytes1);
  canBus.sendTXBuffer0();  //transmit loaded data
  delay(1000);
  Serial.print("\nMessage Sent\n");

  canBus.readRXBuffer0(&DLCRX, &dataBytesRX[0], &IDRX, &EXRX, &RRX);    //load in received message
  
  printRXBuffer(IDRX, DLCRX, dataBytesRX, EXRX, RRX);

}
void printRXBuffer(uint32_t ID, byte DLC, byte data[], byte ext, byte remote)
{
	Serial.println("\nPrinting Message");
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
}

