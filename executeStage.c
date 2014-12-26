#include <stdbool.h>
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "executeStage.h"
#include "tools.h"
#include "memoryStage.h"
#include "instructions.h"
#include "registers.h"

/*
 *E register holds the input for the execute stage.
 *It is only accessible from this file. (static)
 */
static eregister E;

//private function prototypes
typedef unsigned int (*f)();
f funPtr[16];
static int calcCnd();
static int calcJump();
static bool set_cc(statusType status);
static bool M_bubble(statusType status);
static unsigned int retZero();
static unsigned int doDump();
static unsigned int performOpl();
static unsigned int performIRMOVL();
static unsigned int performRRMOVL();
static unsigned int performRMMOVL();
static unsigned int performMRMOVL();
static unsigned int performPOPL();
static unsigned int performPUSHL();
static unsigned int performCALL();
static unsigned int performRET();

bool modCC;

/**
 *Returns: copy of eregister
 */
eregister getEregister()
{
    return E;
}

/*
 *Clear the E register
 */
void clearEregister()
{
    clearBuffer((char*) &E, sizeof(E));
    E.icode = NOP;
    E.stat = SAOK;
}

/*
 *executeStage - Executes a given instruction and detects
 *               branch misprediction
 *Params:
 *       forward - pointer for forwarded values
 *       status - struct for M and W stages' status
 */
void executeStage(forwardType* forward, statusType status, bubbleType* bubble)
{
    modCC = set_cc(status);
    int e_Cnd = calcCnd();
    int M_Cnd = calcJump();
    unsigned int valE = funPtr[E.icode](modCC);
    if((E.icode == RRMOVL) && !e_Cnd)
    {
        E.dstE = RNONE;
    }
    forward->e_dstE = E.dstE;
    forward->e_valE = valE;
    
    //pass along bubbling values
    bubble->e_Cnd = M_Cnd;
    bubble->E_icode = E.icode;
    bubble->E_dstM = E.dstM;

    if(M_bubble(status))
    {
        updateMregister(SAOK, NOP, 0, 0, 0, RNONE, RNONE);
    }
    else
    {
        updateMregister(E.stat, E.icode, M_Cnd, valE, E.valA, E.dstE, E.dstM);
    }
}

/*
 * updateEregister - updates the E register values
 */

