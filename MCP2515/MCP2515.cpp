#include <SPI.h>
#include "MCP2515.h"

/* 
Future work - change some functions to integer return to allow for failure cases
*/

MCP2515::MCP2515(int ss){
	_ss = ss;
}

/******************
	Initialise CAN controller and perform software reset
*/
void MCP2515::begin(){
	// setup pins
	pinMode (SCK, OUTPUT);
	pinMode (MISO, INPUT);
	pinMode (MOSI, OUTPUT);
	pinMode (_ss, OUTPUT);
	pinMode(RESET_PIN,OUTPUT);
	pinMode(INT_PIN,INPUT);
	digitalWrite(INT_PIN,HIGH);
	digitalWrite(_ss, HIGH);

	// initialize SPI settings:
	SPI.begin(); 
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);
	
	//reset(); //software reset
	//digitalWrite(RESET_PIN, HIGH);
	//delay(10);
	//digitalWrite(RESET_PIN, LOW);
	//delay(100);
	//digitalWrite(RESET_PIN, HIGH);
	//delay(100);
	reset();
}

/******************
	Set Bit Rate of MCP2515
*/
void MCP2515::setBitRate(int bitRate){
	byte config1, config2, config3;

	switch(bitRate)
	{
		case 5:
			config1 = CNF1_5; //values defined in header file
			config2 = CNF2_5;
			config3 = CNF3_5;
			break;
		case 10:
			config1 = CNF1_10;
			config2 = CNF2_10;
			config3 = CNF3_10;
			break;
		case 20:
			config1 = CNF1_20;
			config2 = CNF2_20;
			config3 = CNF3_20;
			break;
		case 50:
			config1 = CNF1_50;
			config2 = CNF2_50;
			config3 = CNF3_50;
			break;
		case 100:
			config1 = CNF1_100;
			config2 = CNF2_100;
			config3 = CNF3_100;
			break;
		case 125:
			config1 = CNF1_125;
			config2 = CNF2_125;
			config3 = CNF3_125;
			break;
		case 250:
			config1 = CNF1_250;
			config2 = CNF2_250;
			config3 = CNF3_250;
			break;
		case 500:
			config1 = CNF1_500;
			config2 = CNF2_500;
			config3 = CNF3_500;
			break;
		case 1000:
			config1 = CNF1_1000;
			config2 = CNF2_1000;
			config3 = CNF3_1000;					
	}

	write(CNF1, config1);
	write(CNF2, config2);
	write(CNF3, config3);
}
/******************
	Set MCP2515 Operating Mode
*/
int MCP2515::setMode(MCP2515Mode mode){
	// Look at adding confirmation
	int retVal =0;
	byte mask = 0xE0;
	byte modeWrite;
	byte modeCurrent;

	switch(mode)
	{
		case CONFIGURATION:
						modeWrite = 0x80;
						break;
		case NORMAL:
						modeWrite = 0x00;
						break;
		case LISTEN:
		// Note: device will get stuck in listen mode if no bus activity. Can be changed by creating wake interupt, see page 57
						modeWrite = 0x60;
						break;
		case SLEEP:
						modeWrite = 0x20;
						break;
		case LOOPBACK:
						modeWrite = 0x40;
						break;
	}
	
	// check current operating mode of device to ensure it has changed
	modify(CANCTRL, mask, modeWrite);
	delay(100); // give device time to change
	modeCurrent = read(CANSTAT) & 0xE0;
	if (modeCurrent == modeWrite)
	{
		retVal = 1;
	}
	return retVal;
}

