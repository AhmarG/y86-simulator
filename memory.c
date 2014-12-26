#include <stdbool.h>
#include "memory.h"
#include "tools.h"

//prototypes 
static unsigned int fetch(int address, bool * memError);
static void store(int address, unsigned int value, bool * memError);

static unsigned int memory[MEMSIZE];

// Function: fetch 
// Description: pulls address from memory[]
// Params: int address- address of word in memeory
//         bool * memError- flag for valid address 
// Returns: memory[address]
// Modifies: none
//
unsigned int fetch(int address, bool * memError)
{
    if(address >= MEMSIZE || address < 0)
    {
        (*memError) = true;
        return 0;
    }
    (*memError) = false;
    return memory[address];
}

// Function: store
// Description: stores value in memory[] 
// Params: int address - address of word in memory 
//         unsinged int value - value to be stored 
//         bool * memError - flag for valid address
// Returns: none
// Modifies: memory[]

void store(int address, unsigned int value, bool * memError)
{
    if(address >= MEMSIZE || address < 0)
    {
        (*memError) = true;
        return;
    }
    (*memError) = false;
    memory[address] = value;
}

// Function: getByte 
// Description: pulls single byte from memory[]
// Params: int byteAddress -  address of byte in memory
//         bool * memError - flag for valid address
// Returns: insigned char, byte
// Modifies: none

unsigned char getByte(int byteAddress, bool * memError)
{
   unsigned  int x = fetch(byteAddress/4, memError);
    return getByteNumber(byteAddress%4, x);
}

// Function: putByte
// Description: puts single byte in memory[] at byteAddress
// Params: int byteAddress - address to add byte
//         unsigned int char - value to be added at address
//         bool * memError - flag for valid address
// Returns: none
// Modifies: memory[]

void putByte(int byteAddress, unsigned char value, bool * memError)
{
   if(byteAddress < 0 || byteAddress > 4095)
   {
       (*memError) = true;
       return;
   }
   unsigned int x = fetch(byteAddress/4, memError);
   unsigned int y = putByteNumber(byteAddress%4, value, x);
   store(byteAddress/4, y, memError);
}

// Function: getWord
// Description: get single word from memeory[]
// Params: int byteAddress - address to pull word from memeory 
//         bool * memError - flag for valid address
// Returns: unsigned int, word
// Modifies: none 

unsigned int getWord(int byteAddress, bool * memError)
{
    if(byteAddress % 4 != 0)
    {
        (*memError) = true;
        return 0;
    }
    (*memError) = false;
   return fetch(byteAddress/4, memError);
}

// Function: putWord
// Description: puts word at address in memeory[]
// Params: int byteAddress - address to add word
//         unsinged int value - word 
//         bool * memError -  flag for valid address 
// Returns: none
// Modifies: memeory[]
void putWord(int byteAddress, unsigned int value, bool * memError)
{
    if(byteAddress % 4 != 0)
    {
        (*memError) = true;
        return;
    }
    (*memError) = false;
    store(byteAddress/4, value, memError);
}

// Function: clearMemeory 
// Description: clears memeory 
// Params: none
// Returns: none
// Modifies: memory[]
void clearMemory()
{
    clearBuffer((char*)memory, 4096);
}
