#include <stdbool.h>
#include "forwarding.h"
#include "bubbling.h"
#include "instructions.h"
#include "status.h"
#include "memory.h"
#include "memoryStage.h"
#include "executeStage.h"
#include "tools.h"
#include "writebackStage.h"

static mregister M;

//prototypes
static unsigned int mem_addr();
static bool mem_read();
static bool mem_write();

/*
 * Returns: copy of the Mregister
 */
mregister getMregister()
{
    return M;
}

/*
 * clearWregister - clears Mregister
 */
void clearMregister()
{
    clearBuffer((char *) &M, sizeof(M));
    M.stat = SAOK;
    M.icode = NOP;
}

/*
 * memoryStage - writes or reads from memory when needed
 *               Updates values for writebackStage
 *
 * params - forwardType * forward, statusType* status, bubbleType* bubble
 * 
 */
void memoryStage(forwardType * forward, statusType* status, bubbleType* bubble)
{
    unsigned int addr = mem_addr();
    unsigned int valM = M.valA;
    bool memError = false;
    unsigned int stat = M.stat;
    
    if(mem_write())
    {
        putWord(addr, valM, &memError);
    }
    if(mem_read())
    {
        valM = getWord(addr, &memError);
    }
    if(memError)
        stat = SADR;
    
    //forward values
    forward->M_dstM = M.dstM;
    forward->M_dstE = M.dstE;
    forward->m_valM = valM;
    forward->M_valE = M.valE;
    forward->M_Cnd = M.Cnd;
    forward->M_valA = M.valA;
    forward->M_icode = M.icode;
    status->m_stat = stat;
    
    bubble->M_icode = M.icode;

    updateWregister(stat, M.icode, M.valE, valM, M.dstE, M.dstM);
}

/*
 * updateMregister- updates the memory register
 *
 * params unsigned int stat, unsigned int icode, unsigned int cnd,
 *        unsigned int valE, unsigned int valA, unsigned int valE
 *        unsigned int dstm
 */
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd,
                     unsigned int valE, unsigned int valA, unsigned int dstE,
                     unsigned int dstM)
{
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

/*
 * mem_addr - selects a memory address
 *
 * Returns: memory address
 */
unsigned int mem_addr()
{
    switch(M.icode)
    {
        case RMMOVL:
        case PUSHL:
        case CALL:
        case MRMOVL:
            return M.valE;
        case POPL:
        case RET:
            return M.valA;
        default:
            return 0;
    }
}

/*
 * mem_read - sets read control signal
 *
 * Returns: TRUE if instruction requires a read from memory
 */
bool mem_read()
{
    switch(M.icode)
    {
        case MRMOVL:
        case POPL:
        case RET:
            return true;
        default:
            return false;
    }
}

/*
 *mem_write - sets write control signal
 *
 * Returns: TRUE if instruction requires write to memory
 */
bool mem_write()
{
    switch(M.icode)
    {
        case RMMOVL:
        case PUSHL:
        case CALL:
            return true;
        default:
            return false;
    }
}
