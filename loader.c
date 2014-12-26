#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tools.h"
#include "loader.h"
#include "memory.h"

/*
 *load - main load function to load instructions and
 *       data into memory from a source file. performs
 *       error checking on file
 *
 * Params:
 *       args - number of arguments
 *       argv[] - command line arguments
 *
 *Returns: TRUE if load was successful
 */
bool load(int args, char* argv[])
{
    FILE* file;
    char* fileName = argv[1];
    char dataLine[80];
    bool memError;

    //check for IO issues
    if(args != 2 || !validFileName(fileName))
    {
        printf("\nfile opening failed");
        printf("\nusage: yess <fileName>.yo\n");
        return false;
    }

    file = fopen(fileName, "r");
    if(file == NULL)
    {
        printf("\nfile did not open correctly");
        printf("usage: yess <fileName>.yo\n");
        fclose(file);
        return false;
    }
    
    //attempt to read file and write to memory
    int lastAddress = -1;
    int lineNum = 1;
    int byteAddress;
    int totalByteNum;
    unsigned char dataByte;

    while(fgets(dataLine, 80, file) != NULL)
    {
        int lineLength = strlen(dataLine);
        if(dataLine[lineLength-1] != '\n')
        {
            discardRest(dataLine, file);
        }

        if(checkLine(dataLine, lastAddress))
        {
            if(isAddress(dataLine))
            {
                byteAddress = grabAddress(dataLine);
                if(isData(dataLine))
                {
                    totalByteNum = countBytes(dataLine);
                    int byteNum;
                    for(byteNum = 1; byteNum <= totalByteNum; byteNum++)
                    {
                        dataByte = grabDataByte(dataLine, (byteNum*2)+7);
                        putByte(byteAddress, dataByte, &memError);
                        if(memError)
                        {
                            fclose(file);
                            return false;
                        }
                        byteAddress++;
                    }
                    lastAddress = byteAddress-1;
                }
            }
            lineNum++;
        }
        else
        {
            printf("Error on line %d\n", lineNum);
            int i;
            for(i = 0; i < strlen(dataLine); i++)
            {
                printf("%c", dataLine[i]);
            }
            printf("\n");
            fclose(file);
            return false;
        }
    }
    fclose(file);
    return true;
}


/*
 *validFileName - makes sure the file extension is .yo
 *
 *Params:
 *      *fileName - name of file
 *
 * Returns: TRUE if fileName ends in .yo
 */
bool validFileName(char* fileName)
{
    int length = strlen(fileName);

    if(fileName[length-1] == 'o'
        && fileName[length-2] == 'y'
        && fileName[length-3] == '.')
    {
        return true;
    }
    return false;
}

/*
 *discardFile - checks if the line in the file is longer
 *              than 80 chars. If so, it keeps reading
 *              chars until the newline is read.
 *
 *Params:
 *       oldString - chars in the line read so far              
 *       fileptr - pointer to the file being read from
 */
void discardRest(char* oldString, FILE* fileptr)
{
    int len = strlen(oldString);
    if(oldString[len-1] == '\n')
        return;
    int check;

    do
    {
       check =  fgetc(fileptr);
    }
    while(check != '\n' && check != EOF);
}

/*
 *grabAddress - gets the address of the data line
 *  
 *Params:
 *       dataLine - line of data in the file
 *
 *Returns: address of the data line
 *
 */
int grabAddress(char* dataLine)
{
    return (int)strtol(dataLine, NULL, 16);
}

/*
 *isAddress - checks if the record contains an address
 *
 *Params:
 *      dataLine - line of data in the file
 *
 *Returns: TRUE if an address exists
 */
bool isAddress(char* dataLine)
{
    if(dataLine[2] != '0')
        return false;
    if(dataLine[3] != 'x')
        return false;
    if(dataLine[7] != ':')
        return false;

    int x;
    for(x = 4; x < 7; x++)
    {
        if(isxdigit(dataLine[x]) == 0)
            return false;
    }
    return true;
}

