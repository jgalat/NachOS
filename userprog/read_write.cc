#include "read_write.h"

void READ_MEM(int usrAddr, int size, int *dst)
{
  if (!(machine->ReadMem(usrAddr, size, dst)))
    ASSERT(machine->ReadMem(usrAddr, size, dst));
}

inline void WRITE_MEM(int usrAddr, int size, int src)
{
  if (!(machine->WriteMem(usrAddr, size, src)))
    ASSERT(machine->WriteMem(usrAddr, size, src));
}

void readStrFromUsr(int usrAddr, char *outStr)
{
  int c;
  do{
    READ_MEM(usrAddr, 1, &c);
    usrAddr++;
    *outStr = c;
    outStr++;
  } while (c != '\0');
 
}
void readBuffFromUsr(int usrAddr, char *outBuff, int byteCount)
{
  int i,c;
  for(i = 0;i < byteCount; i++){
    READ_MEM(usrAddr++, 1, &c);
    outBuff[i] = c;
  } 
}

void writeStrToUsr(char *str, int usrAddr)
{  
  do {
    WRITE_MEM(usrAddr++, 1, (int) *str);
  }while (*(str++) != '\0');
}

void writeBuffToUsr(char *str, int usrAddr, int byteCount)
{
  int i;
  for(i = 0; i < byteCount; i++){
    WRITE_MEM(usrAddr++, 1, (int) str[i]);
  }
}

