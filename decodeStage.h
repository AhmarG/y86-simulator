#ifndef DECODESTAGE_H
#define DECODESTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
}dregister;

//prototypes for fuctions
dregister getDregister();
void clearDregister();
void decodeStage(forwardType forward, bubbleType* bubble);
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, 
					unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);
#endif
