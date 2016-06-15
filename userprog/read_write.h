#ifndef READ_WRITE_H
#define READ_WRITE_H

#include "system.h"
#include "copyright.h"

void READ_MEM(int usrAddr, int size, int *dst);

void readStrFromUsr(int usrAddr, char *outStr);

void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount);

void writeStrToUsr(char *str, int usrAddr);

void writeBuffToUsr(char *str, int usrAddr, int byteCount);

#endif
