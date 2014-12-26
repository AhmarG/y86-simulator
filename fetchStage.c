#include <stdbool.h>
#include <stdio.h>
#include "forwarding.h"
#include "bubbling.h" 
#include "instructions.h"
#include "fetchStage.h"
#include "tools.h"
#include "memory.h"
#include "registers.h"
#include "decodeStage.h"


//F register holds the input for the fetch stage. 
//It is only accessible from this file. (static)
static fregister F;

//private function prototypes
static unsigned int selectPC(forwardType forward);
static bool checkValid(unsigned int f_icode);
static unsigned int predictPC(unsigned int f_icode, unsigned int valC, unsigned int valP);
static bool needValC(unsigned int f_icode);
static bool needRegisterID(unsigned int f_icode);
static unsigned int buildValC(unsigned int f_pc, bool* memError);
static unsigned int calcValP(unsigned int f_pc, unsigned int f_icode);
static bool F_stall(bubbleType bubble);
static bool D_stall(bubbleType bubble);
static bool D_bubble(bubbleType bubble);




// Function: getFregister
// Description: Returns a copy of the F register
// Returns: fregister
// Modifies: none
fregister getFregister()
{
    return F;
}

/*
 *clearFregister - clears the F register
 *
 */
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}

/*
 *fetchStage - Fetches an instruction from memory
 */
void fetchStage(forwardType forward, bubbleType bubble)
{
    bool instr_valid;
    bool memError = false;
    unsigned int f_pc = selectPC(forward);
    unsigned int f_stat = SAOK;
    unsigned int f_icode;
    unsigned int f_ifun;
    unsigned int rA = RNONE;
    unsigned int rB = RNONE;
    unsigned int valC = 0;
    unsigned int valP;
    unsigned int testPC;
    unsigned int instruction = getByte(f_pc, &memError);
    
    //set values for f_icode and f_ifun
    //check memError for exception
    if(memError)
    {
        f_stat = SADR;
        f_ifun = 0;
        f_icode = NOP;
    }
    
    
        f_ifun = getBits(0, 3, instruction);
        f_icode = getBits(4, 7, instruction); 
    
    
    //Determine if the intsruction set is valid
    instr_valid = checkValid(f_icode);
    
    
    if(instr_valid == true)
    {
        //F.predPC += 1;
        if(f_icode == HALT)
        {
            f_stat = SHLT;

        }
        if(needRegisterID(f_icode))
        {
            testPC = f_pc + 1;
            instruction = getByte(testPC, &memError);
            if(memError)
                f_stat = SADR;
            rA = getBits(4,7,instruction);
            rB = getBits(0,3,instruction);
            //F.predPC += 1;
        }
        if(needValC(f_icode))
        {
            testPC = f_pc;
            if(needRegisterID(f_icode))
                testPC += 1;
            valC = buildValC(testPC, &memError);
            if(memError)
                f_stat = SADR;

            //F.predPC += 4;
        }

    }
    else
    {
        f_stat = SINS;
        //F.predPC += 1;
    }

    if(!F_stall(bubble))
    {
        valP = calcValP(f_pc, f_icode);
        if(f_icode == JXX || f_icode == CALL)
            F.predPC = valC;
        else
            F.predPC = valP;
    }

    if(!D_stall(bubble))
    {
        if(D_bubble(bubble))
            updateDregister(SAOK, NOP, 0, RNONE, RNONE, 0, 0);
        else
            updateDregister(f_stat,f_icode,f_ifun,rA ,rB ,valC ,valP);
    }
}

/*selectPC - gets address of next instruction
 *
 *Params:
 *     forward - forwarded values
 *
 *Returns: address of next instr
 *
 */
unsigned int selectPC(forwardType forward)
{
    if(forward.M_icode == JXX && !forward.M_Cnd)
        return forward.M_valA;
    else if(forward.W_icode == RET)
        return forward.W_valM;
    else
        return F.predPC;
}

/*
 *predictPC - predict address of next instruction
 *
 *
 * Returns: predicted PC
 */
unsigned int predictPC(unsigned int f_icode, unsigned int valC, unsigned int valP)
{
    switch(f_icode)
    {
        case JXX:
        case CALL: return valC;
    }
    return valP;
}