/******************
	Transfer Buffer Load Functions
*/
void MCP2515::loadTXBuffer0(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[])
{
	loadTXBuffer(ID, LOADTX0ID, frameType, dataLength, dataBytes);
}
void MCP2515::loadTXBuffer1(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[])
{
	loadTXBuffer(ID, LOADTX1ID, frameType, dataLength, dataBytes);
}
void MCP2515::loadTXBuffer2(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[])
{
	loadTXBuffer(ID, LOADTX2ID, frameType, dataLength, dataBytes);
}
void MCP2515::loadTXBuffer(uint32_t ID, byte command, messageType frameType, int dataLength, byte dataBytes[]){
/* Notes: standard frame ID is 11 bits leading to max int value of 2047
	Extended frame ID is 29 bits leading to max int value of 536870911
	Going to want to call this function within library so dont need used to pass address
	have loadTXBuffer0 etc
	Up to user to ensure ID is not truncated*/
	//Serial.print("\n");
	//Serial.print("ID: ");
	//Serial.println(ID,BIN);
	byte IDSIDL, SIDH, SIDL, EID8, EID0, DLC, EID1716;
	if (dataLength>8) dataLength = 8;
	switch(frameType)
	{
		case STANDARD:
						IDSIDL = (ID & 0x07);    //first 3 bits of register2

						SIDH = (ID >> 3) & 0xFF; //register1						
						SIDL = (IDSIDL << 5) & 0xE0;
						EID8 = 0x00;
						EID0 = 0x00;
						DLC  = 0x0F & dataLength;
						break;
		case EXTENDED:
						IDSIDL = ((ID>>18) & 0x07);    //first 3 bits of register2
						EID1716= ((ID>>16) & 0x03);

						SIDH = (ID >> 21) & 0xFF; //register1						
						SIDL = ((IDSIDL << 5) & 0xE0)+8 +EID1716;
						EID8 = (ID>>8) & 0xFF;
						EID0 = ID & 0xFF;
						DLC  = 0x0F & dataLength;
						break;	
		case STANDARDREMOTE:
						IDSIDL = (ID & 0x07);    //first 3 bits of register2

						SIDH = (ID >> 3) & 0xFF; //register1						
						SIDL = (IDSIDL << 5) & 0xE0;
						EID8 = 0x00;
						EID0 = 0x00;
						DLC  = 0x40;			//No data in remote frame, just set RTR bit
						break;	
		case EXTENDEDREMOTE:
						IDSIDL = ((ID>>18) & 0x07);    //first 3 bits of register2
						EID1716= ((ID>>16) & 0x03);

						SIDH = (ID >> 21) & 0xFF; //register1						
						SIDL = ((IDSIDL << 5) & 0xE0)+8 +EID1716;
						EID8 = (ID>>8) & 0xFF;
						EID0 = ID & 0xFF;
						DLC  = 0x40;
						break;												
	}
	byte IDBytes[] = { SIDH, SIDL, EID8, EID0, DLC};
	
	if (DLC ==0x40)
	{
		multiWrite(command, IDBytes, 0, 0);
	}
	else
	{
		multiWrite(command, IDBytes, dataBytes, dataLength);
	}
}

/******************
	Transfer Buffer Request To Send
*/
void MCP2515::sendTXBuffer0()
{
	sendTXBuffer(RTSTXB0);
}
void MCP2515::sendTXBuffer1()
{
	sendTXBuffer(RTSTXB1);
}
void MCP2515::sendTXBuffer2()
{
	sendTXBuffer(RTSTXB2);
}
void MCP2515::sendTXBuffer(byte RTSCOMMAND)
{
	digitalWrite(_ss, LOW);
	SPI.transfer(RTSCOMMAND);
	digitalWrite(_ss,HIGH);
}

/******************
	Receive buffer read functions
*/
void MCP2515::readRXBuffer0(byte *DLC, byte *data, uint32_t *ID, byte* ext, byte* remote){
	readRXBuffer(READRX0ID, DLC, data, ID, ext, remote);
}
void MCP2515::readRXBuffer1(byte *DLC, byte *data, uint32_t *ID, byte* ext, byte* remote){
	readRXBuffer(READRX1ID, DLC, data, ID, ext, remote);
}
void MCP2515::readRXBuffer(byte command, byte *DLC, byte *data, uint32_t *ID, byte* ext, byte* remote)
{
	byte RXDLC;
	uint32_t SIDH, SIDL, EID8, EID0;
	int i;

	digitalWrite(_ss, LOW);
	SPI.transfer(command);
	SIDH = SPI.transfer(0xFF);
	SIDL = SPI.transfer(0xFF);
	EID8 = SPI.transfer(0xFF);
	EID0 = SPI.transfer(0xFF);
	RXDLC = SPI.transfer(0xFF); 

	if ((SIDL & 0x08)==0x08) //extended ID message received
	{
		*ext = 1; //set to show extended message
		if ((RXDLC & 0x40)==0x40) //remote message received
		{
			*DLC = 0;			//no data received as extended		
			*remote = 1;		//set to show message remote	
		}
		else
		{
			*DLC = RXDLC & 0x0F;
			*remote = 0;
			for(i=0; i<*DLC; i++)
			{
				data[i] = SPI.transfer(0xFF);
			}
		}
		//Join extended ID bits
		*ID = ((SIDH<<21)&0x1FE00000)+
			  ((SIDL<<13)&0x001C0000)+
			  ((SIDL<<16)&0x00030000)+
			  ((EID8<<8) &0x0000FF00)+
			  ((EID0)    &0x000000FF);
	}
	else //standard ID message
	{		
		*ext = 0;
		if((SIDL & 0x10)==0x10)	//standard remote message received
		{
			*DLC = 0;
			*remote = 1;
		}
		else
		{
			*DLC = RXDLC & 0x0F;
			*remote = 0;
			for(i=0; i<*DLC; i++)
			{
				data[i] = SPI.transfer(0xFF);
			}
		}
		
		*ID = ((SIDH<<3)&0x7F8)+((SIDL>>5)&0x07); // Join Standard ID bits	
	}
	digitalWrite(_ss,HIGH);
}


