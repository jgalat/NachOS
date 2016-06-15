#include "copyright.h"
#include "synchconsole.h"
#include "console.h"

void ReadAvail(void *c) 
{
  ((SynchConsole*) c)->readAvailV();
}


void WriteDone(void *c) 
{
  ((SynchConsole*) c)->writeDoneV(); 
}



SynchConsole::SynchConsole(const char *in, const char *out)
{
  readAvail = new Semaphore("read", 0);
  writeDone = new Semaphore("write", 0); 
  oneRead = new Lock("oneread");
  oneWrite = new Lock("onewrite");
  console = new Console(in, out, ReadAvail, WriteDone, this);
}


SynchConsole::~SynchConsole()
{
  delete readAvail;
  delete writeDone;
  delete oneRead;
  delete oneWrite;
  delete console;
}

void
SynchConsole::Write(char c)
{
  oneWrite->Acquire();
  console->PutChar(c);
	writeDone->P();
	oneWrite->Release();
}

char
SynchConsole::Read()
{
  oneRead->Acquire();
  readAvail->P();
	char ch = console->GetChar();
	oneRead->Release();
	return ch;
}  