/*
 *isData - checks if the record has data to be stored
 *
 *Params:
 *      dataLine - line of data in the file
 *
 * Returns - TRUE if there is data to be stored
 */
bool isData(char* dataLine)
{
    if(isxdigit(dataLine[9]))
    {
        return true; 
    }
    return false;
}

/*
 *isSpaces - checks if there are spaces in a given range
 *
 *Params:
 *      dataLine - line of data in the file
 *      start - beginning of the examined range
 *      end - end of the examined range
 *
 *Returns: TRUE if the range contains spaces     
 */
bool isSpaces(char* dataLine, int start, int end)
{
    if(start > end)
        return false;
    while(start <= end)
    {
        if(dataLine[start] != ' ')
            return false;
        start++;
    }
    return true;
}

/*
 *checkHex - checks if there are hex chars in a given range
 *
 *Params:
 *      dataLine - line of data in the file
 *      start - beginning of examined range
 *      end - end of examined range
 *
 *Returns: TRUE if range contains hex digits
 */
bool checkHex(char* dataLine, int start, int end)
{
    while(start <= end)
    {
        if(isxdigit(dataLine[start]) != 0)
            return true;
        start++;
    }
    return false;
}

/*
 *grabDataByte - grabs a byte of data from a line of data
 *
 *Params:
 *      dataLine - line of data in the file
 *      start - starting index into the data record
 *
 *Returns: byte of data chosen
 */
unsigned char grabDataByte(char* dataLine, int start)
{
    char byte[3] = {dataLine[start], dataLine[start+1], '\0'};
    return (unsigned char)strtol(byte, NULL, 16);
}

/*
 *checkLine - checks for errors in a line
 *
 *Params:
 *      dataLine - line in a file
 *      lastAddress - previous address
 *
 *Returns:  TRUE if line contains no errors     
 */
bool checkLine(char* dataLine, int lastAddress)
{
    if(dataLine[22] != '|')
        return false;
    
    if(isAddress(dataLine))
    {
        if(isSpaces(dataLine, 0,1) && isSpaces(dataLine,8,8) && isSpaces(dataLine,21,21))
        {
            if(checkAddrFormat(dataLine, lastAddress))
            {
                if(checkDataFormat(dataLine))
                    return true;

                else if(isSpaces(dataLine,8,20))
                    return true;
            }
                
        }
    }
    else if(isSpaces(dataLine,0,21))
        return true;

    return false;

}

/*
 *checkAddrFormat - helper for checkLine. makes sure address is
 *                  greater than previous address
 *                   
 *Params:
 *      dataLine - line in the file
 *      lastAddress - previous address
 *
 *Returns: TRUE if address is correct
 */
bool checkAddrFormat(char* dataLine, int lastAddress)
{
    if(checkHex(dataLine,4,6))
    {
        if(lastAddress == -1)
            return true;
        else
        {
            int currAddress = grabAddress(dataLine);
            if(currAddress > lastAddress)
                return true;
            else
                return false;
        }
    }
    else
        return false;
}

/*
 *checkDataFormat - helper for checkLine. makes sure data
 *                  is in correct format
 *
 *Params:
 *      dataLine - line in the file
 *
 *Returns: TRUE if data is in correct format      
 */
bool checkDataFormat(char* dataLine)
{
    if(isData(dataLine))
    {
        int x = 9;
        while(isxdigit(dataLine[x]) != 0)
        {
            x++;
        }
        if(((x-9)%2) != 0)
            return false;

        if(isSpaces(dataLine, x, 21))
        {
            return true;
        }
    }
    return false;
}

/*
 *countBytes - counts number of bytes in a valid dataLine
 *
 *Params:
 *      dataLine - line of data in the file
 *
 * Returns: number of bytes of data on that line
 */
int countBytes(char* dataLine)
{
    int first = 9;
    int second = 10;
    int total = 0;
    while(checkHex(dataLine, first, second))
    {
        first +=2;
        second +=2;
        total++;
    }
    return total;

}
