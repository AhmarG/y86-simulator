#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "writebackStage.h"
#include "tools.h"
#include "instructions.h"
#include "executeStage.h"
#include "decodeStage.h"
#include "registers.h"
#include "dump.h"

static wregister W;

/* getWregister - gets the w register
 *
 * Returns: copy of the W register
 */
wregister getWregister()
{
    return W;
}

/*
 * clearWregister - clears Wregister
 */
void clearWregister()
{
    clearBuffer((char *) &W, sizeof(W));
    W.stat = SAOK;
    W.icode = NOP;
}

/*
 * writebackStage - determines when the pipeline should stop executing.
 *                  also handles dump instructions
 *
 * Returns: TRUE when pipeline should stop
 */
bool writebackStage(forwardType * forward, statusType* status)
{

    forward->W_dstE = W.dstE;
    forward->W_valE = W.valE;
    forward->W_dstM = W.dstM;
    forward->W_valM = W.valM;
    forward->W_icode = W.icode;
    status->W_stat = W.stat;
    
    if(W.stat == SAOK)
    {
        setRegister(W.dstE, W.valE);
        setRegister(W.dstM, W.valM);
    }

    if(W.stat == SINS)
    {
       printf("Invalid instruction\n");
       dumpProgramRegisters();
       dumpProcessorRegisters();
       dumpMemory();
       return true;
    }
    else if(W.stat == SADR)
    {
        printf("Invalid memory address\n");
        dumpProgramRegisters();
        dumpProcessorRegisters();
        dumpMemory();
        return true;
    }
    else if(W.stat == SHLT)
    {
        return true;
    }
    
    if(W.icode == DUMP && W.valE & 1)
    {
        dumpProgramRegisters();
    }

    if(W.icode == DUMP && W.valE & 2)
    {
        dumpProcessorRegisters();
    }

    if(W.icode == DUMP && W.valE & 4)
    {
        dumpMemory();
    }

    return false;
}

/*
 * update Wregister - updates the Wregister with values from the mem stage
 *
 * params
 *      unsigned int stat, unsigned int icode, unsigned int valE
 *      unsigned int valm, unsigned int dstE, unsigned int dstM
 *
 */
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE,
                     unsigned int valM, unsigned int dstE, unsigned int dstM)
{
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}
