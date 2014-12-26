#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
}eregister;

//prototypes
eregister getEregister();
void clearEregister();
void executeStage(forwardType* forward, statusType status, bubbleType* bubble);
void createFunPtrArray();
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun, 
					unsigned int valC, unsigned int valA, unsigned int valB, 
					unsigned int dstE, unsigned int dstM, unsigned int srcA,
					unsigned int srcB );
#endif
