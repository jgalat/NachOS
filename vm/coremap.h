#ifndef COREMAP_H
#define COREMAP_H

#include "copyright.h"
#include "system.h"
#include "utility.h"

#include "strategy.h"

typedef struct {
  AddrSpace *owner; /* usamos 'this' y no tenemos que adivinar el thread */
  int virtualPage;
} CoreEntry;

class CoreMap{
	public: 
	
	CoreMap(int nitems, const char *_strategy);	
	~CoreMap();
	
	int Find(AddrSpace *as, int vpn);
	void Clear(int which);
	int NumClear() { return bm->NumClear(); };	
	bool Test(int which) { return bm->Test(which); };
	
	private:
	CoreEntry *entries;
	BitMap    *bm;
	Strategy  *strategy;
};

#endif // COREMAP_H
