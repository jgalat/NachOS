#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"

void ReadAvail(void *c);
void WriteDone(void *c);

class SynchConsole {
  public:
    SynchConsole(const char *in, const char *out);
    ~SynchConsole();
    
    void Write(char c);
    char Read();
    
    void readAvailV() { readAvail->V(); }
    void writeDoneV() { writeDone->V(); }


  private:
    Console *console;
    Semaphore *readAvail, *writeDone;
    Lock *oneRead, *oneWrite;
};



#endif //SYNCHCONSOLE_H
