#ifndef MCP2515_h
#define MCP2515_h

#include <Arduino.h>
#include <SPI.h>

/*
Created by: Simon Cooper
University of Bath
Integrated Mechanical and Electrical Engineering Final Year Project
Designed to include the full functionality of the MCP2515 CAN controller

Designed to be used with the CAN shield designed by Simon Cooper
using a 16MHz oscillator. (20MHz quartz crystal support to be added in near future)
*/
#define RESET_PIN 2
#define INT_PIN 3

/*******************
	Register Map
*******************/

//Support bit modify SPI command
#define BFPCTRL 	0x0C		//RXnBF Control and Status
#define TXRTSCTRL 	0x0D		//TXnRTS PIN CONTROL AND STATUS REGISTER - Request to send bits for buffers
#define CANSTAT		0x0E		//CAN status register
#define CANCTRL		0x0F		//CAN control register - clock out and clock prescalers
#define CNF3		0x28		//Bit timing configuration register 3
#define CNF2		0x29		//Bit timing configuration register 2
#define CNF1		0x2A		//Bit timing configuration register 1
#define CANINTE		0x2B		//Interrupt enables
#define CANINTF		0x2C		//Interrupt flags
#define EFLG		0x2D		//Error flags
#define TXB0CTRL	0x30		//Transmit buffer 0 control register
#define TXB1CTRL	0x40		//Transmit buffer 1 control register
#define TXB2CTRL	0x50		//Transmit buffer 2 control register
#define RXB0CTRL	0x60		//Receive  buffer 0 control register
#define RXB1CTRL	0x70		//Receive  buffer 1 control register

/********************************
*	Mask and filter Registers   *
********************************/
//Note: Filter/mask ID registers- 4 registers for SIDH SIDL EID8 EID0 starting at SIDH

//Filters: 
//0,1 - RX buffer 0
//2,3,4,5 - RX buffer 1
#define FILTER0 	0x00		//Receive filter 0 standard ID high
#define FILTER1		0x04		//Receive filter 1 standard ID high
#define FILTER2		0x08		//Receive filter 2 standard ID high
#define FILTER3		0x10		//Receive filter 3 standard ID high
#define FILTER4		0x14		//Receive filter 4 standard ID high
#define FILTER5		0x18		//Receive filter 5 standard ID high

//Masks:
//0 - RX buffer 0
//1 - RX buffer 1
#define MASK0		0x20		//Receive mask 0 standard ID high
#define MASK1		0x24		//Receive mask 1 standard ID high

/********************************
*	TX and RX buffer Registers  *
********************************/
//Note: ID registers - 4 registers for SIDH SIDL EID8 EID0 starting at SIDH followed by DLC register and 8 data registers
#define TXB0SIDH	0x31		//Transfer buffer 0 standard ID high
#define TXB1SIDH	0x41		//Transfer buffer 1 standard ID high
#define TXB2SIDH	0x51		//Transfer buffer 2 standard ID high
#define RXB0SIDH	0x61		//Receive  buffer 0 standard ID high
#define RXB1SIDH	0x71		//Receive  buffer 1 standard ID high
#define TXB0DLC		0x35		//Transfer buffer 0 data length code
#define TXB1DLC		0x45		//Transfer buffer 1 data length code
#define TXB2DLC		0x55		//Transfer buffer 2 data length code
#define RXB0DLC		0x65		//Receive  buffer 0 data length code
#define RXB1DLC		0x75		//Receive  buffer 1 data length code

/*******************
	SPI Commands
*******************/
#define RESET		0xC0		// Resets internal registers and sets configuration mode
#define READ		0x03		// Read data from register beginning at selected address
#define WRITE		0x02		// Write data to  register beginning at selected address
#define READSTATUS  0xA0		// Reads several status bits for TX and RX functions
#define RXSTATUS 	0xB0		// Indicates filter match and message type (standard/extended/remote) of received message
#define BITMODIFY	0x05		// Allows modifying of individual bits in supporting registers: instruction - address - mask - data

#define READRX0ID	0x90		// Read from RXB0SIDH (Note: associate RX flag bit is cleared after CS goes high) 	
#define READRX0D0	0x92		// Read from RXB0D0 (first data byte)
#define READRX1ID	0x94		// Read from RXB1SIDH
#define READRX1D0	0x96		// Read from RXB1D0

#define LOADTX0ID	0x40		// Load from TXB0SIDH
#define LOADTX0D0	0x41		// Load from TXB0D0
#define LOADTX1ID	0x42		// Load from TXB1SIDH
#define LOADTX1D0	0x43		// Load from TXB1D0
#define LOADTX2ID	0x44		// Load from TXB2SIDH
#define LOADTX2D0	0x45		// Load from TXB2D0