/******************
	Message Transmission Features - Datasheet section 3
*/

// Message Aborted Flag Bit
	// 1 = Message was aborted
	// 0 = Message transmitted successfully
bool MCP2515::ABTFTX0(){
	bool retVal;
	retVal = ABTF(TXB0CTRL);
	return retVal;
}
bool MCP2515::ABTFTX1(){
	bool retVal;
	retVal = ABTF(TXB1CTRL);
	return retVal;
}
bool MCP2515::ABTFTX2(){
	bool retVal;
	retVal = ABTF(TXB2CTRL);
	return retVal;
}

bool MCP2515::ABTF(byte address){
	byte retVal;
	retVal = ((read(address)&0x40)==0x40);
	return retVal;
}

// Message lost arbitration bit
	// 1 = Message lost arbitration
	// 0 = Message did not lose arbitration
bool MCP2515::MLOATX0(){
	bool retVal;
	retVal = MLOA(TXB0CTRL);
	return retVal;
}
bool MCP2515::MLOATX1(){
	bool retVal;
	retVal = MLOA(TXB1CTRL);
	return retVal;
}
bool MCP2515::MLOATX2(){
	bool retVal;
	retVal = MLOA(TXB2CTRL);
	return retVal;
}

bool MCP2515::MLOA(byte address){
	byte retVal;
	retVal = ((read(address)&0x20)==0x20);
	return retVal;
}

// Transmission Error Detected Bit
	// 1 = Bus error occurred during transmission
	// 0 = No bus error occurred during transmission
bool MCP2515::TXERRTX0(){
	bool retVal;
	retVal = TXERR(TXB0CTRL);
	return retVal;
}
bool MCP2515::TXERRTX1(){
	bool retVal;
	retVal = TXERR(TXB1CTRL);
	return retVal;
}
bool MCP2515::TXERRTX2(){
	bool retVal;
	retVal = TXERR(TXB2CTRL);
	return retVal;
}

bool MCP2515::TXERR(byte address){
	byte retVal;
	retVal = ((read(address)&0x10)==0x10);
	return retVal;
}

// Message Transmit Request Bit
	// 1 = Buffer pending transmission - cleared when sent
	// 0 = Not pending transmission
bool MCP2515::TXREQTX0(){
	bool retVal;
	retVal = TXREQ(TXB0CTRL);
	return retVal;
}
bool MCP2515::TXREQTX1(){
	bool retVal;
	retVal = TXREQ(TXB1CTRL);
	return retVal;
}
bool MCP2515::TXREQTX2(){
	bool retVal;
	retVal = TXREQ(TXB2CTRL);
	return retVal;
}
bool MCP2515::TXREQ(byte address){
	byte retVal;
	retVal = ((read(address)&0x08)==0x08);
	return retVal;
}

// Abort Message Transmission
void MCP2515::abortTX0(){
	modify(TXB0CTRL, 0x08, 0x00);
}
void MCP2515::abortTX1(){
	modify(TXB1CTRL, 0x08, 0x00);
}
void MCP2515::abortTX2(){
	modify(TXB2CTRL, 0x08, 0x00);
}

// Set Transmit Buffer Priority - must be done in configuration mode
	//By default all the same and higher buffer numbers win priority