/*
 * calcValP - determines predicted valP based on length of given instruction
 *
 * Params:
 *      f_pc - address of current instruction
 *      f_icode - current instruction
 *
 * Returns: predicted valP
 */
unsigned int calcValP(unsigned int f_pc, unsigned int f_icode)
{   
    switch(f_icode)
    {
        case HALT:
        case NOP:
        case RET:
            return  f_pc + 1;
        case OPL:
        case RRMOVL:
        case PUSHL:
        case POPL:
            return f_pc + 2;
        case DUMP:
        case JXX:
        case CALL:
            return f_pc + 5;
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            return f_pc + 6;
        default:
            return f_pc + 1;
    }
}

/*checkValid - checks to see if instruction can be used 
 *
 *Params: 
 *    unsinged int * f_icode, points to icode funtion 
 *Returns
 *    bool true, or false
 */
bool checkValid(unsigned int f_icode)
{
    switch (f_icode) 
    {
        case HALT:
        case NOP:
        case DUMP:
        case IRMOVL:
        case OPL:
        case RRMOVL:
        case RMMOVL:
        case MRMOVL:
        case POPL:
        case PUSHL:
        case RET:
        case JXX:
        case CALL: 
        return true;
        
        default:
            return false;
    }
}

/*
 *needRegisterID - checks if instruction needs the registerID
 *
 * Params:
 *      f_icode - instruction code
 *
 *Returns: TRUE if instruction requires registerID
 */
bool needRegisterID(unsigned int f_icode)
{
    switch(f_icode)
    {
        case RRMOVL:
        case OPL:
        case PUSHL:
        case POPL:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            return true;
        default:
                return false;
    }
}


/*
 *needValC - checks if instruction needs valC
 *
 * Params:
 *      f_icode - instruction code
 *
 *Returns: TRUE if valC is needed for instruction
 */
bool needValC(unsigned int f_icode)
{
    switch(f_icode)
    {
        case DUMP:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
        case JXX:
        case CALL:
            return true;

        default:
                return false;
    }
}

/*
 *buildValC - builds a word for valC 
 *
 * Params: 
 *      f_pc - address of value start building from
 *      memError - detects memory errors
 *
 * Returns: correct valC
 */
unsigned int buildValC(unsigned int f_pc, bool* memError)
{
    unsigned char  byte0 = getByte(f_pc + 1, memError);
    unsigned char  byte1 = getByte(f_pc + 2, memError);
    unsigned char  byte2 = getByte(f_pc + 3, memError);
    unsigned char  byte3 = getByte(f_pc + 4, memError);

    return buildWord(byte0,byte1,byte2,byte3);
}
/*
 * F_stall - checks to see if the fetchstage needs to be stalled 
 *
 * Params: 
 *      bubble - pointer to a bubble struct 
 *
 * returns true or false 
 */
bool F_stall(bubbleType bubble)
{
    if(bubble.D_icode == RET || bubble.E_icode == RET ||
        bubble.M_icode == RET)
        return true;
    else if((bubble.E_icode == MRMOVL || bubble.E_icode == POPL)
        && (bubble.E_dstM == bubble.srcA || bubble.E_dstM == bubble.srcB))
        return true;

    return false;
}

/*
 * D_stall - checks to see if the the decodeStages needs to be stalled
 *
 * Params:
 *      bubble- pointer to a bubble struct
 *
 *returns true or false
 */
bool D_stall(bubbleType bubble)
{
    if((bubble.E_icode == MRMOVL || bubble.E_icode == POPL)
        &&(bubble.E_dstM==bubble.srcA || bubble.E_dstM == bubble.srcB))
        return true;

    return false;
}

/*
 *D_bubble - checks to see if decodeStage needs to be bubbled 
 *
 * Params:
 *      bubble- pointer to a bubble struct
 *
 * returns ture or false
 */
bool D_bubble(bubbleType bubble)
{
    bool branchMisPred = (bubble.E_icode == JXX && !bubble.e_Cnd);
    
    bool retr = (bubble.D_icode == RET || bubble.E_icode == RET
                    || bubble.M_icode == RET);
    
    bool hazard = (!((bubble.E_icode == MRMOVL || bubble.E_icode == POPL)
                && (bubble.E_dstM == bubble.srcA ||
                bubble.E_dstM == bubble.srcB)) && retr);
    
    if(branchMisPred || hazard)
        return true;
    
    return false;
}

