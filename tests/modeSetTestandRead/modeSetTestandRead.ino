#include <SPI.h>
#include <MCP2515.h>
MCP2515 canbus(10);  //declare instance of the MCP2515 class

void setup() {                
  Serial.begin(9600);
  canbus.begin();      //initialise SPI parameters and pin modes
}

void loop() {
  byte retVal;  //variable to store current operating mode
  
  delay(100);
  canbus.setMode(CONFIGURATION);  //set configuration mode
  retVal = canbus.read(0x0E);    //read current operating mode
  Serial.println(retVal,HEX);    //print to serial monitor register value of current mode
  
  delay(100);
  canbus.setMode(NORMAL);
  retVal = canbus.read(0x0E);
  Serial.println(retVal,HEX);
  
  delay(100);
  canbus.setMode(LISTEN);
  retVal = canbus.read(0x0E);
  Serial.println(retVal,HEX);
  
  delay(100);
  canbus.setMode(SLEEP);
  retVal = canbus.read(0x0E);
  Serial.println(retVal,HEX);
  
  delay(100);
  canbus.setMode(LOOPBACK);
  retVal = canbus.read(0x0E);
  Serial.println(retVal,HEX);

}