#define RTSTXB0		0x81		// Request to send TX buffer 0
#define RTSTXB1		0x82		// Request to send TX buffer 1
#define RTSTXB2		0x84		// Request to send TX buffer 2 		- Incorrect in previous library

/**********************
	Error Counters
**********************/
#define TEC 		0x1C		//TEC register
#define REC 		0x1D 		//REC Register

/**********************
	Baud Rate Config
**********************/
/***
Baud rate configurations. Bit time has to be between 8 and 25 TQ. Old library used 16TQ to achieve this. Will need to alter the following
values for 20MHz resonator when I get.
***/	

// Configured for 16MHz oscillator
// SOF = 0
// SAM = 0
// WAKFIL = 0
// 5 kbps - added
#define CNF1_5 		0x3F
#define CNF2_5 		0xBF
#define CNF3_5 		0x07
// 10 kbps
#define CNF1_10 	0x31
#define CNF2_10		0xB8
#define CNF3_10		0x05
// 20 kbps
#define CNF1_20 	0x18
#define CNF2_20		0xB8
#define CNF3_20		0x05
// 50 kbps
#define CNF1_50 	0x09
#define CNF2_50		0xB8
#define CNF3_50		0x05
// 100 kbps
#define CNF1_100 	0x04
#define CNF2_100	0xB8
#define CNF3_100	0x05
// 125 kbps
#define CNF1_125 	0x03
#define CNF2_125	0xB8
#define CNF3_125	0x05
// 250 kbps
#define CNF1_250 	0x01
#define CNF2_250	0xB8
#define CNF3_250	0x05
// 500 kbps
#define CNF1_500 	0x00
#define CNF2_500	0xB8
#define CNF3_500	0x05

// 800 kbps - added
// Cannot be supported

// 1000 kbps - Bit time = 8TQ
#define CNF1_1000 	0x80 //SJW = 3TQ BRP = 0
#define CNF2_1000	0x90 //
#define CNF3_1000	0x02


enum MCP2515Mode {CONFIGURATION, NORMAL, LISTEN, SLEEP, LOOPBACK};
enum messageType {STANDARD, EXTENDED, STANDARDREMOTE, EXTENDEDREMOTE};
enum transmitPrio {HIGHEST, HIGHER, LOWER, LOWEST};

class MCP2515
{
private:
	int _ss;	// spi slave select pin

public:
	MCP2515(int ss); //input slave select pin - allows multiple buses


	void begin();	//set up controller
	
	void setBitRate(int bitRate);	//set device bit rate
	
	int setMode(MCP2515Mode mode);	//set device mode
	
	//Functions for loading transmission buffers
	void loadTXBuffer(uint32_t ID, byte command, messageType frameType, int DLC, byte dataBytes[]);
	void loadTXBuffer0(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[]);
	void loadTXBuffer1(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[]);
	void loadTXBuffer2(uint32_t ID, messageType frameType, int dataLength, byte dataBytes[]);

	//functions for transmitting loaded buffers
	void sendTXBuffer0();
	void sendTXBuffer1();
	void sendTXBuffer2();
	void sendTXBuffer(byte RTSCOMMAND);

	//functions for reading receive buffers
	void readRXBuffer(byte command, byte *DLC, byte *data, uint32_t* ID, byte* ext, byte* remote);
	void readRXBuffer0(byte *DLC, byte *data, uint32_t *ID, byte* ext, byte* remote);
	void readRXBuffer1(byte *DLC, byte *data, uint32_t *ID, byte* ext, byte* remote);

	byte RXStatus();	//RXStatus command, returns information of messages received
	void reset();		//reset controller - perform on startup
	byte read(byte address);	//SPI read command
	void write(byte address, byte data);	//SPI write command
	void multiWrite(byte address, byte IDBytes[], byte dataBytes[], int DLC);	//Function for writing to TX Buffers
	void modify(byte address, byte mask, byte data);	//SPI modify command

	//Message Reception Buffers - change what type of messages each RX buffer can receive
	void RXB0ReceiveAny();
	void RXB0ReceiveExt();
	void RXB0ReceiveStd();
	void RXB0ReceiveValid();
	void RXB1ReceiveAny();
	void RXB1ReceiveExt();
	void RXB1ReceiveStd();
	void RXB1ReceiveValid();

	//Change function of RX pins on MCP2515
	void RX0PinModeOutput();
	void RX1PinModeOutput();
	void RXPinsModeOutput();
	void RX0PinModeInt();
	void RX1PinModeInt();
	void RXPinsModeInt();

	//Enable the RX pins on MCP2515
	void RXB0Enable();
	void RXB1Enable();
	void RXPinsEnable();

	//Disable the RX pins on MCP2515
	void RXB0Disable();
	void RXB1Disable();
	void RXPinsDisable();

	//Set RX pins high or Low if used as digital outputs
	void RXB0PinHigh();
	void RXB0PinLow();
	void RXB1PinHigh();
	void RXB1PinLow();

