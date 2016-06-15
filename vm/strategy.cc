#include "copyright.h"
#include "system.h"
#include "utility.h"

#include "strategy.h"


/* FIFO */
FIFO::FIFO()
{
  queue = new List<int>;
}

FIFO::~FIFO()
{
  delete queue;
}

int FIFO::Next(void*)
{
  if(queue->IsEmpty())
    return -1;
  else
    return queue->Remove();
}

void FIFO::Add(void *_data)
{
  int ppn = *((int*)_data);
  queue->Append(ppn); 
}

/* LRU */
LRU::LRU()
{
  queue = new List<int>;
}

LRU::~LRU()
{
  delete queue;
}

int LRU::Next(void*)
{
  if(queue->IsEmpty())
    return -1;
  else
    return queue->Remove();
}

void LRU::Add(void *_data)
{
  int ppn = *((int*) _data);
  queue->ItemRemove(ppn); /* we just want it out, if it isn't in there doesn't matter */
  queue->Append(ppn);
}


/* ESC */
ESC::ESC(int max)
{
  npp = max;
  victim = 0;
}

//ESC::~ESC() {}

void ESC::Add(void *)
{
  /* nothing */
}


/* based on http://courses.cs.tamu.edu/bart/cpsc410/Supplements/Slides/page-rep3.pdf page 3 
   http://uw.cs.cs350.narkive.com/58GQ1cDQ/enhanced-second-chance-replacement */
int ESC::Next(void *_data)
{
  CoreEntry *entries = (CoreEntry *)_data;
  AddrSpace *pointed;
  int i, ptr, vpn;
  
  
  /* first loop, look for (0,0), don't reset bits */
  for(i = 0, ptr = victim; i < npp; i++, ptr = (ptr + 1) % npp)
  {
    pointed = entries[ptr].owner;
    vpn     = entries[ptr].virtualPage;
    if((!pointed->TestUse(vpn)) && (!pointed->TestDirty(vpn))) /* (0,0) */
    {
      victim = (ptr + 1) % npp;
      return ptr; /* ptr page is (0,0) */
    }
  }
  
  /* second loop, we look for (0,1) while reseting bits */
  for(i = 0, ptr = victim; i < npp; i++, ptr = (ptr + 1) % npp)
  {
    pointed = entries[ptr].owner;
    vpn     = entries[ptr].virtualPage;
    if(pointed->TestUse(vpn)) /* (1,*) */
    {
      pointed->ClearUse(vpn); /* unset use bit -> (0,*) */
      continue;
    } else { /* (0,*) */ 
      if(pointed->TestDirty(vpn)) /* (0,1) */
      {
        victim = (ptr + 1) % npp;
        return ptr; /* ptr page is (0,1) */
      } /* no else, we know there aren't (0,0) still in this loop */
    }
  }
  
  /* third loop, we look for (0,0) or (0,1) */
  /* in the cycle we have only (0,0) and (0,1), all use bits were turned off */
  for(i = 0, ptr = victim; i < npp; i++, ptr = (ptr + 1) % npp)
  {
    pointed = entries[ptr].owner;
    vpn     = entries[ptr].virtualPage;
    if(!pointed->TestDirty(vpn)) /* (0,0) */
    {
      victim = (ptr + 1) % npp;
      return ptr; /* ptr page is (0,0) */
    }
  }
  
  /* no (0,0) found. victim is (0,1), return it. */
  int result = victim;
  victim = (victim + 1) % npp;
  return result;
}