void MCP2515::setPriorityTX0(transmitPrio prio){
	setPriority(TXB0CTRL, prio);
}
void MCP2515::setPriorityTX1(transmitPrio prio){
	setPriority(TXB1CTRL, prio);
}
void MCP2515::setPriorityTX2(transmitPrio prio){
	setPriority(TXB2CTRL, prio);
}
void MCP2515::setPriority(byte address, transmitPrio prio){
	byte setVal = 0x00;
	
	switch(prio)
	{
		case HIGHEST:
			setVal = 0x03;
			break;
		case HIGHER:
			setVal = 0x02;
			break;
		case LOWER:
			setVal = 0x01;
			break;
		case LOWEST:
			setVal = 0x00;
			break;
	}
	modify(address, 0x03, setVal);
}


//Set RTS pin modes (input or RTS on falling edge)
//Set as digital inputs
void MCP2515::setB0RTSPinInput(){
	modify(TXRTSCTRL, 0x01, 0x00);
}
void MCP2515::setB1RTSPinInput(){
	modify(TXRTSCTRL, 0x02, 0x00);
}
void MCP2515::setB2RTSPinInput(){
	modify(TXRTSCTRL, 0x04, 0x00);
}
void MCP2515::setAllRTSPinsInput(){
	setB0RTSPinInput();
	setB1RTSPinInput();
	setB2RTSPinInput();
}
//Set as RTS Pins
void MCP2515::setB0RTSPinRTS(){
	modify(TXRTSCTRL, 0x01, 0x0F);
}
void MCP2515::setB1RTSPinRTS(){
	modify(TXRTSCTRL, 0x02, 0x0F);
}
void MCP2515::setB2RTSPinRTS(){
	modify(TXRTSCTRL, 0x04, 0x0F);
}
void MCP2515::setAllRTSPinsRTS(){
	setB0RTSPinRTS();
	setB1RTSPinRTS();
	setB2RTSPinRTS();
}
//Read RTS pin values - only valid in digital input mode
bool MCP2515::readB0RTS(){
	byte retVal;
	retVal = (read(TXRTSCTRL)&0x08)==0x08;
	return retVal;
}
bool MCP2515::readB1RTS(){
	byte retVal;
	retVal = (read(TXRTSCTRL)&0x10)==0x10;
	return retVal;
}
bool MCP2515::readB2RTS(){
	byte retVal;
	retVal = (read(TXRTSCTRL)&0x20)==0x20;
	return retVal;
}
///******************
//	Message Reception Features - Datasheet section 4
//
//	UNTESTED UNTESTED UNTESTED UNTESTED UNTESTED
//*/
//
void MCP2515::RXB0ReceiveAny(){
	//Allow reception of any messages regardless of filters to RX Buffer 0
	modify(RXB0CTRL, 0x60, 0xFF);
}
void MCP2515::RXB0ReceiveExt(){
	//Allow reception of extended messages that fit filter criteria of RX Buffer 0
	modify(RXB0CTRL, 0x60, 0x40);
}
void MCP2515::RXB0ReceiveStd(){
	//Allow reception of standard messages that fit filter criteria of RX Buffer 0
	modify(RXB0CTRL, 0x60, 0x20);
}
void MCP2515::RXB0ReceiveValid(){
	//Allow reception of any messages matching filter criteria of RX Buffer 0
	//DEFAULT SETUP
	modify(RXB0CTRL, 0x60, 0x00);
}

void MCP2515::RXB1ReceiveAny(){
	//Allow reception of any messages regardless of filters to RX Buffer 0
	modify(RXB1CTRL, 0x60, 0xFF);
}
void MCP2515::RXB1ReceiveExt(){
	//Allow reception of extended messages that fit filter criteria of RX Buffer 0
	modify(RXB1CTRL, 0x60, 0x40);
}
void MCP2515::RXB1ReceiveStd(){
	//Allow reception of standard messages that fit filter criteria of RX Buffer 0
	modify(RXB1CTRL, 0x60, 0x20);
}
void MCP2515::RXB1ReceiveValid(){
	//Allow reception of any messages matching filter criteria of RX Buffer 0
	//DEFAULT SETUP
	modify(RXB1CTRL, 0x60, 0x00);
}


