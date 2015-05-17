void setup(){
Serial.begin(9600);
bool retVal;
byte testVal1 = 0x42;
byte testVal2 = 0x31;

retVal = messageAbortedTX0(testVal1);
Serial.print("Test value 1 should return true: ");
Serial.println(retVal);
retVal = messageAbortedTX0(testVal2);
Serial.print("Test value 2 should return false: ");
Serial.println(retVal);

}

void loop()
{

}


// Modified functions to test bool return works correctly. In library first function takes no input
// and second function takes a register address input. This is just for debugging.
bool messageAbortedTX0(byte input)
{
	bool retVal;
	retVal = messageAborted(input);
	return retVal;
}

bool messageAborted(byte input)
{
	byte retVal;
	retVal = ((input&0x40)==0x40);
	return retVal;
}
