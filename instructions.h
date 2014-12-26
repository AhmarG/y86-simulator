//icodes
#define HALT    0x0
#define NOP     0x1
#define RRMOVL  0x2
#define IRMOVL  0x3
#define RMMOVL  0x4
#define MRMOVL  0x5
#define OPL     0x6
#define JXX     0x7
#define CALL    0x8
#define RET     0x9
#define PUSHL   0xA
#define POPL    0xB
#define DUMP    0xC

//function codes
#define ADDL    0x0
#define SUBL    0x1
#define ANDL    0x2
#define XORL    0x3

//conditional moverino
#define RRMOVLC 0x0
#define CMOVLE  0x1
#define CMOVL   0x2
#define CMOVE   0x3
#define CMOVNE  0x4
#define CMOVGE  0x5
#define CMOVG   0x6

//conditional jumperino
#define JMP     0x0
#define JLE     0x1
#define JL      0x2
#define JE      0x3
#define JNE     0x4
#define JGE     0x5
#define JG      0x6

//status codes
#define SAOK    1 // AOK
#define SHLT    2 // HLT
#define SADR    3 // ADR
#define SINS    4 // INS
