#include <stdio.h>
#include <stdbool.h>
#include "tools.h"

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).

   Author: Ahmar Gordon
   UserID: gordona
*/

/*
 *getBits - gets the bits of an unsigned int
 *          in a selected bit range
 *
 *Params:
 *      low - low significant bit
 *      high - high significant bit
 *      source - int to extract bit from
 *Returns: 
 *      Bits in a selected range
 *
 */
unsigned int getBits(int low, int high, unsigned int source)
{
    if(low > high)
    {
        return 0;
    }
    source = source << (31 - high);
    source = source >> (31 - high + low);
    return source;
} 

/*
 *setBits - sets the bits of an unsigned int
 *          in a selected bit range to 1
 *
 *Params:
 *      low - low significant bit
 *      high - high significant bit
 *      source - int to extract bit from
 *Returns: 
 *      Newly changed unsigned int
 *
 */
unsigned int setBits(int low, int high, unsigned int source)
{
    if(low > high || low < 0 || high > 31)
    {
        return source;
    }
    unsigned int x = getBits(low, high, -1);
    x = x << low;
    source = source | x;
    return source;
}

/*
 *clearBits - changes the bits of an unsigned int
 *          in a selected bit range to 0
 *
 *Params:
 *      low - low significant bit
 *      high - high significant bit
 *      source - int to extract bit from
 *Returns: 
 *      Newly changed unsigned int
 *
 */
unsigned int clearBits(int low, int high, unsigned int source)
{
    if(low > high || low < 0 || high > 31)
        return source;

    unsigned int x = getBits(low, high, -1);
    x = ~(x << low);
    source = source & x;
    return source;
}

/*
 *assignOneBit - changes one bit of an unsigned int to 0
 *
 *Params:
 *      bitNumber - selected bit to change
 *      bitValue - desired value of bit
 *      source - int to change selected bit in
 *Returns: 
 *      Newly changed unsigned int
 *
 */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
    if(bitNumber < 0 || bitNumber > 31 || bitValue < 0 || bitValue > 1)
        return source;
    
    if(bitValue == 0)
    {
        return clearBits(bitNumber, bitNumber, source);
    }
    return setBits(bitNumber, bitNumber, source);
}

/*
 *getByteNumber - gets a byte of an unsigned int
 *
 *Params:
 *      byteNo - selected byte to return
 *      source - int to get byte from
 *Returns: 
 *      A byte of an unsigned int as a char
 *
 */
unsigned char getByteNumber(int byteNo, unsigned int source)
{
    if(byteNo < 0 || byteNo > 3)
        return 0;

    if(byteNo == 0)
        return getBits(0, 7, source);
    else if(byteNo == 1)
        return getBits(8, 15, source);
    else if(byteNo == 2)
        return getBits(16, 23, source);
    else if(byteNo == 3)
        return getBits(24, 31, source);
    else
        return 0;
}

/*
 *putByteNumber - changes a byte of an unsigned int to a 
 *                desired value
 *
 *Params:
 *      byteNo - selected byte to change
 *      byteValue - value to be placed in byte
 *      source - int to change byte in
 *Returns: 
 *      A byte of an unsigned int as a char
 *
 */
unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
    if(byteNo < 0 || byteNo > 3)
        return source;

    if(byteNo == 0)
    {
        source = clearBits(0, 7, source);
        return source | byteValue;
    }
    else if(byteNo == 1)
    {
        source = clearBits(8, 15, source);
        unsigned int shift = byteValue << 8;
        source = source | shift;
        return source;
    }
    else if(byteNo == 2)
    {
        source = clearBits(16, 23, source);
        unsigned int shift = byteValue << 16;
        return source | shift;
    }
    else if(byteNo == 3)
    {
        source = clearBits(24, 31, source);
        unsigned int shift = byteValue << 24;
        return source | shift;
    }
    else
        return source;

}

/*
 *buildWord - creates an unsigned int from given bits
 *
 *Params:
 *      byte0 - first byte of new int
 *      byte1 - second byte of new int
 *      byte2 - third byte of new int
 *      byte3 - fourth byte of new int
 *Returns: 
 *      new unsigned int from given char bytes
 *
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
    unsigned int x = 0;
    x = x | byte0;
    x = x | (byte1 << 8);
    x = x | (byte2 << 16);
    x = x | (byte3 << 24);
    return x;
}

 /*
 *isNegative - determines if an unsigned int would be negative
 *             if it were treated as signed
 *
 *Params:
 *      source - unsigned int to evaluate
 *Returns: 
 *      true if the int would be negative, false if not
 *
 */
bool isNegative(unsigned int source)
{
    source = source >> 31;
    if(source == 1)
        return true;
    return false;
}

/*
 *expandBits - initializes an array to contain a char
 *             representation of an int in binary
 *
 *Params:
 *      source - int to expand into char array
 *      bits - char array containing binary values
 *
 */
void expandBits(unsigned int source, char bits[36])
{
    int i;
    bits[35] = 0;
    for(i = 34; i >= 0; i--)
    {
        if(i == 8 || i == 17 || i == 26)
        {
            bits[i] = ' ';
        }
        else
        {
            if((source & 1) == 1)
            {
                bits[i] = '1';
            }
            else
            {
                bits[i] = '0';
            }
            source = source >> 1;
        }
    }
}

/*
 *clearBuffer - initializes each byte of a chunk of data 
 *              to 0
 *
 *Params:
 *      buff - pointer for chunk of data
 *      size - size of a piece of the chunk of data
 *
 */
void clearBuffer(char * buff, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        *buff = '\0';
        buff++;
    }
}


