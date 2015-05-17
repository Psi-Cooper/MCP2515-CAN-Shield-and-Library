/*Node 3 - Driver Interface.
Code Name: Styx
Reads in value from two switches and transmits on to CAN bus
*/
#include <SPI.h>
#include <MCP2515.h>

MCP2515 styx(10);
//variables for button
int countUp  = 0;
int countDown = 1;

uint32_t styxID = 0x41;

void setup(){
  Serial.begin(115200);
  styx.begin();
  styx.setBitRate(250);
  styx.resetFiltersAndMasks();
  styx.setMaskOrFilter(MASK0, 0xFF, 0xFF, 0xFF, 0xFF);  //no messages received RXB0
  styx.setMaskOrFilter(MASK1, 0xFF, 0xFF, 0xFF, 0xFF);  //no messages received RXB1
  styx.setMode(NORMAL);
  
  //setting up input buttons
  pinMode(countUp, INPUT_PULLUP);
  pinMode(countDown, INPUT_PULLUP);
}

long currentTime = 0;
long prevTime = 0;
int  interval = 10; //time between message sends

void loop(){
  currentTime = millis();  
  byte up = digitalRead(countUp);  //button up state
  byte down = digitalRead(countDown);  //button down state
  
  byte data[] = {up, down};
  
  if(currentTime-prevTime>interval){
    //transmit button states
    styx.loadTXBuffer0(styxID, STANDARD, sizeof(data), data);
    styx.sendTXBuffer0();
    prevTime = currentTime;
  }
  
}
 
