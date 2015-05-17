/*03/04/2015
Testing CAN shield and library with car - ford fiesta zetec 09
Testing diagnostic message transmission and reception using OBDII, J1979, PID standards
*/

#include <SPI.h>
#include <MCP2515.h>

MCP2515 canbus(10);

void setup() {                
  Serial.begin(115200);
  byte retVal;
  canbus.begin();

  retVal = canbus.read(CANSTAT );
  Serial.print("\nCar CANbus test. Current operating mode: ");
  Serial.print(retVal,HEX);
  delay(100);
    
  canbus.setBitRate(500);
  canbus.RXB0ReceiveValid();
  canbus.resetFiltersAndMasks();
  delay(1000);
  //Masking first 11bits for ECU(standard ID message)
  canbus.setMaskOrFilter(MASK0, 0xFF, 0x00,0x00,0x00);
  
  //Filter bytes for ECU IDs
  canbus.setMaskOrFilter(FILTER0, 0xFD, 0x00,0x00,0x00);
  
  if(canbus.setMode(NORMAL)==1){  
  retVal = canbus.read(CANSTAT);
  Serial.print("\nOperating Mode: ");
  Serial.print(retVal,HEX);  
  delay(1000);
  Serial.println("\nDevice now ready for diagnostics"); 
  }
  else{
    Serial.print("\nDevice failed to initialise requested mode");
    while(1){
      delay(100);
    }
  }
  Serial.print(millis()); //Start time for graphing data
}

uint32_t diagID = 0x7DF;
//byte PID = 0x05; //Engine Coolant Temp
byte PID = 0x0C; //Engine RPM

byte dataBytes[] = {2, 0x01, PID, 0x55, 0x55, 0x55, 0x55, 0x55};  //padded with 0x55 (wikipedia)

unsigned long startTime = millis();
unsigned long currentTime = 0;
unsigned long checkTime = 0;
int interval = 1; //time between data checks

void loop() {  
  byte retVal;
  // RX variables
  byte dataBytesRX[]={0,0,0,0,0,0,0,0};
  uint32_t IDRX = 0;
  byte DLCRX= 0;
  byte EXRX=0;
  byte RRX = 0;
  
        
  currentTime = millis();
  if(currentTime - checkTime>interval){  //request data every 1ms
    canbus.loadTXBuffer0(diagID, STANDARD, sizeof(dataBytes), dataBytes);
    canbus.sendTXBuffer0();
  }
  
  
  //continously check for received data and print to monitor with timestamp
  retVal = canbus.RXStatus();
  if((retVal&0x40)==0x40){    
    canbus.readRXBuffer0(&DLCRX, &dataBytesRX[0], &IDRX, &EXRX, &RRX);
    int RPM = (((dataBytesRX[3]<<8)&0xFF00) + (dataBytesRX[4]&0xFF))/4;
    Serial.print("\n");
    Serial.print(RPM);
    Serial.print("\t ");
    Serial.print(currentTime);
  }
 
 
 checkTime = currentTime;  //update check time value for requesting
 
 if(currentTime-startTime>30000){  //when 30seconds passed, end test
   Serial.print("\nTest End");
   while(1){delay(100);}
 }
 
}

