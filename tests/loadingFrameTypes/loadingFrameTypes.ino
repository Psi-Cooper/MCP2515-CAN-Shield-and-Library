/* Following test was run with a modified library that printed to the serial monitor in binary
the byte values that would be transmitted to the transmission registers. This was used to confirm that data was
being broken up correctly.
*/

#include <SPI.h>
#include <MCP2515.h>
MCP2515 canBus(10);
void setup() {                
  Serial.begin(9600);
  canBus.begin();
  canBus.setBitRate(500);
  
  delay(100);
  canBus.setMode(NORMAL);
  //random array of data values for testing
  byte dataBytes1[] = {0xFF,0xEE,0xDD,0x11, 0x10, 0x11};
  byte dataBytes2[] = {0xCC,0x12,0x34,0x56};
  
  uint32_t ID1=1234;  //standard ID
  uint32_t ID2=111234;  //value greater than allowed for standard ID

  //Load 4 different frame types with 1 second delay inbetween each.
  Serial.println("Sending Standard Frame:");
  canBus.loadTXBuffer0(ID1, STANDARD, sizeof(dataBytes1), dataBytes1);
  delay(1000);
  Serial.print("\n\n");
  
  Serial.println("Sending Extended Frame:");
  canBus.loadTXBuffer0(ID2, EXTENDED, sizeof(dataBytes2), dataBytes2);
  delay(1000);
  Serial.print("\n\n");
  
  Serial.println("Sending Standard Remote Frame:");
  canBus.loadTXBuffer0(ID1, STANDARDREMOTE, 0, 0);
  delay(1000);
  Serial.print("\n\n");
  
  Serial.println("Sending Extended Remote Frame:");
  canBus.loadTXBuffer0(ID2, EXTENDEDREMOTE, sizeof(dataBytes2), dataBytes2);
  delay(1000);
  Serial.print("\n\n");
}

void loop() {
}

