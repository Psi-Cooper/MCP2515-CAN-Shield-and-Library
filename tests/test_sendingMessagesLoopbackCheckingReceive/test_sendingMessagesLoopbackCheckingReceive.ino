//This code initialises the MCP2515 in loopback mode then loads a user chosen data frame in to TXBuffer0
//and transmits. The RX buffers are not checked however the interrupt generation register and RX status
//register are checked to check interrupts are correct and that the correct message type is being received
//and in the correct location. Additional tests would check different message types and buffer rollover functions
#include <SPI.h>
#include <MCP2515.h>

MCP2515 bus1(10);

void setup() {                

  
  Serial.begin(9600);
   
  	
  bus1.begin();
  byte retVal;
  
  bus1.modify(CANINTF, 0x01,0x00);
  bus1.modify(CANINTF, 0x02,0x00);

  
  retVal = bus1.read(CANSTAT);
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(100);
  
  bus1.setBitRate(500);
  
  bus1.modify(RXB0CTRL,0x64,0xFF);
  bus1.modify(RXB1CTRL,0x60,0xFF);
  
  bus1.setMode(LOOPBACK);
  retVal = bus1.read(0x0E);
  Serial.print("\nMode: ");
  Serial.print(retVal,HEX);
  delay(1000);
  
  retVal = bus1.RXStatus();
  Serial.print("\nRXSTATUS: ");
  Serial.print(retVal,HEX);
  
  retVal = bus1.read(CANINTF);
  Serial.print("\nCANINTF: ");
  Serial.print(retVal,HEX);
  
  
  
  
  byte dataBytes1[] = {0xFF,0xEE,0xDD,0x11, 0x10, 0x11};
  byte dataBytes2[] = {0xCC,0x12,0x34,0x56};
  uint32_t ID1=1234;
  uint32_t ID2=111234;

  Serial.println("\nSending Standard Remote Frame:");
  //bus1.loadTXBuffer0(ID1, STANDARDREMOTE, sizeof(dataBytes1), dataBytes1);
  bus1.loadTXBuffer0(ID1, STANDARD, sizeof(dataBytes1), dataBytes1);
  bus1.sendTXBuffer0();
  delay(1000);
  Serial.print("\nMessage Sent\n");
  
  //Serial.println("\nSending Extended Frame:");
  //bus1.loadTXBuffer2(ID1, EXTENDED, sizeof(dataBytes1), dataBytes1);
  //bus1.sendTXBuffer2();
  //delay(1000);
  //Serial.print("\nMessage Sent\n");
  
  
  
  
  retVal = bus1.RXStatus();
  Serial.print("\nRXSTATUS: ");
  Serial.print(retVal,HEX);
  
  retVal = bus1.read(CANINTF);
  Serial.print("\nCANINTF: ");
  Serial.print(retVal,HEX);
  
  

}

void loop() {

  
  
  
  
}

