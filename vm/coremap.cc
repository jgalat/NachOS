#include "copyright.h"
#include "coremap.h"

#include <string.h>

CoreMap::CoreMap(int nitems, const char *_strategy)
{
  bm = new BitMap(nitems);
  entries = new CoreEntry[nitems];
  
  int i;
  
  for(i = 0; i < nitems; i++)
  {
    entries[i].owner = NULL;
    entries[i].virtualPage = -1;
  }
  
  strategy = NULL;
  
  if (strcmp("FIFO", _strategy) == 0)
    strategy = new FIFO();

  if (strcmp("LRU", _strategy) == 0)
    strategy = new LRU();

  if (strcmp("ESC", _strategy) == 0)
    strategy = new ESC(nitems);
    
  ASSERT(strategy != NULL);
}

CoreMap::~CoreMap()
{
    delete bm;
    delete [] entries;
}

int CoreMap::Find(AddrSpace *as, int vpn)
{
  int ppn = bm->Find();
  if (ppn >= 0)
  {
    entries[ppn].owner = as;
    entries[ppn].virtualPage = vpn;
    /* Find() already marked map[ppn] */
  }
  else /* no free page, ppn == -1 */ 
  { 
    if (currentThread->space == as) {
      as->SaveState(); /* we update the pageTable on current before anything, some strats needs an updated one (AMR) */
    }
    ppn = strategy->Next( (void*) entries); 
    AddrSpace *previousOwner = entries[ppn].owner;
    int previousVpn = entries[ppn].virtualPage;
    previousOwner->SwapOut(previousVpn);
    entries[ppn].owner = as;
    entries[ppn].virtualPage = vpn;
    /* bit in map was already marked by previousOwner */
  }
  
  strategy->Add((void*) &ppn);
  return ppn;
}

void CoreMap::Clear(int which)
{
  bm->Clear(which);
  entries[which].owner = NULL;
  entries[which].virtualPage = -1;
}


