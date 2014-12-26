#include <stdio.h>
#include <stdbool.h>
#include "tools.h"
#include "instructions.h"
#include "forwarding.h"
#include "bubbling.h"
#include "memory.h"
#include "dump.h"
#include "status.h"
#include "registers.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "loader.h"
#include <stdlib.h>
void initialize();

/*
 *  MAIN: creates types for processing and steps 
 *  through each stage like a pipline
 */

int main(int argv, char * args[])
{
    //bool loadError;
    initialize();

    //create forward type 
    forwardType forward;

    //create status type
    statusType status;

    //create bubble type
    bubbleType bubble;
    
   // loadError = !load(argv, args);
    if(!load(argv,args))
    {
        dumpMemory();
        exit(0);
        
    }
    int clockCount = 0;
    bool stop = false;
    while(stop == false)
    {
        stop = writebackStage(&forward, &status);
        memoryStage(&forward, &status, &bubble);
        executeStage(&forward, status, &bubble);
        decodeStage(forward, &bubble);
        fetchStage(forward, bubble);
        clockCount++;
    }
    //dumpProgramRegisters();
    //dumpProcessorRegisters();
    //dumpMemory();
    printf("\nTotal clock cycles = %d\n", clockCount);
}
// create values, delete contents 
void initialize()
{
    createFunPtrArray();
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}