bool MCP2515::receivedRTRRXB0(){
	byte retVal;
	retVal = read(RXB0CTRL);
	bool RTR;
	RTR = ((retVal >>3)&0x01)==1;
	return RTR;
}

bool MCP2515::receivedRTRRXB1(){
	byte retVal;
	retVal = read(RXB1CTRL);
	bool RTR;
	RTR = ((retVal >>3)&0x01)==1;
	return RTR;
}

void MCP2515::enableRXRollover(){
	//Enable rollover of messages from RXB0 to RXB1 if full and criteria met
	modify(RXB0CTRL, 0x04, 0xFF);
}
void MCP2515::disableRXRollover(){
	//Disable rollover of messages from RXB0 to RXB1 if full and criteria met
	modify(RXB0CTRL, 0x04, 0x00);
}

byte MCP2515::RXB0FilterHit(){
	//Returns filter that accepted message
	byte retVal;
	retVal = read(RXB0CTRL)&0x01;
	return retVal;
}
byte MCP2515::RXB1FilterHit(){
	//Returns filter that accepted message
	byte retVal;
	retVal = read(RXB1CTRL)&0x07;
	return retVal;
}

void MCP2515::RX0PinModeOutput(){
	//Pin set as digital output
	modify(BFPCTRL, 0x01, 0x00);
}
void MCP2515::RX1PinModeOutput(){
	//Pin set as digital output
	modify(BFPCTRL, 0x02, 0x00);
}
void MCP2515::RXPinsModeOutput(){
	//Pins set as digital output
	RX0PinModeOutput();
	RX1PinModeOutput();
}

void MCP2515::RX0PinModeInt(){
	//Pin set as interrupt on TX buffer full
	modify(BFPCTRL, 0x01, 0xFF);
}
void MCP2515::RX1PinModeInt(){
	//Pin set as interrupt on TX buffer full
	modify(BFPCTRL, 0x02, 0xFF);
}
void MCP2515::RXPinsModeInt(){
	RX0PinModeInt();
	RX1PinModeInt();
}
//
void MCP2515::RXB0Enable(){
	//RX0 Pin function enabled
	modify(BFPCTRL, 0x04, 0xFF);
}
void MCP2515::RXB1Enable(){
	//RX1 Pin function enabled
	modify(BFPCTRL, 0x08, 0xFF);
}
void MCP2515::RXPinsEnable(){
	//Enable RX Pin functions
	RXB0Enable();
	RXB1Enable();
}

void MCP2515::RXB0Disable(){
	//RX0 Pin function enabled
	modify(BFPCTRL, 0x04, 0x00);
}
void MCP2515::RXB1Disable(){
	//RX1 Pin function enabled
	modify(BFPCTRL, 0x08, 0x00);
}
void MCP2515::RXPinsDisable(){
	//Disable RX Pin functions
	RXB0Disable();
	RXB1Disable();
}

