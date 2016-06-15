// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 
    
    void SetArgs(int _argc, char **_args) { argc = _argc; 
                                            args = _args; };
    void WriteArgs();
    
#ifdef USE_TLB
    void AddEntry(TranslationEntry e); 
    
    TranslationEntry GetEntry(int vpn) { return pageTable[vpn]; };
#endif

#ifdef VM
    void SwapOut(int vpn);
    
    int SwapIn(int missing);

    bool TestUse(int vpn) { return pageTable[vpn].use; };
    bool TestDirty(int vpn) { return pageTable[vpn].dirty; };
    void ClearUse(int vpn) { pageTable[vpn].use = false; }; /* second chance */
    
#ifdef DEMAND_LOADING
    int Load(int missing);
#endif // DEMAND_LOADING


#endif //VM


  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
	  char **args;
	  int argc;

#ifdef VM
    OpenFile *swap;
    bool *swapped_once;
    
#ifdef DEMAND_LOADING    
    OpenFile *exec; /* we will read the file later */
    int startMemPos;
#endif

#endif // VM
};

#endif // ADDRSPACE_H
