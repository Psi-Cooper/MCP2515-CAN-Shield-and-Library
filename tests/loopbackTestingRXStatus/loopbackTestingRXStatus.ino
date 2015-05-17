/*Setting MCp2515 in loopback mode, testing transmission and reception of message types and response
of RX status register ie the value it returns representing the message type that was sent
standard/extended/remote
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 canBus(10);
int cs = 10;
void setup() {                 
  Serial.begin(9600); 	
  canBus.begin();
  byte retVal;    
  canBus.setBitRate(500);
  canBus.setMode(LOOPBACK);
  byte dataBytes1[] = {0xFF,0xEE,0xDD,0x11, 0x10, 0x11};
  byte dataBytes2[] = {0xCC,0x12,0x34,0x56};
  uint32_t ID1=1234;
  uint32_t ID2=111234;

  Serial.println("\nSending Extended Remote Frame:");
  canBus.loadTXBuffer0(ID1, EXTENDEDREMOTE, sizeof(dataBytes1), dataBytes1);
  canBus.sendTXBuffer0();
  delay(1000);
  Serial.print("\nMessage Sent\n");
  
  retVal = canBus.RXStatus();
  Serial.print("\nRXSTATUS: ");
  Serial.print(retVal,HEX);
}

void loop() {
}

