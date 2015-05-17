//Test of basic SPI instructions within library

#include <SPI.h>
#define INT_PIN 3
MCP2515 canbus(10); //declare an instance of the MCP2515 class with slaveselect = pin 10
#define _ss 10;      //no function to setup pins yet so must still be defined here

void setup() {                   
  Serial.begin(9600);  //Serial communication baud rate for printing to serial monitor 	
  // setup pins as outputs and inputs
  //SCK, MISO and MOSI are defined by the arduino board in use
  pinMode (SCK, OUTPUT);
  pinMode (MISO, INPUT);
  pinMode (MOSI, OUTPUT);
  pinMode (_ss, OUTPUT);
	
  //connecting the interrupt of the MCP2515 - not necessary
  pinMode(INT_PIN,INPUT);
  digitalWrite(INT_PIN,HIGH);
  digitalWrite(_ss, HIGH);

  // initialize SPI settings:
  SPI.begin(); 
  SPI.setDataMode(SPI_MODE0);          //use mode 0,0
  SPI.setClockDivider(SPI_CLOCK_DIV4); //4MHz SPI clock used
  SPI.setBitOrder(MSBFIRST);           //transferring most significant bit first
  
  canbus.reset();  //reset command    
}

void loop() {
  // Set CONFIGURATION MODE
  canbus.modify(CANCTRL, 0xE0, 0x80);  //0xE0 is mask, 0x80 is config mode
  readCanStat();
  delay(500);

  // Set LOOPBACK MODE
  canbus.modify(CANCTRL, 0xE0, 0x40);
  readCanStat();
  delay(500);
}

void readCanStat(){
//function for reading the current MCP2515 operating mode
//and the printing to the serial monitor
byte retVal;  //variable to hold current op.mode
retVal = canbus.read(CANSTAT);
Serial.println(retVal, HEX);
}