void MCP2515::RXB0PinHigh(){
	//set RXB0 pin high - only valid when configured as output
	modify(BFPCTRL, 0x10, 0xFF);
}
void MCP2515::RXB0PinLow(){
	//set RXB0 pin low- only valid when configured as output
	modify(BFPCTRL, 0x10, 0x00);
}
void MCP2515::RXB1PinHigh(){
	//set RXB1 pin high - only valid when configured as output
	modify(BFPCTRL, 0x20, 0xFF);
}
void MCP2515::RXB1PinLow(){
	//set RXB1 pin low- only valid when configured as output
	modify(BFPCTRL, 0x20, 0x00);
}
//
//
///******************
//	MASKS AND FILTERS
//*/
void MCP2515::setMaskOrFilter(byte address, byte b0, byte b1, byte b2, byte b3){
	//only works in configuration mode, up to user to put device in correct mode
	write(address, b0);
	write(address+1,b1);
	write(address+2,b2);
	write(address+3,b3);
}
void MCP2515::resetFiltersAndMasks(){
	setMaskOrFilter(FILTER0, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(FILTER1, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(FILTER2, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(FILTER3, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(FILTER4, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(FILTER5, 0x00,0x00,0x00,0x00);
	
	setMaskOrFilter(MASK0, 0x00,0x00,0x00,0x00);
	setMaskOrFilter(MASK1, 0x00,0x00,0x00,0x00);
}
void MCP2515::setFilterStandardID(byte address, uint32_t ID){
	//Developed for easier car testing, can set filter to standard ID value
	//Masking first 11bits (standard ID message)
  	setMaskOrFilter(MASK0, 0xFF, 0xE0,0x00,0x00);
  	setMaskOrFilter(MASK1, 0xFF, 0xE0,0x00,0x00);
  	byte filt1, filt2, filt3, filt4;
  	filt3 =0x00;
  	filt4 =0x00;
  	filt2 = (ID&0x007)<<5;
  	filt1 = (ID&0x7F8)>>3;
  	setMaskOrFilter(address, filt1, filt2, filt3, filt4);
}

/******************
	Bit Timing
*/
void MCP2515::SAMConfig(bool config){
	//Device must been in configuration mode
	if (config)
	{
		//Bus sampled 3 times at sample point
		modify(CNF2, 0x40, 0xFF);	
	}
	else
	{
		//Bus sampled single time at sample point
		modify(CNF2, 0x40, 0x00);
	}
}

void MCP2515::setSOFBit(bool config){
	if (config)
	{
		//CLKOUT pin enabled for SOF signal
		modify(CNF3, 0x80, 0xFF);
	}
	else
	{
		//CLKOUT pin enabled for clockout function
		modify(CNF3, 0x80, 0x00);
	}
} 

//NO IDEA WHAT THIS DOES - could restrict wake up to only work when filter check passed 
void MCP2515::wakeUpFilterEnable(){
	modify(CNF3, 0x40, 0xFF);
}
void MCP2515::wakeUpFilterDisable(){
	modify(CNF3, 0x40, 0x00);
}

/******************
	Interrupts
*/
void MCP2515::MERREInt(bool config){
	// Interrupt on error during message reception or transmission
	if (config)
	{modify(CANINTE, 0x80, 0xFF);}
	else {modify(CANINTE, 0x80, 0x00);}
}
void MCP2515::WAKIEInt(bool config){
	// Interrupt on CAN bus activity
	if (config)
	{modify(CANINTE, 0x40, 0xFF);}
	else {modify(CANINTE, 0x40, 0x00);}
}
void MCP2515::ERRIEInt(bool config){
	// Interrupt on EFLG error condition change
	if (config)
	{modify(CANINTE, 0x20, 0xFF);}
	else {modify(CANINTE, 0x20, 0x00);}
}
void MCP2515::TX2IEInt(bool config){
	// Interrupt on TXB2 becoming empty
	if (config)
	{modify(CANINTE, 0x10, 0xFF);}
	else {modify(CANINTE, 0x10, 0x00);}
}
void MCP2515::TX1IEInt(bool config){
	// Interrupt on TXB1 becoming empty
	if (config)
	{modify(CANINTE, 0x08, 0xFF);}
	else {modify(CANINTE, 0x08, 0x00);}
}
void MCP2515::TX0IEInt(bool config){
	// Interrupt on TXB0 becoming empty
	if (config)
	{modify(CANINTE, 0x04, 0xFF);}
	else {modify(CANINTE, 0x04, 0x00);}
}
void MCP2515::RX1IEInt(bool config){
	// Interrupt when message received in RXB1
	if (config)
	{modify(CANINTE, 0x02, 0xFF);}
	else {modify(CANINTE, 0x02, 0x00);}
}
void MCP2515::RX0IEInt(bool config){
	// Interrupt when message received in RXB0
	if (config)
	{modify(CANINTE, 0x01, 0xFF);}
	else {modify(CANINTE, 0x01, 0x00);}
}

byte MCP2515::readInterruptFlags(){
	byte retVal;
	retVal = read(CANINTF);
	return retVal;
}

void MCP2515::clearMessageReception(){
	modify(CANINTF, 0x03, 0x00);
}
void MCP2515::resetInterruptBit(byte mask){
	//reset interrupts using mask - page 51 of datasheet
	modify(CANINTF, mask, 0x00);
}
/******************
Error functions
*/
byte MCP2515::readTEC(){
	byte retVal;
	retVal = read(TEC);
	return retVal;
}
byte MCP2515::readREC(){
	byte retVal;
	retVal = read(REC);
	return retVal;
}
byte MCP2515::readEFLG(){
	byte retVal;
	retVal = read(EFLG);
	return retVal;
}
void MCP2515::resetRX1OVRInt(){
	modify(EFLG, 0x80, 0x00);
}
void MCP2515::resetRX0OVRInt(){
	modify(EFLG, 0x40, 0x00);
}
/*****************
Configuration Mode Functions
 requires device in config mode to work
*/
void MCP2515::ABATBit(bool config){
	//If 1 - abort all pending transmit buffers
	//If 0 - Terminate request to abort all pending transmit buffers
	if (config)
	{modify(CANCTRL, 0x10, 0xFF);}
	else {modify(CANCTRL, 0x10, 0x00);}
}
void MCP2515::OSMEnable(){
	//Message will only attempt transmission one time
	modify(CANCTRL, 0x80, 0xFF);
}
void MCP2515::OSMDisable(){
	//Message will reattempt transmission if required
	modify(CANCTRL, 0x80, 0x00);
}
void MCP2515::CLKOUTEnable(){
	//Clock out pin enabled
	modify(CANCTRL, 0x04, 0xFF);
}
void MCP2515::CLKOUTDisable(){
	//Clock out pin disabled
	modify(CANCTRL, 0x04, 0x00);
}
void MCP2515::CLKOUTPrescalar(int scalar){
	switch(scalar) {
		case 1:
				modify(CANCTRL, 0x03, 0x00);
				break;
		case 2:
				modify(CANCTRL, 0x03, 0x01);
				break;
		case 4:
				modify(CANCTRL, 0x03, 0x02);
				break;
		case 8:
				modify(CANCTRL, 0x03, 0x03);
				break;
		default:
				break;
	}
}
byte MCP2515::readICOD(){
	//returns 3 bit value of ICOD - interrupt flag code bits
	byte retVal;
	retVal = (read(CANSTAT)&0x0E)>>1;
	return retVal;
}




byte MCP2515::RXStatus(){
	byte retVal;
	digitalWrite(_ss, LOW);
	SPI.transfer(RXSTATUS);
	retVal = SPI.transfer(0xFF);	
	digitalWrite(_ss, HIGH);
	return retVal;
}

/******************
	Software reset over SPI of MCP2515
*/
void MCP2515::reset(){
	digitalWrite(_ss, LOW);
	//delay(10);
	SPI.transfer(RESET);	
	//delay(10);
	digitalWrite(_ss, HIGH);
	//delay(10);
}

/******************
	Read from address
*/
byte MCP2515::read(byte address){
	byte retVal;
	digitalWrite(_ss, LOW);
	//delay(10);
	SPI.transfer(READ);
	SPI.transfer(address);
	retVal = SPI.transfer(0xFF);
	//delay(10);
	digitalWrite(_ss,HIGH);
	return retVal;
}

/******************
	Write to address
*/
void MCP2515::write(byte address, byte data){
	digitalWrite(_ss, LOW);
	//delay(10);
	SPI.transfer(WRITE);
	SPI.transfer(address);
	SPI.transfer(data);
	//delay(10);
	digitalWrite(_ss,HIGH);
	//delay(1);
}

/******************
	Multiple writes to address - used for loading transfer buffers
*/
void MCP2515::multiWrite(byte instruction, byte IDBytes[], byte dataBytes[], int dataSize){
	int i;
	digitalWrite(_ss, LOW);
	//delay(1);
	SPI.transfer(instruction);
	//Serial.print("Sending ID and DLC: ");
	for (i=0; i<5; i++)
	{		
		//Serial.print(IDBytes[i],BIN);
		//Serial.print("\t");
		SPI.transfer(IDBytes[i]);
	}
	//Serial.print("\n");
	//Serial.print("Sending Data: ");
	if (dataSize >8) dataSize = 8;	//Limit data to maximum of 8 bytes, prevents user breaking.
	for (i = 0; i<dataSize; i++)
	{
		//Serial.print(dataBytes[i], HEX); // debug line
		//Serial.print("\t");
		SPI.transfer(dataBytes[i]);
	}
	//delay(1);
	digitalWrite(_ss,HIGH);
	//delay(1);
}

/******************
	Bitmodify at address
*/
void MCP2515::modify(byte address, byte mask, byte data){
	byte retVal;
	digitalWrite(_ss, LOW);
	//delay(10);
	SPI.transfer(BITMODIFY);
	SPI.transfer(address);
	SPI.transfer(mask);
	SPI.transfer(data);
	//delay(10);
	digitalWrite(_ss,HIGH);
}		