	//Masks and filters
	void setMaskOrFilter(byte address, byte b0, byte b1, byte b2, byte b3);
	void resetFiltersAndMasks();
	void setFilterStandardID(byte address, uint32_t ID);
	
	void clearMessageReception();	//clear message reception state

	//Flag for checking whether TXbuffer message was aborted(1) or transmitted(0)
	bool ABTFTX0();
	bool ABTFTX1();
	bool ABTFTX2();
	bool ABTF(byte address);

	//Flag for checking whether TXbuffer message lost arbitration(1) or won(0)
	bool MLOATX0();
	bool MLOATX1();
	bool MLOATX2();
	bool MLOA(byte address);

	//Flag for checking whether bus error occured during transmission of buffer(1) or no error(0)
	bool TXERRTX0();
	bool TXERRTX1();
	bool TXERRTX2();
	bool TXERR(byte address);

	//Flag for checking whether TXBuffer is pending transmission(1) or not pending(0)
	bool TXREQTX0();
	bool TXREQTX1();
	bool TXREQTX2();
	bool TXREQ(byte address);

	//Abort message transmission on TXBuffers
	void abortTX0();
	void abortTX1();
	void abortTX2();

	//Set priority of TX buffers - determines transmission order {HIGHEST, HIGH, LOW, LOWEST};
	void setPriorityTX0(transmitPrio prio);
	void setPriorityTX1(transmitPrio prio);
	void setPriorityTX2(transmitPrio prio);	
	void setPriority(byte address, transmitPrio prio);

	//Set TXRTS pins on MCP2515 as digital Inputs
	void setB0RTSPinInput();
	void setB1RTSPinInput();
	void setB2RTSPinInput();
	void setAllRTSPinsInput();

	//Set TXRTS pins on MCP2515 as request to send pins - message send on falling edge
	void setB0RTSPinRTS();
	void setB1RTSPinRTS();
	void setB2RTSPinRTS();
	void setAllRTSPinsRTS();

	//Read value input on TXRTS pins of MCP2515 - only valid when set up as digital inputs
	bool readB0RTS();
	bool readB1RTS();
	bool readB2RTS();

	//Flag check to see if remote message received in RX Buffers
	bool receivedRTRRXB0();
	bool receivedRTRRXB1();

	//Enable or disable messages hitting RXB0 criteria to rollover to RXB1 if RXB0 full
	void enableRXRollover();
	void disableRXRollover();	

	//Returns which filter allowed a message through- can be used to quickly check for specific ID if configured
	byte RXB0FilterHit();
	byte RXB1FilterHit();

	//Set how many times CAN lines sampled at sampling point - 1 for three times, 0 for just once (default)
	void SAMConfig(bool config);

	//Set whether clockout pin of mcp2515 is for a SOF (start of frame) signal (1), or used as a clock out (0)
	void setSOFBit(bool config);

	//Enable or disable the wakeup filter - sets up a low pass filter function so that device doesn't wake up when short glitches on CAN bus occurs
	void wakeUpFilterEnable();
	void wakeUpFilterDisable();

	//Enable(1) or disable(0) the MCP2515 from generating interrupts in following conditions
	void MERREInt(bool config);		//error during message reception or transmission
	void WAKIEInt(bool config);		//CAN Bus activity
	void ERRIEInt(bool config);		//EFLG error condition change (error flags)
	void TX2IEInt(bool config);		//TX2 Buffer emptying
	void TX1IEInt(bool config);		//TX1 Buffer emptying
	void TX0IEInt(bool config);		//TX0 Buffer emptying
	void RX1IEInt(bool config);		//RX1 Buffer filling
	void RX0IEInt(bool config);		//RX0 Buffer filling
	
	//Read state of interrupt flags - returns whole CANINTF register - see datasheet for details
	byte readInterruptFlags();
	//Reset Interrupts using mask
	void resetInterruptBit(byte mask);

	//Error functions
	byte readTEC();			//Read Transmission error count
	byte readREC();			//Read Reception error count
	byte readEFLG();		//Read error flags register
	void resetRX1OVRInt();	//Reset RX1 buffer overflow error
	void resetRX0OVRInt();	//Reset RX0 buffer overflow error

	//Configuration Mode Functions - device must be in CONFIG mode to run
	//(1)-abort all pending transmissions, (0)-terminate abort request
	void ABATBit(bool config);	

	//Enable or Disable one shot mode - gives messages one change to transmit (off by default)	
	void OSMEnable();				
	void OSMDisable();

	//Enable or Disable the Clock out pin on MCP2515
	void CLKOUTEnable();
	void CLKOUTDisable();
	
	//Change the prescaler value of the clock out pin - options of [1 2 4 8]
	void CLKOUTPrescalar(int scalar);

	//Read the ICOD register - interrupt flag code bits. See datasheet for more information
	byte readICOD();

};

#endif