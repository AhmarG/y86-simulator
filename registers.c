#include <stdbool.h>
#include "registers.h"
#include "tools.h"

static unsigned int registers[REGSIZE];
static unsigned int CC;

// Function: getRegister
// Description: returns int from  register 
// Params: int regNum
// Returns: int in register
// Modifies: registers[]

unsigned int getRegister(int regNum)
{
    if(regNum < 0 || regNum > 7)
    {
        return 0;
    }
    return registers[regNum];
}

// Function: setRegister
// Description: adds int to register
// Params: int regNum, unsigned int regValue
// Returns: none
// Modifies: registers[]

void setRegister(int regNum, unsigned int regValue)
{
    if(regNum < 0 || regNum > 7)
    {
        return;
    }
    registers[regNum] = regValue;
}

// Function: clearRegister
// Description: sets all registers to 0
// Modifies: registers[]

void clearRegisters()
{
    clearBuffer((char*)registers, 8);
}

// Function: setCC
// Description: set to CC value 
// Params: int bitNumber, unsigned int value 
// Returns: none
// Modifies: CC

void setCC(unsigned int bitNumber, unsigned int value)
{
    setBits(bitNumber, bitNumber, value);
    if(bitNumber == ZF || bitNumber == SF || bitNumber == OF)
    {
        CC = assignOneBit(bitNumber, value, CC);
    }
}

// Function: getCC 
// Description: gets CC value 
// Params: unsigned int bitNumber - 
// Returns: unsinged int
// Modifies: none

unsigned int getCC(unsigned int bitNumber)
{
    return getBits(bitNumber, bitNumber, CC);
}

/*
 *set CC regiser to zero
 */
void resetCC()
{
    CC = 0;
}