void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int valC, unsigned int valA, unsigned int valB,
                     unsigned int dstE, unsigned int dstM, unsigned int srcA,
                     unsigned int srcB)
{
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

/*
 *createFunPtrArray - initializes an array of function pointers
 */
void createFunPtrArray()
{
    int i;
    for(i = 0; i < 16; i++)
    {
        funPtr[i] = retZero;
    }

    funPtr[HALT] = retZero;
    funPtr[NOP] = retZero;
    funPtr[DUMP] = doDump;
    funPtr[OPL] = performOpl;
    funPtr[IRMOVL] = performIRMOVL;
    funPtr[RRMOVL] = performRRMOVL;
    funPtr[RMMOVL] = performRMMOVL;
    funPtr[MRMOVL] = performMRMOVL;
    funPtr[POPL] = performPOPL;
    funPtr[PUSHL] = performPUSHL;
    funPtr[CALL] = performCALL;
    funPtr[RET] = performRET;
}

/*
 *calcCnd - sets the Cnd flags to take a conditional move
 *
 * Returns: value of Cnd
 */
int calcCnd()
{
    int sf = getCC(SF);
    int zf = getCC(ZF);
    int of = getCC(OF);
    
    if(E.icode == RRMOVL)
    {
        switch(E.ifun)
        {
            case RRMOVLC:
                return 1;
                break;
            case CMOVLE:
                if((sf ^ of) | zf)
                    return 1;
                    break;
            case CMOVL:
                if(sf ^ of)
                    return 1;
                    break;
            case CMOVE:
                if(zf)
                    return 1;
                    break;
            case CMOVNE:
                if(!zf)
                    return 1;
                    break;
            case CMOVGE:
                if(!(sf^of))
                    return 1;
                    break;
            case CMOVG:
                if(!(sf^of) & (!zf))
                    return 1;
                    break;
            default:
                return 0;
        }
    }
    return 0;
}

/*
 *
 */
int calcJump()
{
    int sf = getCC(SF);
    int zf = getCC(ZF);
    int of = getCC(OF);

    if(E.icode == JXX)
    {
        switch(E.ifun)
        {
            case JMP:
                return 1;
            case JLE:
                if((sf^of) || zf)
                    return 1;
                    break;
            case JL:
                if(sf^of)
                    return 1;
                    break;
            case JE:
                if(zf)
                    return 1;
                    break;
            case JNE:
                if(!zf)
                    return 1;
                    break;
            case JGE:
                if(!(sf^of))
                    return 1;
                    break;
            case JG:
                if(!(sf^of) && !zf)
                    return 1;
                    break;
            default:
                return 0;
        }
    }
    return 0;
}

/*
 * set_cc - determines whether the cc should be set based on exceptions
 *
 * Params:
 *      status - struct for M and W stages' status
 *
 * Returns: TRUE if it is safe to set cc
 */
bool set_cc(statusType status)
{
    unsigned int m = status.m_stat;
    unsigned int w = status.W_stat;
    if(E.icode == OPL)
    {
        if(m != SADR && m != SINS && m != SHLT)
        {
            if(w != SADR && w != SINS && w != SHLT)
            {
                return true;
            }
        }
    }
    return false;
}

/*
 * M_bubble - Determines if mem stage should be bubbled
 *
 * Params:
 *       status - struct for M and W stages' status
 * Returns: TRUE if M reg should be bubbled
 */
bool M_bubble(statusType status)
{
    unsigned int m = status.m_stat;
    unsigned int w = status.W_stat;
    if(m == SADR || m == SINS || m == SHLT)
    {
        return true;
    }
    if(w == SADR || w == SINS || w == SHLT)
    {
        return true;
    }
    return false;

}

/*
 * Returns zero for HALT and NOP
 */
unsigned int retZero()
{
    return 0;
}

/*
 * executes a dump
 */
unsigned int doDump()
{
    return E.valC;
}

/*
 * executes an ADDL SUBL ANDL or XORL. updates CC
 *
 * Returns: value of computation
 */
unsigned int performOpl()
{
    int a = (int)E.valA;
    int b = (int)E.valB;
    int value = 0;
    if(modCC)
    {
        resetCC();
        switch(E.ifun)
        {
            case ADDL:
            value = a + b;
            if(value == 0)
                setCC(ZF, 1);
            if(value < 0)
                setCC(SF, 1);
            if(((a < 0) == (b < 0)) && ((value < 0) != (a < 0)))
                setCC(OF, 1);
                break;
            case SUBL:
            value = b - a;
            if(value == 0)
                setCC(ZF, 1);
            if(value < 0)
                setCC(SF, 1);
            if(((a > 0) != (b > 0)) && ((value > 0) == (a > 0)))
                setCC(OF, 1);
                break;
            case ANDL:
            value = a & b;
            if(value == 0)
                setCC(ZF, 1);
            if(value < 0)
                setCC(SF, 1);
                break;
            case XORL:
            value = a ^ b;
            if(value == 0)
                setCC(ZF, 1);
            if(value < 0)
                setCC(SF, 1);
                break;
        }

    }
    return value;
}

/*
 * executes an immediate register move
 */
unsigned int performIRMOVL()
{
    return E.valC;
}

/*
 * executes a register to register move and a conditional move
 */
unsigned int performRRMOVL()
{
    return E.valA;
}

/*
 * execute a register to memory move
 */
unsigned int performRMMOVL()
{
    return E.valB + E.valC;
}

/*
 * executes a memory to register move
 */
unsigned int performMRMOVL()
{
    return  E.valB + E.valC;
}

/*
 *executes a pop
 */
unsigned int performPOPL()
{
    return E.valB + 4;
}

/*
 * executes a push
 */
unsigned int performPUSHL()
{
    return E.valB - 4;
}

/*
 *
 */
unsigned int performCALL()
{
    return E.valB - 4;
}

unsigned int performRET()
{
    return E.valB + 4;
}
