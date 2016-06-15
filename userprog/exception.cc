// exception.cc 
//  Entry point into the Nachos kernel from user programs.
//  There are two kinds of things that can cause control to
//  transfer back to here from user code:
//
//  syscall -- The user code explicitly requests to call a procedure
//  in the Nachos kernel.  Right now, the only function we support is
//  "Halt".
//
//  exceptions -- The user code does something that the CPU can't handle.
//  For instance, accessing memory that doesn't exist, arithmetic errors,
//  etc.  
//
//  Interrupts (which can also cause control to transfer from user
//  code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"


#include "read_write.h"

#include <string.h>
#include <time.h>


//----------------------------------------------------------------------
// ExceptionHandler
//   Entry point into the Nachos kernel.  Called when a user program
//  is executing, and either does a syscall, or generates an addressing
//  or arithmetic exception.
//
//   For system calls, the following is the calling convention:
//
//   system call code -- r2
//    arg1 -- r4
//    arg2 -- r5
//    arg3 -- r6
//    arg4 -- r7
//
//  The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//  "which" is the kind of exception.  The list of possible exceptions 
//  are in machine.h.
//----------------------------------------------------------------------

void startProc(void *) {
  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();
  currentThread->space->WriteArgs();
  
  machine->Run();
}



void incPC()
{
  int pc = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, pc);
  pc = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, pc);
  pc += 4;
  machine->WriteRegister(NextPCReg, pc);
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    
    if (which == SyscallException){
      switch(type){
        case SC_Halt:
            DEBUG('a', "Shutdown, initiated by user program.\n");
             interrupt->Halt();
             break;
         case SC_Create:
         {
             char buff[256];
             int vaddr = machine->ReadRegister(4);
             readStrFromUsr(vaddr, buff);
             fileSystem->Create(buff, 0);
             incPC();
             break;
         }
         case SC_Open:
         {   
             char buff[256];
             int vaddr = machine->ReadRegister(4);
             readStrFromUsr(vaddr, buff);
             OpenFile *file = fileSystem->Open(buff);
             int fd;
             if (file != NULL)
               fd = currentThread->AddFile(file);
             else
               fd = -1;
             machine->WriteRegister(2, fd);
             incPC();
             break;
         }
         case SC_Write: /* Revisar si quiere escribir en 0 */
         {
             int vaddr = machine->ReadRegister(4);
             int size  = machine->ReadRegister(5);
             int fd    = machine->ReadRegister(6);
             char *outBuff = new char[size];
             readBuffFromUsr(vaddr, outBuff, size);
             
             switch(fd){
               case ConsoleOutput:
              {
                 int i;
                 for (i = 0; i < size; i++) {
                   sConsole->Write(outBuff[i]);
                 }
                 break;
               }
               case ConsoleInput:
                 ASSERT(false);
                 break;
               default:
               {
                 OpenFile *file = currentThread->GetFile(fd);
                 file->Write(outBuff, size);
                 break;
               }
             }
             
             incPC();
             delete [] outBuff;
             break;
         }
         case SC_Read: /* Revisar si quiere leer en 1 */
         {
             int vaddr = machine->ReadRegister(4);
             int size  = machine->ReadRegister(5);
             int fd    = machine->ReadRegister(6);
             char *outBuff = new char[size];
             
             switch(fd){
               case ConsoleInput:
               {
                 int i;
                 for (i = 0; i < size; i++){
                   outBuff[i] = sConsole->Read();
                 }
                 writeBuffToUsr(outBuff, vaddr, size);
                 break;
               }
               case ConsoleOutput:
                 ASSERT(false);
                 break;
              default:
              {
                 OpenFile *file  = currentThread->GetFile(fd);
                 size  = file->Read(outBuff, size);
                 writeBuffToUsr(outBuff, vaddr, size);
                 break;
               }
             }
             machine->WriteRegister(2, size); /* retornar negativo si falló */
             incPC();
             delete [] outBuff;
             break;
         }
         case SC_Close:
         {
             int fd = machine->ReadRegister(4);
             OpenFile *file = currentThread->GetFile(fd);
             if (file != NULL)
             {
               currentThread->RemoveFile(file);
               delete file;
             }
             incPC();
             break;
         }
         case SC_Exit:
         {
             int status = machine->ReadRegister(4);
             currentThread->SetExitStatus(status);
             threadList->RemoveThread(currentThread);
             currentThread->Finish();
             break;
         }
         case SC_Join:
         {   
             SpaceId id = machine->ReadRegister(4);
             Thread *t = threadList->GetThread(id);
             int status = t != NULL ? t->Join() : -1; // devuelve -1 si el thread devolvió -1 ó si falló el join.
             machine->WriteRegister(2, status);
             incPC();
             break;
         }
         case SC_Exec:
         {   
             int addr = machine->ReadRegister(4);
             int addr_args = machine->ReadRegister(5);
             
             char **args = new char *[MAX_ARGS];
             
             char name[256];
             readStrFromUsr(addr, name);
             OpenFile *f = fileSystem->Open(name);
                          
             if (f != NULL)
             { /* file exists */
               int i = 0, a;
               if(addr_args != 0)
               { /* we have args! */
                 /* We read the args */
                 for(i = 0; i < MAX_ARGS; i++)
                 {
                   READ_MEM(addr_args + 4 * i, 4, &a);
                   if (a == 0)
                     break;
                   char temp[256];
                   readStrFromUsr(a, temp);
                   args[i] = new char [strlen(temp) + 1];
                   strcpy(args[i], temp);
                 }
               }
               
               AddrSpace *s = new AddrSpace(f);
               Thread *t = new Thread(name, 0);

               t->space = s;
               
               t->space->SetArgs(i, args);
               
               t->Fork(startProc, NULL);
               SpaceId pid = threadList->AddThread(t);
               machine->WriteRegister(2, pid);
               #ifndef DEMAND_LOADING /* if defined, keep it open */
               delete f;
               #endif //DEMAND_LOADING
               
             }
             else {
                machine->WriteRegister(2, -1);
             }
             incPC();
             break;
         }
         default:
           printf("Unexpected user mode exception %d %d\n", which, type);
           ASSERT(false);
           break;
      }
    } 
    #ifdef USE_TLB
    else if (which == PageFaultException) {
      
      stats->numPageFaults++;

      TranslationEntry *free;
      
      int missing = machine->ReadRegister(BadVAddrReg) / PageSize;
      
      #ifdef VM /* we have swap now */
      int where;
      TranslationEntry e = currentThread->space->GetEntry(missing);
      
      #ifdef DEMAND_LOADING
      if(!e.valid)
      { /* it's not loaded yet.. */
        where = currentThread->space->Load(missing); /* load from binary */
        e.physicalPage = where; /* e.vp now translates to where */
        e.valid = true; /* this translation is now valid */
      }
      #endif //DEMAND_LOADING
      
      if(e.physicalPage == -1) /* if DEMAND_LOADING is defined, first time load won't put it in swap, but could be in future */
      { /* it's in swap, get it */
        where = currentThread->space->SwapIn(missing);
        e.physicalPage = where; /* e.vp now translates to where */
      }
      
      currentThread->space->AddEntry(e); /* we update the pageTable in its AddrSpace, can happen that no changes were made, but still write it. is O(1). */
      #endif // VM
      
      int i;
      /* we search for free spot in tlb */
      for (free = NULL, i = 0; i < TLBSize; i++)
        if (!machine->tlb[i].valid)
        {
          free = &machine->tlb[i];
          break;
        }
      
      if (free == NULL) { /* none? random */
        int victim = Random() % TLBSize;
        free = &machine->tlb[victim]; 
        /* if the coming entry was on swap or binary, this next step is redundant.. but is O(1)... */
        currentThread->space->AddEntry(*free); /* save it, don't lose the bits. */
        /* if no VM, we have infinite memory, no modifications... */
      }
      
      #ifdef VM
      *free = e;
      #else
      *free = currentThread->space->GetEntry(missing);
      #endif
    }
    #endif
    else {
      printf("Unexpected user mode exception %d %d\n", which, type);
      ASSERT(false);
    }
}
