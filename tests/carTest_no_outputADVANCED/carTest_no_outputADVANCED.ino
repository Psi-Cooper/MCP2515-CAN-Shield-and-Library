/*03/04/2015
Testing CAN shield and library with car - ford fiesta zetec 09
Counting message transmissions. IDs checked have been pre-identified in an "all-output" test
*/

#include <SPI.h>
#include <MCP2515.h>

MCP2515 canbus(10);

void setup() {                 
  Serial.begin(115200);  //initialise serial connection
  	
  byte retVal;
  canbus.begin();
  
  //Check current operating mode
  retVal = canbus.read(CANSTAT );
  Serial.print("\nCar CANbus test. Current operating mode: ");
  Serial.print(retVal,HEX);
  delay(100);
    
  canbus.setBitRate(500);      //set bit rate at 500kbps
  canbus.RXB0ReceiveValid();    //set RXB0 to accept any valid message meeting filter criteria
  canbus.resetFiltersAndMasks();
  delay(1000);

  if(canbus.setMode(LISTEN)==1){  //ensure device in listen only mode
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



unsigned long checkTime = 10000;  //time to count messages for

void loop() { 
  uint32_t retVal;  //variable to store counted message value
  
  //Count messages transmitted in 10 seconds for all IDs, then each individual ID
  retVal = allIDCount();
  Serial.print("\nCount for all IDs: ");
  Serial.print(retVal);
  
  retVal = IDCount(35);
  Serial.print("\nCount for ID35: ");
  Serial.print(retVal);
  
  retVal = IDCount(128);
  Serial.print("\nCount for ID128: ");
  Serial.print(retVal);
  
  retVal = IDCount(253);
  Serial.print("\nCount for ID253: ");
  Serial.print(retVal);
  
  retVal = IDCount(400);
  Serial.print("\nCount for ID400: ");
  Serial.print(retVal);
  
  retVal = IDCount(512);
  Serial.print("\nCount for ID512: ");
  Serial.print(retVal);
  
  retVal = IDCount(513);
  Serial.print("\nCount for ID513: ");
  Serial.print(retVal);
  
  retVal = IDCount(517);
  Serial.print("\nCount for ID517: ");
  Serial.print(retVal);
  
  retVal = IDCount(526);
  Serial.print("\nCount for ID526: ");
  Serial.print(retVal);
  
  retVal = IDCount(527);
  Serial.print("\nCount for ID527: ");
  Serial.print(retVal);
  
  retVal = IDCount(529);
  Serial.print("\nCount for ID529: ");
  Serial.print(retVal);
  
  retVal = IDCount(530);
  Serial.print("\nCount for ID530: ");
  Serial.print(retVal);
  
  retVal = IDCount(561);
  Serial.print("\nCount for ID561: ");
  Serial.print(retVal);
  
  retVal = IDCount(576);
  Serial.print("\nCount for ID576: ");
  Serial.print(retVal);
  
  retVal = IDCount(629);
  Serial.print("\nCount for ID629: ");
  Serial.print(retVal);
  
  retVal = IDCount(640);
  Serial.print("\nCount for ID640: ");
  Serial.print(retVal);
  
  retVal = IDCount(698);
  Serial.print("\nCount for ID698: ");
  Serial.print(retVal);
  
  retVal = IDCount(864);
  Serial.print("\nCount for ID864: ");
  Serial.print(retVal);
  
  retVal = IDCount(868);
  Serial.print("\nCount for ID868: ");
  Serial.print(retVal);
  
  retVal = IDCount(1024);
  Serial.print("\nCount for ID1024: ");
  Serial.print(retVal);
  
  retVal = IDCount(1029);
  Serial.print("\nCount for ID1029: ");
  Serial.print(retVal);
  
  retVal = IDCount(1056);
  Serial.print("\nCount for ID1056: ");
  Serial.print(retVal);
  
  retVal = IDCount(1060);
  Serial.print("\nCount for ID1060: ");
  Serial.print(retVal);
  
  retVal = IDCount(1064);
  Serial.print("\nCount for ID1064: ");
  Serial.print(retVal);
  
  retVal = IDCount(1072);
  Serial.print("\nCount for ID1072: ");
  Serial.print(retVal);
  
  retVal = IDCount(1075);
  Serial.print("\nCount for ID1075: ");
  Serial.print(retVal);
  
  retVal = IDCount(1120);
  Serial.print("\nCount for ID1120: ");
  Serial.print(retVal);
  
  retVal = IDCount(1200);
  Serial.print("\nCount for ID1200: ");
  Serial.print(retVal);
  
  retVal = IDCount(1251);
  Serial.print("\nCount for ID1251: ");
  Serial.print(retVal);
  
  retVal = IDCount(1265);
  Serial.print("\nCount for ID1265: ");
  Serial.print(retVal);
  
  retVal = IDCount(1266);
  Serial.print("\nCount for ID1266: ");
  Serial.print(retVal);
  
  //after completing - do nothing
  while(1)
  {delay(100);}
}


uint32_t IDCount(uint32_t ID){
  //function for counting messages of a specific ID (filtering)
  canbus.setMode(CONFIGURATION);
    
  //Masking first 11bits (standard ID message)
  canbus.setMaskOrFilter(MASK0, 0xFF, 0xE0,0x00,0x00);
  
  //Code to generate filter byte values from ID
  byte filt1, filt2, filt3, filt4, retVal;
  filt3 =0x00;
  filt4 =0x00;  
  filt2 = (ID&0x007)<<5;
  filt1 = (ID&0x7F8)>>3;    //want top 8 bits of 11 bit ID as all standard 0x000 is 12 bits 0 000 0000 0 000 -> 0x7F8
  canbus.setMaskOrFilter(FILTER0, filt1, filt2, filt3, filt4);
  canbus.setMode(NORMAL);
  
  long startTime = millis();    //current arduino system time
  unsigned long currentTime = startTime;
  int state = 0;
  uint32_t  counter = 0;
  
  while(currentTime-startTime<checkTime){  //while still within 10s counting time
    if (state==0){  //state determines whether first message has been received, timer doesn't start until this happens
    currentTime = millis();
    startTime = currentTime;
    }
    retVal = canbus.RXStatus(); 
    if((retVal&0x40)==0x40){  //Message in RXbuf0. Masked so don't care what type of message received just that in RXbuf0
      state =1;
      currentTime = millis();
      counter++;    
      canbus.resetInterruptBit(0x01); //clears message received status in rxb0    
    } 
  }
return counter;  //return message count value
}

uint32_t allIDCount(){
  canbus.setMode(CONFIGURATION);
  canbus.resetFiltersAndMasks();
  canbus.setMode(NORMAL);
  delay(10);
  byte retVal;
  
  long startTime = millis();
  unsigned long currentTime = startTime;
  int state = 0;
  uint32_t  counter = 0;
  while(currentTime-startTime<checkTime){
    if (state==0){
    currentTime = millis();
    startTime = currentTime;
    }
    retVal = canbus.RXStatus(); 
    if((retVal&0x40)==0x40){  //Message in RXbuf0. Masked so don't care what type of message received just that in RXbuf0
    state =1;
    currentTime = millis();
    counter++;    
    canbus.resetInterruptBit(0x01); //clears message received status in rxb0
    
    } 
  }
return counter;
}
