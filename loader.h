#ifndef LOADER_H
#define LOADER_H
bool load(int args, char* argv[]);
bool validFileName(char* fileName);
void discardRest(char* oldString, FILE* fileptr);
int grabAddress(char* dataLine);
bool isAddress(char* dataLine);
bool isData(char* dataLine);
bool isSpaces(char* dataLine, int start, int end);
bool checkHex(char* dataLine, int start, int end);
unsigned char grabDataByte(char* dataLine, int start);
bool checkLine(char* dataLine, int lastAddress);
bool checkAddrFormat(char* dataLine, int lastAddress);
bool checkDataFormat(char* dataLine);
int countBytes(char* dataLine);
#endif




