#include <stdbool.h>
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "tools.h"
#include "executeStage.h"
#include "instructions.h"
#include "registers.h"

/*
 *D register holds the input for the decode stage.
 *It is only accessible from this file. (static)
 */

static dregister D;

//private function prototypes
static unsigned int getSrcA();
static unsigned int getSrcB();
static unsigned int getDstE();
static unsigned int getDstM();
static unsigned int selectFwdA(unsigned int srcA, forwardType forward);
static unsigned int forwardb(unsigned int srcB, forwardType forward);
static bool bubbleE(bubbleType* bubble);


/*
* Return: copy of the Dregister
*/
dregister getDregister()
{
    return D;
}

/*
 * clearDregister - clears out Dregister and resets status
 */
void clearDregister()
{
    clearBuffer((char*) &D, sizeof(D));
    D.stat = SAOK;
    D.icode = NOP;
}

/*
 * decodeStage - updates values for the executeStage
 */
void decodeStage(forwardType forward, bubbleType* bubble)
{
    unsigned int srcA = getSrcA();
    unsigned int srcB = getSrcB();
    unsigned int dstE = getDstE();
    unsigned int dstM = getDstM();
    //ready forwarding values
    unsigned int valA = selectFwdA(srcA, forward);
    unsigned int valB = forwardb(srcB, forward);
    
    //actions taken if bubbling
    bubble->D_icode = D.icode;
    bubble->srcA = srcA;
    bubble->srcB = srcB;
    
    if(bubbleE(bubble))
     {
         updateEregister(SAOK, NOP, 0, 0, 0, 0, RNONE, RNONE, RNONE, RNONE);
     }
    else
     {
         updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB, dstE, dstM, srcA, srcB);
     }
}

/*
*updateDregister- sets the fields of Dregister to be used in decode stage 
*
*Params
*	unsigned int stat, unsigned in icode, unsigned int ifun, unsigned int ifun,
*	unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP
*
*Returns: none 
*/
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun,
                     unsigned int	rA, unsigned int rB, unsigned int valC, unsigned int valP)
{
D.stat = stat;
D.icode = icode;
D.ifun = ifun;
D.rA = rA;
D.rB = rB;
D.valC = valC;
D.valP = valP;
}

/*
 * getSrcA - checks for value of SrcA by comparing intruction codes
 *         - what register should be used as the A source 
 * Params NONE
 *
 * returns: D.rA, ESP or RNONE
 */
unsigned int getSrcA()
{
    switch(D.icode)
    {
        case RRMOVL:
        case RMMOVL:
        case OPL:
        case PUSHL:
            return D.rA;
        case POPL:
        case RET:
            return ESP;
        default:
            return RNONE; // does not need register 
    }
}

/*
 * getSrcB - checks for value of SrcB by comparing instruction codes
 *         - what register should be used as the B source
 * Params: NONE
 *
 * returns: D.rB, ESP
 */
unsigned int getSrcB()
{
 switch(D.icode)
 {
     case OPL:
     case RMMOVL:
     case MRMOVL:
        return D.rB;
     case PUSHL:
     case POPL:
     case CALL:
     case RET:
        return ESP;
     default:
        return RNONE;
 }
}

/*
 * getDstE - determines value to be used for destination register
 *
 * Returns: destination register
 */
unsigned int getDstE()
{
 switch(D.icode)
 {
     case RRMOVL:
     case IRMOVL:
     case OPL:
        return D.rB;
     case PUSHL:
     case POPL:
     case CALL:
     case RET:
        return ESP;
     default:
        return RNONE;
  }    
}

/*
 * getDstM - determines value to be used for destination memory location
 *
 * Returns: memory location destination
 */
unsigned int getDstM()
{
 switch(D.icode)
 {
     case MRMOVL:
     case POPL:
        return D.rA;
     default:
        return RNONE;    
 }
}

/*
 * selectFwdA - determines value to be used as valA in executeStage
 * Params:
 *      srcA - source register
 *
 *Returns: valA to be used
 */
unsigned int selectFwdA(unsigned int srcA, forwardType forward)
{
 if(D.icode == CALL || D.icode == JXX){ return D.valP;}
 else if(srcA == RNONE) {  return 0; }
 else if(srcA == forward.e_dstE){ return forward.e_valE;}
 else if(srcA == forward.M_dstM){ return forward.m_valM;}
 else if(srcA == forward.M_dstE){ return forward.M_valE;}
 else if(srcA == forward.W_dstM){ return forward.W_valM;}
 else if(srcA == forward.W_dstE){ return forward.W_valE;}
 else
    return getRegister(srcA);

}

/*
 * forwardb - detemines value to be used as valB in executeStage
 * Params:
 *      srcB - source register
 *
 *Returns: valB to be used
 */
unsigned int forwardb(unsigned int srcB, forwardType forward)
{
 if(srcB == RNONE) {  return 0; }
 else if(srcB == forward.e_dstE){ return forward.e_valE;}
 else if(srcB == forward.M_dstM){ return forward.m_valM;}
 else if(srcB == forward.M_dstE){ return forward.M_valE;}
 else if(srcB == forward.W_dstM){ return forward.W_valM;}
 else if(srcB == forward.W_dstE){ return forward.W_valE;}
 else
    return getRegister(srcB); 
}

/*
 *bubbleE - determines if the E register needs to be bubbled
 *
 *Params:
 *      bubble - register value for bubbling
 *
 *Returns: TRUE if E register needs to be bubbled
 */
bool bubbleE(bubbleType* bubble)
{
    bool branchMisPred = (bubble->E_icode == JXX && !bubble->e_Cnd);
    bool hazard = ((bubble->E_icode == MRMOVL || bubble->E_icode == POPL)
                    && (bubble->E_dstM == bubble->srcA || bubble->E_dstM == bubble->srcB));
    return branchMisPred || hazard;
}


